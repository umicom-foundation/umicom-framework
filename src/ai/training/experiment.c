/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/experiment.c
 *
 * PURPOSE:
 *   Run repeatable source-code decoder experiments with file-level holdout,
 *   deterministic window selection and sampled cross-entropy evidence.
 *
 * ENGINEERING:
 *   Reuse dataset_manifest.c for immutable source data and reproducible
 *   sampling, and decoder.c for all neural maths and SGD. This file does not
 *   implement its own tokenizer, model, gradient engine or checkpoint codec.
 *   It publishes no model or report if training/evaluation is interrupted.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/experiment.h"
#include "umicom/ai/training/experiment_accumulation.h"
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"
#include "umicom/ai/training/tape.h"

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static size_t TokenCount(const UmiAiTrainingExperimentConfig *config)
{
    return (size_t)config->decoder.contextLength *
           (size_t)config->batchSequences;
}

UmiStatus UmiAiTrainingExperimentDefaults(
    UmiAiTrainingExperimentConfig *outConfig)
{
    UmiAiTrainingExperimentConfig candidate = {
        .decoder = {
            .vocabularySize = UMI_AI_BYTE_VOCABULARY,
            .contextLength = 8U,
            .channels = 8U,
            .hiddenChannels = 16U,
            .attentionHeads = 2U,
            .initialSeed = UINT64_C(20261021)
        },
        .steps = 64U,
        .evalInterval = 16U,
        .evalBatches = 5U,
        .batchSequences = 1U,
        .learningRate = 0.025F,
        .trainSeed = UINT64_C(4477),
        .evalSeed = UINT64_C(5544)
    };
    if (outConfig == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outConfig = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingExperimentValidate(
    const UmiAiTrainingExperimentConfig *config)
{
    size_t tokens;
    if (config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (config->steps == 0U || config->steps > UMI_AI_EXPERIMENT_MAX_STEPS ||
        config->evalInterval == 0U ||
        config->evalInterval > config->steps ||
        config->evalBatches == 0U ||
        config->evalBatches > UMI_AI_EXPERIMENT_MAX_EVALUATION_BATCHES ||
        config->batchSequences == 0U ||
        config->batchSequences > UMI_AI_DATASET_MAX_SEQUENCES ||
        !isfinite(config->learningRate) ||
        config->learningRate <= 0.0F || config->learningRate > 1.0F ||
        config->decoder.vocabularySize != UMI_AI_BYTE_VOCABULARY ||
        config->decoder.contextLength == 0U ||
        config->decoder.contextLength > UMI_AI_DECODER_MAX_CONTEXT ||
        config->decoder.contextLength > UMI_AI_DATASET_MAX_CONTEXT ||
        config->decoder.channels == 0U ||
        config->decoder.channels > UMI_AI_DECODER_MAX_CHANNELS ||
        config->decoder.hiddenChannels == 0U ||
        config->decoder.hiddenChannels > UMI_AI_DECODER_MAX_HIDDEN_CHANNELS ||
        config->decoder.attentionHeads == 0U ||
        config->decoder.attentionHeads > config->decoder.channels ||
        config->decoder.channels % config->decoder.attentionHeads != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    tokens = TokenCount(config);
    /* The current float32 tape bounds the logits matrix and attention
     * intermediates. Refuse impossible mini-batches before allocation. */
    if (tokens > UMI_AI_TRAINING_MAX_ELEMENTS /
                 (size_t)config->decoder.vocabularySize) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingExperimentEvaluate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    double *outMeanLoss, double *outPerplexity)
{
    size_t inputs[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    size_t targets[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    double sum = 0.0;
    double mean, perplexity;
    size_t tokens;
    UmiStatus status;
    if (model == NULL || dataset == NULL || outMeanLoss == NULL ||
        outPerplexity == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingExperimentValidate(config);
    if (status != UMI_STATUS_OK) return status;
    tokens = TokenCount(config);
    for (uint32_t batch = 0U; batch < config->evalBatches; ++batch) {
        float loss = 0.0F;
        status = UmiAiTrainingDatasetBatch(
            dataset, UMI_AI_TRAINING_PARTITION_EVALUATE,
            (size_t)config->decoder.contextLength,
            (size_t)config->batchSequences,
            config->evalSeed, (uint64_t)batch,
            inputs, targets, tokens);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingDecoderEvaluate(model, inputs, targets,
                                              tokens, &loss);
        if (status != UMI_STATUS_OK) return status;
        if (!isfinite(loss) || loss < 0.0F) return UMI_STATUS_INVALID_STATE;
        sum += (double)loss;
    }
    mean = sum / (double)config->evalBatches;
    perplexity = exp(mean);
    if (!isfinite(mean) || !isfinite(perplexity))
        return UMI_STATUS_INVALID_STATE;
    *outMeanLoss = mean;
    *outPerplexity = perplexity;
    return UMI_STATUS_OK;
}

/* The optional training batch is larger than the dataset sampler's fixed
 * per-call limit. Draw it in independently reproducible whole-context chunks.
 * All IDs are selected before decoder training begins, so source failure
 * cannot leave a partially updated model. Absolute draw identifiers allow
 * checkpoint continuation without a separate RNG state or format change. */
static UmiStatus DrawEffectiveBatch(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingExperimentAccumulation *accumulation,
    uint64_t absoluteStepIndex,
    size_t *inputs, size_t *targets)
{
    size_t drawn = 0U;
    const size_t context = (size_t)config->decoder.contextLength;
    const size_t effective = (size_t)accumulation->effectiveSequences;
    const uint64_t drawsPerStep =
        (uint64_t)((effective + UMI_AI_DATASET_MAX_SEQUENCES - 1U) /
                   UMI_AI_DATASET_MAX_SEQUENCES);
    uint64_t draw = 0U;
    while (drawn < effective) {
        size_t remaining = effective - drawn;
        size_t sequences = remaining < UMI_AI_DATASET_MAX_SEQUENCES ?
                           remaining : UMI_AI_DATASET_MAX_SEQUENCES;
        size_t count = sequences * context;
        UmiStatus status = UmiAiTrainingDatasetBatch(
            dataset, UMI_AI_TRAINING_PARTITION_TRAIN, context, sequences,
            config->trainSeed, absoluteStepIndex * drawsPerStep + draw,
            inputs + drawn * context, targets + drawn * context, count);
        if (status != UMI_STATUS_OK) return status;
        drawn += sequences;
        ++draw;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingExperimentAccumulationValidate(
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingExperimentAccumulation *accumulation)
{
    UmiAiTrainingCapacityPlan plan;
    UmiStatus status = UmiAiTrainingExperimentValidate(config);
    if (status != UMI_STATUS_OK) return status;
    if (accumulation == NULL || accumulation->effectiveSequences == 0U ||
        accumulation->microbatchSequences == 0U ||
        accumulation->effectiveSequences > UMI_AI_EXPERIMENT_MAX_EFFECTIVE_SEQUENCES ||
        accumulation->microbatchSequences > accumulation->effectiveSequences ||
        (size_t)accumulation->effectiveSequences >
            UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS /
            (size_t)config->decoder.contextLength)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Inspect the real decoder parameter and tensor shapes; do not create
     * a second model definition or depend on available host RAM estimates. */
    status = UmiAiTrainingCapacityPlanCompute(&config->decoder,
                   (size_t)accumulation->microbatchSequences, &plan);
    return status;
}

/* Clone through the canonical portable codec. No second parameter layout or
 * unsupported pointer aliasing of the Framework decoder is introduced. */
static UmiStatus CloneDecoder(const UmiAiTrainingDecoder *source,
                              UmiAiTrainingDecoder **outClone)
{
    uint8_t *bytes;
    size_t capacity = 0U, written = 0U;
    UmiStatus status = UmiAiTrainingDecoderCheckpointSize(source, &capacity);
    if (status != UMI_STATUS_OK) return status;
    if (capacity == 0U || capacity > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    bytes = malloc(capacity);
    if (bytes == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingDecoderCheckpointEncode(source, bytes, capacity,
                                                  &written);
    if (status == UMI_STATUS_OK && written == capacity) {
        status = UmiAiTrainingDecoderCheckpointDecode(bytes, written, outClone);
    } else if (status == UMI_STATUS_OK) {
        status = UMI_STATUS_INVALID_STATE;
    }
    free(bytes);
    return status;
}

static bool SameArchitecture(const UmiAiTrainingDecoderConfig *left,
                              const UmiAiTrainingDecoderConfig *right)
{
    return left->vocabularySize == right->vocabularySize &&
           left->contextLength == right->contextLength &&
           left->channels == right->channels &&
           left->hiddenChannels == right->hiddenChannels &&
           left->attentionHeads == right->attentionHeads &&
           left->initialSeed == right->initialSeed;
}

static UmiStatus RunInternal(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingDecoder *checkpointModel,
    const UmiAiTrainingExperimentAccumulation *accumulation,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult)
{
    UmiAiTrainingDatasetSummary datasetSummary;
    UmiAiTrainingDecoder *created = NULL;
    UmiAiTrainingExperimentResult report = {0};
    UmiAiTrainingExperimentProgress progress = {0};
    size_t inputs[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    size_t targets[UMI_AI_DATASET_MAX_CONTEXT * UMI_AI_DATASET_MAX_SEQUENCES];
    size_t *effectiveInputs = NULL;
    size_t *effectiveTargets = NULL;
    UmiStatus status;
    size_t tokens;
    size_t effectiveTokens = 0U;
    uint64_t startSteps = 0U, verifiedSteps = 0U;
    UmiAiTrainingDecoderConfig verifiedConfig;
    if (dataset == NULL || outModel == NULL || outResult == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = accumulation == NULL ?
        UmiAiTrainingExperimentValidate(config) :
        UmiAiTrainingExperimentAccumulationValidate(config, accumulation);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingDatasetInspect(dataset, &datasetSummary);
    if (status != UMI_STATUS_OK) return status;
    if (datasetSummary.trainingDocuments == 0U ||
        datasetSummary.evaluationDocuments == 0U)
        return UMI_STATUS_INVALID_STATE;
    tokens = TokenCount(config);
    if (checkpointModel == NULL) {
        status = UmiAiTrainingDecoderCreate(&config->decoder, &created);
    } else {
        status = UmiAiTrainingDecoderInspect(checkpointModel,
                                              &verifiedConfig, &startSteps);
        if (status != UMI_STATUS_OK) return status;
        if (!SameArchitecture(&config->decoder, &verifiedConfig) ||
            startSteps > (uint64_t)config->steps) return UMI_STATUS_INVALID_STATE;
        status = CloneDecoder(checkpointModel, &created);
    }
    if (status != UMI_STATUS_OK) return status;
    if (accumulation != NULL) {
        effectiveTokens = (size_t)accumulation->effectiveSequences *
                          (size_t)config->decoder.contextLength;
        effectiveInputs = malloc(effectiveTokens * sizeof(*effectiveInputs));
        effectiveTargets = malloc(effectiveTokens * sizeof(*effectiveTargets));
        if (effectiveInputs == NULL || effectiveTargets == NULL) {
            status = UMI_STATUS_OUT_OF_MEMORY;
            goto fail;
        }
    }
    progress.completedSteps = startSteps;
    status = UmiAiTrainingExperimentEvaluate(created, dataset, config,
                                              &progress.heldOutCrossEntropy,
                                              &progress.heldOutPerplexity);
    if (status != UMI_STATUS_OK) goto fail;
    report.dataset = datasetSummary;
    report.baselineHeldOutLoss = progress.heldOutCrossEntropy;
    report.bestObservedHeldOutLoss = progress.heldOutCrossEntropy;
    report.bestObservedStep = startSteps;
    report.evaluationCount = 1U;
    if (observer != NULL) {
        status = observer(&progress, userData);
        if (status != UMI_STATUS_OK) goto fail;
    }
    for (uint32_t step = (uint32_t)startSteps + 1U;
         step <= config->steps; ++step) {
        if (accumulation == NULL) {
            /* Preserve the original dataset draw and single-batch training
             * behaviour, including its optimiser and checkpoint semantics. */
            status = UmiAiTrainingDatasetBatch(
                dataset, UMI_AI_TRAINING_PARTITION_TRAIN,
                (size_t)config->decoder.contextLength,
                (size_t)config->batchSequences,
                config->trainSeed, (uint64_t)(step - 1U),
                inputs, targets, tokens);
            if (status != UMI_STATUS_OK) goto fail;
            status = UmiAiTrainingDecoderTrainStep(
                created, inputs, targets, tokens,
                config->learningRate, &progress.lastTrainingLoss);
        } else {
            /* One effective batch, one atomic SGD update. All microbatch
             * forward/backward operations remain Framework decoder-owned. */
            status = DrawEffectiveBatch(dataset, config, accumulation,
                    (uint64_t)(step - 1U), effectiveInputs, effectiveTargets);
            if (status != UMI_STATUS_OK) goto fail;
            status = UmiAiTrainingDecoderTrainAccumulated(
                    created, effectiveInputs, effectiveTargets,
                    effectiveTokens, (size_t)accumulation->microbatchSequences,
                    config->learningRate, &progress.lastTrainingLoss);
        }
        if (status != UMI_STATUS_OK) goto fail;
        if (!isfinite(progress.lastTrainingLoss) ||
            progress.lastTrainingLoss < 0.0F) {
            status = UMI_STATUS_INVALID_STATE;
            goto fail;
        }
        progress.hasTrainingLoss = 1;
        progress.completedSteps = (uint64_t)step;
        if ((step % config->evalInterval == 0U) || step == config->steps) {
            status = UmiAiTrainingExperimentEvaluate(
                created, dataset, config,
                &progress.heldOutCrossEntropy, &progress.heldOutPerplexity);
            if (status != UMI_STATUS_OK) goto fail;
            report.evaluationCount += 1U;
            if (progress.heldOutCrossEntropy < report.bestObservedHeldOutLoss) {
                report.bestObservedHeldOutLoss = progress.heldOutCrossEntropy;
                report.bestObservedStep = (uint64_t)step;
            }
            if (observer != NULL) {
                status = observer(&progress, userData);
                if (status != UMI_STATUS_OK) goto fail;
            }
        }
    }
    status = UmiAiTrainingDecoderInspect(created, &verifiedConfig,
                                          &verifiedSteps);
    if (status != UMI_STATUS_OK) goto fail;
    if (verifiedSteps != (uint64_t)config->steps ||
        !SameArchitecture(&verifiedConfig, &config->decoder)) {
        status = UMI_STATUS_INVALID_STATE;
        goto fail;
    }
    report.completedSteps = verifiedSteps;
    report.finalHeldOutLoss = progress.heldOutCrossEntropy;
    report.lastTrainingLoss = progress.lastTrainingLoss;
    *outModel = created;
    *outResult = report;
    free(effectiveInputs);
    free(effectiveTargets);
    return UMI_STATUS_OK;
fail:
    free(effectiveInputs);
    free(effectiveTargets);
    UmiAiTrainingDecoderDestroy(created);
    return status;
}

UmiStatus UmiAiTrainingExperimentRun(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult)
{
    return RunInternal(dataset, config, NULL, NULL, observer, userData,
                       outModel, outResult);
}

UmiStatus UmiAiTrainingExperimentResume(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingDecoder *checkpointModel,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult)
{
    if (checkpointModel == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, checkpointModel, NULL,
                       observer, userData, outModel, outResult);
}

/* New opt-in entry points retain the original C ABI and execution path. */
UmiStatus UmiAiTrainingExperimentRunAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingExperimentAccumulation *accumulation,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult)
{
    if (accumulation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, NULL, accumulation,
                       observer, userData, outModel, outResult);
}

UmiStatus UmiAiTrainingExperimentResumeAccumulated(
    const UmiAiTrainingDataset *dataset,
    const UmiAiTrainingExperimentConfig *config,
    const UmiAiTrainingExperimentAccumulation *accumulation,
    const UmiAiTrainingDecoder *checkpointModel,
    UmiAiTrainingExperimentObserver observer,
    void *userData,
    UmiAiTrainingDecoder **outModel,
    UmiAiTrainingExperimentResult *outResult)
{
    if (accumulation == NULL || checkpointModel == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    return RunInternal(dataset, config, checkpointModel, accumulation,
                       observer, userData, outModel, outResult);
}
