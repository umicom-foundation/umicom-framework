/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/supervised_continuation.c
 * PURPOSE:
 *   Drive exact example-by-example native SGD continuation and atomically
 *   bind the independent adapter weights to the saved source-locked cursor.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/supervised_continuation.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define UMI_CONTINUATION_HEADER_BYTES 256U
#define UMI_CONTINUATION_TRAILER_BYTES 32U
static const uint8_t CONTINUATION_MAGIC[8] = {
    'U','M','I','S','F','T','C','R'
};

static int ContinuationPortableFloats(void)
{
    return sizeof(float) == 4U && sizeof(double) == 8U && FLT_RADIX == 2 &&
           FLT_MANT_DIG == 24 && FLT_MAX_EXP == 128 &&
           DBL_MANT_DIG == 53 && DBL_MAX_EXP == 1024;
}

static void ContinuationWrite32(uint8_t *p, uint32_t value)
{
    for (size_t i = 0U; i < 4U; ++i)
        p[i] = (uint8_t)(value >> (8U * i));
}

static void ContinuationWrite64(uint8_t *p, uint64_t value)
{
    for (size_t i = 0U; i < 8U; ++i)
        p[i] = (uint8_t)(value >> (8U * i));
}

static uint32_t ContinuationRead32(const uint8_t *p)
{
    uint32_t value = 0U;
    for (size_t i = 0U; i < 4U; ++i)
        value |= (uint32_t)p[i] << (i * 8U);
    return value;
}

static uint64_t ContinuationRead64(const uint8_t *p)
{
    uint64_t value = 0U;
    for (size_t i = 0U; i < 8U; ++i)
        value |= (uint64_t)p[i] << (i * 8U);
    return value;
}

static void ContinuationWriteFloat(uint8_t *p, float number)
{
    uint32_t bits = 0U;
    memcpy(&bits, &number, sizeof(bits));
    ContinuationWrite32(p, bits);
}

static void ContinuationWriteDouble(uint8_t *p, double number)
{
    uint64_t bits = 0U;
    memcpy(&bits, &number, sizeof(bits));
    ContinuationWrite64(p, bits);
}

static float ContinuationReadFloat(const uint8_t *p)
{
    uint32_t bits = ContinuationRead32(p);
    float number = 0.0F;
    memcpy(&number, &bits, sizeof(number));
    return number;
}

static double ContinuationReadDouble(const uint8_t *p)
{
    uint64_t bits = ContinuationRead64(p);
    double number = 0.0;
    memcpy(&number, &bits, sizeof(number));
    return number;
}

static UmiStatus ContinuationDigest(const uint8_t *bytes, size_t count,
                                    uint8_t digest[UMI_SHA256_BYTES])
{
    UmiSha256 sha;
    UmiSha256Init(&sha);
    UmiStatus status = UmiSha256Update(&sha, bytes, count);
    if (status != UMI_STATUS_OK) return status;
    return UmiSha256Final(&sha, digest);
}

static int ContinuationEvaluationValid(
    const UmiAiTrainingSupervisedEvaluation *evaluation)
{
    if (evaluation == NULL || evaluation->exampleCount == 0U ||
        evaluation->supervisedTokens == 0U ||
        evaluation->correctTopOneTokens > evaluation->supervisedTokens ||
        !isfinite(evaluation->meanCrossEntropy) ||
        evaluation->meanCrossEntropy < 0.0 ||
        !isfinite(evaluation->topOneAccuracy) ||
        evaluation->topOneAccuracy < 0.0 ||
        evaluation->topOneAccuracy > 1.0 ||
        (evaluation->perplexityAvailable != 0 &&
         evaluation->perplexityAvailable != 1)) return 0;
    if (evaluation->perplexityAvailable &&
        (!isfinite(evaluation->perplexity) || evaluation->perplexity < 1.0))
        return 0;
    return 1;
}

/* The source digest and frozen checkpoint are checked on EVERY public call:
 * even a caller-modified cursor cannot silently switch model or dataset.
 * Whole example epochs, not raw token windows, define SGD continuation.
 * One successful train call consumes exactly one deterministic index. */
static UmiStatus ContinuationValidate(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedContinuation *state)
{
    UmiAiTrainingDecoderProjectionAdapterInfo model = {0};
    UmiAiTrainingSupervisedDatasetInfo corpus = {0};
    UmiStatus status;
    if (adapter == NULL || dataset == NULL || state == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingSupervisedExperimentPlanValidate(&state->plan);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &model);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingSupervisedDatasetInspect(dataset, &corpus);
    if (status != UMI_STATUS_OK) return status;
    if (corpus.contextLength == 0U ||
        corpus.contextLength != model.decoder.contextLength ||
        corpus.trainingExamples == 0U || corpus.evaluationExamples == 0U ||
        corpus.trainingTargets == 0U || corpus.evaluationTargets == 0U ||
        state->selectedProjections != model.selectedProjections ||
        state->rank != model.rank ||
        state->trainingExamples != corpus.trainingExamples ||
        state->evaluationExamples != corpus.evaluationExamples ||
        memcmp(state->orderedCorpusSha256, corpus.orderedCorpusSha256, 32U) != 0 ||
        memcmp(state->frozenCheckpointSha256,
               model.frozenCheckpointSha256,
               UMI_AI_DECODER_ADAPTER_SHA256_HEX_CAPACITY) != 0 ||
        state->completedAdapterSteps != model.completedAdapterSteps ||
        state->completedEpochs > state->plan.epochs ||
        state->nextExampleOffset > state->trainingExamples ||
        !isfinite(state->totalWeightedPreUpdateLoss) ||
        state->totalWeightedPreUpdateLoss < 0.0 ||
        (state->finished != 0 && state->finished != 1) ||
        (state->stoppedAtTarget != 0 && state->stoppedAtTarget != 1) ||
        (state->hasHeldOut != 0 && state->hasHeldOut != 1))
        return UMI_STATUS_INVALID_STATE;
    if (state->hasHeldOut &&
        (!ContinuationEvaluationValid(&state->lastHeldOut) ||
         state->lastHeldOut.exampleCount != corpus.evaluationExamples ||
         state->lastHeldOut.supervisedTokens != corpus.evaluationTargets))
        return UMI_STATUS_INVALID_STATE;
    if (state->completedEpochs > UINT64_MAX / state->trainingExamples ||
        state->initialAdapterSteps > UINT64_MAX -
            state->completedEpochs * state->trainingExamples ||
        state->nextExampleOffset > UINT64_MAX - state->initialAdapterSteps -
            state->completedEpochs * state->trainingExamples)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    uint64_t expected = state->initialAdapterSteps +
        state->completedEpochs * state->trainingExamples +
        state->nextExampleOffset;
    if (expected != state->completedAdapterSteps ||
        (state->completedEpochs == state->plan.epochs && !state->finished) ||
        (state->finished &&
         (state->completedEpochs == 0U || state->nextExampleOffset != 0U)) ||
        (!state->finished && state->stoppedAtTarget))
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedContinuationBegin(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedExperimentPlan *plan,
    UmiAiTrainingSupervisedContinuation *outState)
{
    UmiAiTrainingDecoderProjectionAdapterInfo model = {0};
    UmiAiTrainingSupervisedDatasetInfo corpus = {0};
    UmiAiTrainingSupervisedContinuation candidate = {0};
    UmiStatus status;
    if (outState == NULL || adapter == NULL || dataset == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingSupervisedExperimentPlanValidate(plan);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderProjectionAdapterInspect(adapter, &model);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingSupervisedDatasetInspect(dataset, &corpus);
    if (status != UMI_STATUS_OK) return status;
    if (corpus.trainingExamples == 0U || corpus.evaluationExamples == 0U ||
        corpus.trainingTargets == 0U || corpus.evaluationTargets == 0U ||
        corpus.contextLength != model.decoder.contextLength ||
        corpus.trainingExamples > UINT64_MAX / plan->epochs ||
        model.completedAdapterSteps > UINT64_MAX -
            corpus.trainingExamples * plan->epochs)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate.plan = *plan;
    memcpy(candidate.orderedCorpusSha256, corpus.orderedCorpusSha256, 32U);
    memcpy(candidate.frozenCheckpointSha256,
           model.frozenCheckpointSha256,
           sizeof(candidate.frozenCheckpointSha256));
    candidate.selectedProjections = model.selectedProjections;
    candidate.rank = model.rank;
    candidate.trainingExamples = corpus.trainingExamples;
    candidate.evaluationExamples = corpus.evaluationExamples;
    candidate.initialAdapterSteps = model.completedAdapterSteps;
    candidate.completedAdapterSteps = model.completedAdapterSteps;
    *outState = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedContinuationInspect(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedContinuation *state,
    UmiAiTrainingSupervisedContinuation *outCopy)
{
    if (outCopy == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ContinuationValidate(adapter, dataset, state);
    if (status == UMI_STATUS_OK) *outCopy = *state;
    return status;
}

UmiStatus UmiAiTrainingSupervisedContinuationTrainNext(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedContinuation *state,
    UmiAiTrainingSupervisedProgress *outProgress)
{
    size_t order[UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES] = {0};
    size_t returned = 0U;
    UmiAiTrainingSupervisedExample example = {0};
    UmiAiTrainingDecoderProjectionStep step = {0};
    UmiAiTrainingSupervisedProgress event = {0};
    UmiAiTrainingSupervisedContinuation candidate;
    UmiStatus status;
    if (outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ContinuationValidate(adapter, dataset, state);
    if (status != UMI_STATUS_OK) return status;
    if (state->finished || state->completedEpochs == state->plan.epochs ||
        state->nextExampleOffset == state->trainingExamples)
        return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingSupervisedDatasetOrder(dataset,
        UMI_AI_TRAINING_PARTITION_TRAIN, state->plan.shuffleSeed,
        (uint64_t)state->completedEpochs, order,
        UMI_AI_SUPERVISED_DATASET_MAX_EXAMPLES, &returned);
    if (status != UMI_STATUS_OK) return status;
    if (returned != state->trainingExamples)
        return UMI_STATUS_INVALID_STATE;
    status = UmiAiTrainingSupervisedDatasetGet(dataset,
        order[state->nextExampleOffset], &example);
    if (status != UMI_STATUS_OK) return status;
    if (example.partition != UMI_AI_TRAINING_PARTITION_TRAIN ||
        example.supervisedTokens == 0U ||
        example.supervisedTokens > UMI_AI_SUPERVISED_DATASET_MAX_CONTEXT ||
        state->totalSupervisedTargets > SIZE_MAX - example.supervisedTokens)
        return UMI_STATUS_INVALID_STATE;
    if (state->completedAdapterSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* All input validation and capacity arithmetic precedes the SGD step.
     * One successful step produces exactly one checkpointable cursor move. */
    UmiAiTrainingSupervisedDatasetInfo corpus = {0};
    status = UmiAiTrainingSupervisedDatasetInspect(dataset, &corpus);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderProjectionAdapterTrainStep(adapter,
        example.inputIds, example.targetIds, example.lossMask,
        corpus.contextLength, state->plan.learningRate, &step);
    if (status != UMI_STATUS_OK) return status;
    if (step.completedAdapterSteps != state->completedAdapterSteps + 1U ||
        step.supervisedTokenCount != example.supervisedTokens ||
        !isfinite(step.selectedCrossEntropyBeforeUpdate) ||
        step.selectedCrossEntropyBeforeUpdate < 0.0F) {
        /* A successful underlying model update cannot be undone here;
         * leave cursor untouched and report the inconsistent owner. */
        return UMI_STATUS_INVALID_STATE;
    }
    candidate = *state;
    candidate.completedAdapterSteps = step.completedAdapterSteps;
    ++candidate.nextExampleOffset;
    candidate.totalSupervisedTargets += example.supervisedTokens;
    candidate.totalWeightedPreUpdateLoss +=
        (double)step.selectedCrossEntropyBeforeUpdate *
        (double)example.supervisedTokens;
    if (!isfinite(candidate.totalWeightedPreUpdateLoss))
        return UMI_STATUS_INVALID_STATE;
    event.epoch = candidate.completedEpochs + 1U;
    event.positionInEpoch = candidate.nextExampleOffset;
    event.supervisedTokens = example.supervisedTokens;
    event.crossEntropyBeforeUpdate = step.selectedCrossEntropyBeforeUpdate;
    event.completedAdapterSteps = candidate.completedAdapterSteps;
    *state = candidate;
    *outProgress = event;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedContinuationFinishEpoch(
    UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    UmiAiTrainingSupervisedContinuation *state,
    UmiAiTrainingSupervisedEpochResult *outResult)
{
    UmiAiTrainingSupervisedContinuation candidate;
    UmiAiTrainingSupervisedEpochResult report = {0};
    UmiStatus status;
    if (outResult == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ContinuationValidate(adapter, dataset, state);
    if (status != UMI_STATUS_OK) return status;
    if (state->finished || state->nextExampleOffset != state->trainingExamples ||
        state->completedEpochs >= state->plan.epochs)
        return UMI_STATUS_INVALID_STATE;
    candidate = *state;
    ++candidate.completedEpochs;
    candidate.nextExampleOffset = 0U;
    const int shouldEvaluate =
        candidate.completedEpochs % candidate.plan.evaluateEveryEpochs == 0U ||
        candidate.completedEpochs == candidate.plan.epochs;
    if (shouldEvaluate) {
        status = UmiAiTrainingSupervisedExperimentEvaluate(
            adapter, dataset, &candidate.lastHeldOut);
        if (status != UMI_STATUS_OK) return status;
        candidate.hasHeldOut = 1;
        if (candidate.plan.stopAtEvaluationLoss > 0.0 &&
            candidate.lastHeldOut.meanCrossEntropy <=
                candidate.plan.stopAtEvaluationLoss) {
            candidate.stoppedAtTarget = 1;
            candidate.finished = 1;
        }
    }
    if (candidate.completedEpochs == candidate.plan.epochs)
        candidate.finished = 1;
    report.completedEpochs = candidate.completedEpochs;
    report.totalSupervisedTargets = candidate.totalSupervisedTargets;
    if (candidate.totalSupervisedTargets != 0U)
        report.meanPreUpdateCrossEntropy =
            candidate.totalWeightedPreUpdateLoss /
            (double)candidate.totalSupervisedTargets;
    report.heldOutEvaluated = shouldEvaluate;
    report.finished = candidate.finished;
    report.stoppedAtTarget = candidate.stoppedAtTarget;
    if (shouldEvaluate) report.heldOut = candidate.lastHeldOut;
    *state = candidate;
    *outResult = report;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingSupervisedContinuationSnapshotSize(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedContinuation *state,
    size_t *outBytes)
{
    size_t inner = 0U;
    UmiStatus status;
    if (outBytes == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!ContinuationPortableFloats()) return UMI_STATUS_UNAVAILABLE;
    status = ContinuationValidate(adapter, dataset, state);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDecoderProjectionSnapshotSize(adapter, &inner);
    if (status != UMI_STATUS_OK) return status;
    if (inner > UINT32_MAX ||
        inner > UMI_AI_SUPERVISED_CONTINUATION_MAX_BYTES -
            UMI_CONTINUATION_HEADER_BYTES - UMI_CONTINUATION_TRAILER_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *outBytes = UMI_CONTINUATION_HEADER_BYTES + inner +
                UMI_CONTINUATION_TRAILER_BYTES;
    return UMI_STATUS_OK;
}

/* Portable header offsets are intentionally explicit, not sizeof(C struct).
 * All count fields have a fixed width and a fully zeroed reserved suffix.
 * Every snapshot contains its complete nested projection checkpoints. */
UmiStatus UmiAiTrainingSupervisedContinuationSnapshotEncode(
    const UmiAiTrainingDecoderProjectionAdapter *adapter,
    const UmiAiTrainingSupervisedDataset *dataset,
    const UmiAiTrainingSupervisedContinuation *state,
    uint8_t *outBytes, size_t capacity, size_t *outWritten)
{
    uint8_t *bytes = NULL;
    size_t needed = 0U, inner = 0U;
    UmiStatus status;
    if (outBytes == NULL || outWritten == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingSupervisedContinuationSnapshotSize(
        adapter, dataset, state, &needed);
    if (status != UMI_STATUS_OK) return status;
    if (needed > UINT32_MAX || capacity < needed)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    bytes = calloc(needed, 1U);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(bytes, CONTINUATION_MAGIC, 8U);
    ContinuationWrite32(bytes + 8U, UINT32_C(1));
    ContinuationWrite32(bytes + 12U, (uint32_t)needed);
    memcpy(bytes + 16U, state->orderedCorpusSha256, 32U);
    memcpy(bytes + 48U, state->frozenCheckpointSha256, 64U);
    ContinuationWrite32(bytes + 112U, (uint32_t)state->plan.epochs);
    ContinuationWrite32(bytes + 116U, (uint32_t)state->plan.evaluateEveryEpochs);
    ContinuationWrite64(bytes + 120U, state->plan.shuffleSeed);
    ContinuationWriteFloat(bytes + 128U, state->plan.learningRate);
    ContinuationWrite32(bytes + 132U, state->selectedProjections);
    ContinuationWrite32(bytes + 136U, (uint32_t)state->rank);
    ContinuationWrite32(bytes + 140U, (uint32_t)state->completedEpochs);
    ContinuationWrite32(bytes + 144U, (uint32_t)state->nextExampleOffset);
    ContinuationWrite32(bytes + 148U, (uint32_t)state->trainingExamples);
    ContinuationWrite32(bytes + 152U, (uint32_t)state->evaluationExamples);
    ContinuationWrite32(bytes + 156U, (uint32_t)state->finished |
        ((uint32_t)state->stoppedAtTarget << 1U));
    ContinuationWrite64(bytes + 160U, state->initialAdapterSteps);
    ContinuationWrite64(bytes + 168U, state->completedAdapterSteps);
    ContinuationWrite64(bytes + 176U, (uint64_t)state->totalSupervisedTargets);
    ContinuationWriteDouble(bytes + 184U, state->totalWeightedPreUpdateLoss);
    ContinuationWriteDouble(bytes + 192U, state->plan.stopAtEvaluationLoss);
    if (state->hasHeldOut) {
        const UmiAiTrainingSupervisedEvaluation *eval = &state->lastHeldOut;
        ContinuationWriteDouble(bytes + 200U, eval->meanCrossEntropy);
        ContinuationWriteDouble(bytes + 208U, eval->topOneAccuracy);
        ContinuationWrite32(bytes + 216U, UINT32_C(1));
        ContinuationWrite32(bytes + 220U, (uint32_t)eval->exampleCount);
        ContinuationWrite32(bytes + 224U, (uint32_t)eval->supervisedTokens);
        ContinuationWrite32(bytes + 228U, (uint32_t)eval->correctTopOneTokens);
        ContinuationWriteDouble(bytes + 232U, eval->perplexity);
        ContinuationWrite32(bytes + 240U,
                            (uint32_t)eval->perplexityAvailable);
    }
    ContinuationWrite32(bytes + 244U,
        (uint32_t)(needed - UMI_CONTINUATION_HEADER_BYTES -
                        UMI_CONTINUATION_TRAILER_BYTES));
    /* Header bytes [248,256) must stay zero. */
    status = UmiAiTrainingDecoderProjectionSnapshotEncode(adapter,
        bytes + UMI_CONTINUATION_HEADER_BYTES,
        needed - UMI_CONTINUATION_HEADER_BYTES -
                 UMI_CONTINUATION_TRAILER_BYTES, &inner);
    if (status == UMI_STATUS_OK &&
        inner != needed - UMI_CONTINUATION_HEADER_BYTES -
                          UMI_CONTINUATION_TRAILER_BYTES)
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = ContinuationDigest(bytes,
            needed - UMI_CONTINUATION_TRAILER_BYTES,
            bytes + needed - UMI_CONTINUATION_TRAILER_BYTES);
    if (status == UMI_STATUS_OK) {
        memcpy(outBytes, bytes, needed);
        *outWritten = needed;
    }
    free(bytes);
    return status;
}

UmiStatus UmiAiTrainingSupervisedContinuationSnapshotRestore(
    UmiAiTrainingDecoder *frozenDecoder,
    const UmiAiTrainingSupervisedDataset *dataset,
    const uint8_t *bytes, size_t byteCount,
    UmiAiTrainingDecoderProjectionAdapter **outAdapter,
    UmiAiTrainingSupervisedContinuation *outState)
{
    UmiAiTrainingSupervisedContinuation candidate = {0};
    UmiAiTrainingDecoderProjectionAdapter *restored = NULL;
    UmiAiTrainingSupervisedDatasetInfo corpus = {0};
    uint8_t digest[UMI_SHA256_BYTES] = {0};
    UmiStatus status;
    size_t childBytes;
    uint32_t flags;
    if (frozenDecoder == NULL || dataset == NULL || bytes == NULL ||
        outAdapter == NULL || *outAdapter != NULL || outState == NULL ||
        byteCount < UMI_CONTINUATION_HEADER_BYTES +
                    UMI_CONTINUATION_TRAILER_BYTES + 1U ||
        byteCount > UMI_AI_SUPERVISED_CONTINUATION_MAX_BYTES)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!ContinuationPortableFloats()) return UMI_STATUS_UNAVAILABLE;
    if (memcmp(bytes, CONTINUATION_MAGIC, 8U) != 0 ||
        ContinuationRead32(bytes + 8U) != 1U ||
        ContinuationRead32(bytes + 12U) != byteCount)
        return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 248U; i < 256U; ++i)
        if (bytes[i] != 0U) return UMI_STATUS_PARSE_ERROR;
    childBytes = (size_t)ContinuationRead32(bytes + 244U);
    if (childBytes == 0U ||
        childBytes != byteCount - UMI_CONTINUATION_HEADER_BYTES -
                                UMI_CONTINUATION_TRAILER_BYTES)
        return UMI_STATUS_PARSE_ERROR;
    status = ContinuationDigest(bytes,
        byteCount - UMI_CONTINUATION_TRAILER_BYTES, digest);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(digest, bytes + byteCount -
                       UMI_CONTINUATION_TRAILER_BYTES, sizeof(digest)) != 0)
        return UMI_STATUS_PARSE_ERROR;
    memcpy(candidate.orderedCorpusSha256, bytes + 16U, 32U);
    memcpy(candidate.frozenCheckpointSha256, bytes + 48U, 64U);
    candidate.frozenCheckpointSha256[64] = '\0';
    candidate.plan.epochs = (size_t)ContinuationRead32(bytes + 112U);
    candidate.plan.evaluateEveryEpochs =
        (size_t)ContinuationRead32(bytes + 116U);
    candidate.plan.shuffleSeed = ContinuationRead64(bytes + 120U);
    candidate.plan.learningRate = ContinuationReadFloat(bytes + 128U);
    candidate.selectedProjections = ContinuationRead32(bytes + 132U);
    candidate.rank = (size_t)ContinuationRead32(bytes + 136U);
    candidate.completedEpochs = (size_t)ContinuationRead32(bytes + 140U);
    candidate.nextExampleOffset = (size_t)ContinuationRead32(bytes + 144U);
    candidate.trainingExamples = (size_t)ContinuationRead32(bytes + 148U);
    candidate.evaluationExamples = (size_t)ContinuationRead32(bytes + 152U);
    flags = ContinuationRead32(bytes + 156U);
    if (flags > 3U) return UMI_STATUS_PARSE_ERROR;
    candidate.finished = (int)(flags & 1U);
    candidate.stoppedAtTarget = (int)((flags >> 1U) & 1U);
    candidate.initialAdapterSteps = ContinuationRead64(bytes + 160U);
    candidate.completedAdapterSteps = ContinuationRead64(bytes + 168U);
    if (ContinuationRead64(bytes + 176U) > SIZE_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.totalSupervisedTargets =
        (size_t)ContinuationRead64(bytes + 176U);
    candidate.totalWeightedPreUpdateLoss =
        ContinuationReadDouble(bytes + 184U);
    candidate.plan.stopAtEvaluationLoss =
        ContinuationReadDouble(bytes + 192U);
    flags = ContinuationRead32(bytes + 216U);
    if (flags > 1U) return UMI_STATUS_PARSE_ERROR;
    candidate.hasHeldOut = (int)flags;
    candidate.lastHeldOut.meanCrossEntropy =
        ContinuationReadDouble(bytes + 200U);
    candidate.lastHeldOut.topOneAccuracy =
        ContinuationReadDouble(bytes + 208U);
    candidate.lastHeldOut.exampleCount =
        (size_t)ContinuationRead32(bytes + 220U);
    candidate.lastHeldOut.supervisedTokens =
        (size_t)ContinuationRead32(bytes + 224U);
    candidate.lastHeldOut.correctTopOneTokens =
        (size_t)ContinuationRead32(bytes + 228U);
    candidate.lastHeldOut.perplexity =
        ContinuationReadDouble(bytes + 232U);
    candidate.lastHeldOut.perplexityAvailable =
        (int)ContinuationRead32(bytes + 240U);
    if (!candidate.hasHeldOut) {
        for (size_t i = 200U; i < 244U; ++i)
            if (bytes[i] != 0U) return UMI_STATUS_PARSE_ERROR;
    }
    status = UmiAiTrainingSupervisedExperimentPlanValidate(&candidate.plan);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingSupervisedDatasetInspect(dataset, &corpus);
    if (status != UMI_STATUS_OK) return status;
    if (memcmp(candidate.orderedCorpusSha256,
               corpus.orderedCorpusSha256, 32U) != 0 ||
        candidate.trainingExamples != corpus.trainingExamples ||
        candidate.evaluationExamples != corpus.evaluationExamples)
        return UMI_STATUS_INVALID_STATE;
    /* Restore is performed only after complete outer format and source checks;
     * the nested adapter checks its own SHA and the live frozen model digest. */
    status = UmiAiTrainingDecoderProjectionSnapshotRestore(frozenDecoder,
        bytes + UMI_CONTINUATION_HEADER_BYTES, childBytes, &restored);
    if (status != UMI_STATUS_OK) return status;
    status = ContinuationValidate(restored, dataset, &candidate);
    if (status == UMI_STATUS_OK) {
        *outAdapter = restored;
        *outState = candidate;
        return UMI_STATUS_OK;
    }
    UmiAiTrainingDecoderProjectionAdapterDestroy(restored);
    return status;
}
