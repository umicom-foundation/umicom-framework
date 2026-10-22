/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/experiment_adamw.c
 *
 * PURPOSE:
 *   Execute reproducible held-out AdamW experiments and exact optimiser
 *   continuation using the Framework's canonical model and checkpoint code.
 *
 * LIFETIME AND TRANSACTION:
 *   A staged decoder and its AdamW moment state stay private until success.
 *   Each micro-operation runs through the original decoder/tape services;
 *   the sampler and evaluation data never leave their reviewed partitions.
 *   Read-only resumed checkpoints are cloned before ANY training operation.
 *   The original SGD experiment and decoder checkpoint stay unchanged.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment_adamw.h"
#include "umicom/ai/training/optimizer_checkpoint.h"
#include "umicom/ai/training/dataset_manifest.h"
#include "umicom/ai/training/gradient_accumulation.h"
#include "optimizer_internal.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static bool SameDecoder(const UmiAiTrainingDecoderConfig *a,
                        const UmiAiTrainingDecoderConfig *b)
{
    return a->vocabularySize == b->vocabularySize &&
           a->contextLength == b->contextLength &&
           a->channels == b->channels &&
           a->hiddenChannels == b->hiddenChannels &&
           a->attentionHeads == b->attentionHeads &&
           a->initialSeed == b->initialSeed;
}

static bool SameAdamW(const UmiAiTrainingAdamWConfig *a,
                      const UmiAiTrainingAdamWConfig *b)
{
    return a->beta1 == b->beta1 && a->beta2 == b->beta2 &&
           a->epsilon == b->epsilon && a->weightDecay == b->weightDecay;
}

UmiStatus UmiAiTrainingExperimentAdamWValidate(
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule)
{
    UmiStatus status = UmiAiTrainingExperimentValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (optimizerOptions == NULL || schedule == NULL ||
        !isfinite(optimizerOptions->beta1) ||
        !isfinite(optimizerOptions->beta2) ||
        !isfinite(optimizerOptions->epsilon) ||
        !isfinite(optimizerOptions->weightDecay) ||
        optimizerOptions->beta1 < 0.0F || optimizerOptions->beta1 >= 1.0F ||
        optimizerOptions->beta2 < 0.0F || optimizerOptions->beta2 >= 1.0F ||
        optimizerOptions->epsilon <= 0.0F ||
        optimizerOptions->weightDecay < 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingLearningRateValidate(schedule);
    if (status != UMI_STATUS_OK) return status;
    if (schedule->totalSteps < (uint64_t)config->steps ||
        config->learningRate != schedule->peakLearningRate)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Validate that even the first warmed-up step is representable in FP32. */
    float initialRate = 0.0F;
    return UmiAiTrainingLearningRateForStep(schedule, UINT64_C(1),
                                           &initialRate);
}

/* A pair is cloned through the approved combined codec, never by a shallow
 * copy of private decoder tensors or AdamW moment pointers. */
static UmiStatus ClonePair(
    const UmiAiTrainingDecoder *sourceModel,
    const UmiAiTrainingAdamW *sourceOptimizer,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer)
{
    uint8_t *bytes = NULL;
    size_t size = 0U, written = 0U;
    UmiStatus status = UmiAiTrainingAdamWCheckpointSize(
        sourceModel, sourceOptimizer, &size);
    if (status != UMI_STATUS_OK) return status;
    if (size == 0U || size > UMI_AI_ADAMW_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    bytes = malloc(size);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingAdamWCheckpointEncode(
        sourceModel, sourceOptimizer, bytes, size, &written);
    if (status == UMI_STATUS_OK && written == size) {
        status = UmiAiTrainingAdamWCheckpointDecode(
            bytes, written, outModel, outOptimizer);
    } else if (status == UMI_STATUS_OK) {
        status = UMI_STATUS_INVALID_STATE;
    }
    free(bytes);
    return status;
}

static UmiStatus RunInternal(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingAdamW *checkpointOptimizer,
    size_t microbatchSequences,
    float maximumGradientNorm,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult,
    UmiAiTrainingGradientClipReport *outLastClipReport)
{
    UmiAiTrainingDecoder *model = NULL;
    UmiAiTrainingAdamW *optimizer = NULL;
    UmiAiTrainingExperimentResult report = {0};
    UmiAiTrainingExperimentProgress progress = {0};
    UmiAiTrainingGradientClipReport lastClip = {0};
    UmiAiTrainingDecoderConfig actual;
    uint64_t startSteps = UINT64_C(0), modelSteps = UINT64_C(0);
    uint64_t optimizerSteps = UINT64_C(0);
    size_t stateBytes = 0U;
    const size_t context = config != NULL ?
                           (size_t)config->decoder.contextLength : 0U;
    const size_t count = config != NULL ?
                         context * (size_t)config->batchSequences : 0U;
    size_t inputs[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    size_t targets[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    UmiStatus status;

    if (dataset == NULL || outModel == NULL || outOptimizer == NULL ||
        outResult == NULL ||
        (checkpointModel == NULL) != (checkpointOptimizer == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!isfinite(maximumGradientNorm) || maximumGradientNorm < 0.0F ||
        (maximumGradientNorm > 0.0F && outLastClipReport == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingExperimentAdamWValidate(
        config, optimizerOptions, schedule);
    if (status != UMI_STATUS_OK) return status;
    if (microbatchSequences != 0U &&
        microbatchSequences > (size_t)config->batchSequences)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (maximumGradientNorm > 0.0F && microbatchSequences == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Validate metadata before allocation; none of these calls execute data. */
    status = UmiAiTrainingDatasetInspect(dataset, &report.dataset);
    if (status != UMI_STATUS_OK) return status;
    if (report.dataset.trainingDocuments == 0U ||
        report.dataset.evaluationDocuments == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;

    if (checkpointModel != NULL) {
        UmiAiTrainingAdamWConfig recordedOptions;
        const size_t *parameterCounts = NULL;
        const double *firstMoment = NULL, *secondMoment = NULL;
        size_t parameters = 0U, elements = 0U;
        /* The checkpoint codec checks that model and moment states are
         * bound and their completed-step counters agree. */
        status = UmiAiTrainingAdamWCheckpointSize(
            checkpointModel, checkpointOptimizer, &stateBytes);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingDecoderInspect(
            checkpointModel, &actual, &startSteps);
        if (status != UMI_STATUS_OK) return status;
        if (!SameDecoder(&actual, &config->decoder) ||
            startSteps > (uint64_t)config->steps)
            return UMI_STATUS_INVALID_ARGUMENT;
        status = UmiAiTrainingAdamWInternalInspect(
            checkpointOptimizer, &recordedOptions, &optimizerSteps,
            &parameters, &elements, &parameterCounts,
            &firstMoment, &secondMoment);
        if (status != UMI_STATUS_OK) return status;
        if (optimizerSteps != startSteps ||
            !SameAdamW(&recordedOptions, optimizerOptions))
            return UMI_STATUS_INVALID_ARGUMENT;
        status = ClonePair(checkpointModel, checkpointOptimizer,
                           &model, &optimizer);
        if (status != UMI_STATUS_OK) goto fail;
    } else {
        status = UmiAiTrainingDecoderCreate(&config->decoder, &model);
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingDecoderAdamWCreate(
            model, optimizerOptions, &optimizer);
        if (status != UMI_STATUS_OK) goto fail;
    }

    status = UmiAiTrainingExperimentEvaluate(
        model, dataset, config,
        &progress.heldOutCrossEntropy, &progress.heldOutPerplexity);
    if (status != UMI_STATUS_OK) goto fail;
    progress.completedSteps = startSteps;
    progress.hasTrainingLoss = 0;
    report.baselineHeldOutLoss = progress.heldOutCrossEntropy;
    report.finalHeldOutLoss = progress.heldOutCrossEntropy;
    report.bestObservedHeldOutLoss = progress.heldOutCrossEntropy;
    report.bestObservedStep = startSteps;
    report.evaluationCount = 1U;
    if (observer != NULL) {
        status = observer(&progress, userData);
        if (status != UMI_STATUS_OK) goto fail;
    }

    for (uint64_t step = startSteps + UINT64_C(1);
         step <= (uint64_t)config->steps; ++step) {
        float learningRate = 0.0F;
        status = UmiAiTrainingDatasetBatch(
            dataset, UMI_AI_TRAINING_PARTITION_TRAIN, context,
            (size_t)config->batchSequences, config->trainSeed,
            step - UINT64_C(1), inputs, targets, count);
        if (status != UMI_STATUS_OK) goto fail;
        status = UmiAiTrainingLearningRateForStep(
            schedule, step, &learningRate);
        if (status != UMI_STATUS_OK) goto fail;
        if (maximumGradientNorm > 0.0F) {
            /* New opt-in path: weight all microbatches before one global
             * clipping decision and the original atomic AdamW transaction. */
            status = UmiAiTrainingDecoderTrainAdamWAccumulatedClipped(
                model, optimizer, inputs, targets, count,
                microbatchSequences, learningRate, maximumGradientNorm,
                &progress.lastTrainingLoss, &lastClip);
        } else if (microbatchSequences == 0U) {
            /* Preserve the existing full-batch AdamW path for callers that
             * do not explicitly request microbatch training. */
            status = UmiAiTrainingDecoderTrainAdamWStep(
                model, optimizer, inputs, targets, count,
                learningRate, &progress.lastTrainingLoss);
        } else {
            status = UmiAiTrainingDecoderTrainAdamWAccumulated(
                model, optimizer, inputs, targets, count,
                microbatchSequences, learningRate, &progress.lastTrainingLoss);
        }
        if (status != UMI_STATUS_OK) goto fail;
        if (!isfinite(progress.lastTrainingLoss) ||
            progress.lastTrainingLoss < 0.0F) {
            status = UMI_STATUS_INVALID_STATE;
            goto fail;
        }
        progress.completedSteps = step;
        progress.hasTrainingLoss = 1;
        if ((step % (uint64_t)config->evalInterval == 0U) ||
            step == (uint64_t)config->steps) {
            status = UmiAiTrainingExperimentEvaluate(
                model, dataset, config,
                &progress.heldOutCrossEntropy, &progress.heldOutPerplexity);
            if (status != UMI_STATUS_OK) goto fail;
            report.evaluationCount += 1U;
            report.finalHeldOutLoss = progress.heldOutCrossEntropy;
            if (progress.heldOutCrossEntropy < report.bestObservedHeldOutLoss) {
                report.bestObservedHeldOutLoss = progress.heldOutCrossEntropy;
                report.bestObservedStep = step;
            }
            if (observer != NULL) {
                status = observer(&progress, userData);
                if (status != UMI_STATUS_OK) goto fail;
            }
        }
    }
    status = UmiAiTrainingDecoderInspect(model, &actual, &modelSteps);
    if (status != UMI_STATUS_OK) goto fail;
    status = UmiAiTrainingAdamWInspect(optimizer, &optimizerSteps, &stateBytes);
    if (status != UMI_STATUS_OK) goto fail;
    if (modelSteps != (uint64_t)config->steps ||
        optimizerSteps != modelSteps ||
        !SameDecoder(&actual, &config->decoder)) {
        status = UMI_STATUS_INVALID_STATE;
        goto fail;
    }
    report.completedSteps = modelSteps;
    report.lastTrainingLoss = progress.lastTrainingLoss;
    *outModel = model;
    *outOptimizer = optimizer;
    *outResult = report;
    if (outLastClipReport != NULL) *outLastClipReport = lastClip;
    return UMI_STATUS_OK;
fail:
    UmiAiTrainingAdamWDestroy(optimizer);
    UmiAiTrainingDecoderDestroy(model);
    return status;
}

UmiStatus UmiAiTrainingExperimentRunAdamW(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult)
{
    return RunInternal(dataset, config, optimizerOptions, schedule, NULL, NULL,
                       0U, 0.0F, observer, userData,
                       outModel, outOptimizer, outResult, NULL);
}

UmiStatus UmiAiTrainingExperimentResumeAdamW(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingAdamW *checkpointOptimizer,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult)
{
    if (checkpointModel == NULL || checkpointOptimizer == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, optimizerOptions, schedule,
                       checkpointModel, checkpointOptimizer, 0U, 0.0F, observer,
                       userData, outModel, outOptimizer, outResult, NULL);
}

/* Effective-batch AdamW experiments preserve the published full-batch APIs.
 * The same deterministic source draw and absolute-step learning-rate schedule
 * are used regardless of how the gradients are partitioned across graphs. */
UmiStatus UmiAiTrainingExperimentRunAdamWAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    size_t microbatchSequences,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult)
{
    if (microbatchSequences == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, optimizerOptions, schedule, NULL, NULL,
                       microbatchSequences, 0.0F, observer, userData,
                       outModel, outOptimizer, outResult, NULL);
}

UmiStatus UmiAiTrainingExperimentResumeAdamWAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingAdamW *checkpointOptimizer,
    size_t microbatchSequences,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult)
{
    if (checkpointModel == NULL || checkpointOptimizer == NULL ||
        microbatchSequences == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, optimizerOptions, schedule,
                       checkpointModel, checkpointOptimizer,
                       microbatchSequences, 0.0F, observer, userData,
                       outModel, outOptimizer, outResult, NULL);
}

/* Optional stability policy composes the same existing training loop;
 * no application-side optimiser or alternate sampler has been introduced. */
UmiStatus UmiAiTrainingExperimentRunAdamWClipped(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    size_t microbatchSequences, float maximumGradientNorm,
    UmiAiTrainingExperimentObserver observer, void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult,
    UmiAiTrainingGradientClipReport *outLastClipReport)
{
    if (!isfinite(maximumGradientNorm) || maximumGradientNorm <= 0.0F ||
        microbatchSequences == 0U || outLastClipReport == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, optimizerOptions, schedule, NULL, NULL,
                       microbatchSequences, maximumGradientNorm,
                       observer, userData, outModel, outOptimizer,
                       outResult, outLastClipReport);
}

UmiStatus UmiAiTrainingExperimentResumeAdamWClipped(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingAdamWConfig *optimizerOptions,
    const UmiAiTrainingLearningRateSchedule *schedule,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingAdamW *checkpointOptimizer,
    size_t microbatchSequences, float maximumGradientNorm,
    UmiAiTrainingExperimentObserver observer, void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingAdamW **outOptimizer,
    UmiAiTrainingExperimentResult *outResult,
    UmiAiTrainingGradientClipReport *outLastClipReport)
{
    if (checkpointModel == NULL || checkpointOptimizer == NULL ||
        !isfinite(maximumGradientNorm) || maximumGradientNorm <= 0.0F ||
        microbatchSequences == 0U || outLastClipReport == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, optimizerOptions, schedule,
                       checkpointModel, checkpointOptimizer,
                       microbatchSequences, maximumGradientNorm,
                       observer, userData, outModel, outOptimizer,
                       outResult, outLastClipReport);
}
