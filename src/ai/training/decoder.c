/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/decoder.c
 *
 * PURPOSE:
 *   Build, train and evaluate one native, bounded decoder-only model.
 *
 * ARCHITECTURE:
 *   The canonical Framework tape owns the tensors, gradients and optimiser.
 *   The existing Transformer Block owns attention/MLP mathematics. This module
 *   only composes token plus position lookup, the block and output logits.
 *   All callbacks are synchronous; models must not be shared concurrently.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/transformer.h"
#include "decoder_private.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

UmiStatus UmiAiDecoderValidateConfig(const UmiAiTrainingDecoderConfig *config)
{
    if (config == NULL || config->vocabularySize < 2U ||
        config->vocabularySize > UMI_AI_DECODER_MAX_VOCABULARY ||
        config->contextLength == 0U ||
        config->contextLength > UMI_AI_DECODER_MAX_CONTEXT ||
        config->channels < 2U ||
        config->channels > UMI_AI_DECODER_MAX_CHANNELS ||
        config->hiddenChannels == 0U ||
        config->hiddenChannels > UMI_AI_DECODER_MAX_HIDDEN_CHANNELS ||
        config->attentionHeads == 0U ||
        config->channels % config->attentionHeads != 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

void UmiAiDecoderParameterShape(const UmiAiTrainingDecoderConfig *config,
                               size_t parameterId, size_t *rows,
                               size_t *columns)
{
    size_t r = (size_t)config->channels;
    size_t c = (size_t)config->channels;
    switch (parameterId) {
    case UMI_DECODER_TOKEN_EMBEDDING:
        r = (size_t)config->vocabularySize; break;
    case UMI_DECODER_POSITION_EMBEDDING:
        r = (size_t)config->contextLength; break;
    case UMI_DECODER_NORM1_SCALE:
    case UMI_DECODER_NORM1_BIAS:
    case UMI_DECODER_NORM2_SCALE:
    case UMI_DECODER_NORM2_BIAS:
    case UMI_DECODER_DOWN_BIAS:
        r = 1U; break;
    case UMI_DECODER_UP:
        c = (size_t)config->hiddenChannels; break;
    case UMI_DECODER_UP_BIAS:
        r = 1U; c = (size_t)config->hiddenChannels; break;
    case UMI_DECODER_DOWN:
        r = (size_t)config->hiddenChannels; break;
    case UMI_DECODER_LOGITS:
        c = (size_t)config->vocabularySize; break;
    default: break;
    }
    *rows = r;
    *columns = c;
}

UmiStatus UmiAiDecoderCreateFromValues(const UmiAiTrainingDecoderConfig *config,
                                      const float *const *values,
                                      uint64_t completedSteps,
                                      UmiAiTrainingDecoder **outModel)
{
    UmiAiTrainingDecoder *newModel;
    UmiAiTrainingRandomState rng;
    UmiStatus status;
    if (outModel == NULL || UmiAiDecoderValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    newModel = calloc(1U, sizeof(*newModel));
    if (newModel == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    newModel->config = *config;
    status = UmiAiTrainingTapeCreate(&newModel->tape);
    if (status != UMI_STATUS_OK) { free(newModel); return status; }
    status = UmiAiTrainingRandomSeed(&rng, config->initialSeed);
    if (status != UMI_STATUS_OK) goto failure;
    for (size_t i = 0U; i < UMI_DECODER_PARAMETER_COUNT; ++i) {
        size_t rows = 0U, cols = 0U;
        float buffer[UMI_AI_TRAINING_MAX_ELEMENTS] = {0};
        UmiAiDecoderParameterShape(config, i, &rows, &cols);
        size_t count = rows * cols;
        if (count > UMI_AI_TRAINING_MAX_ELEMENTS) {
            status = UMI_STATUS_CAPACITY_EXCEEDED; goto failure;
        }
        if (values != NULL) {
            if (values[i] == NULL) { status = UMI_STATUS_INVALID_ARGUMENT; goto failure; }
            memcpy(buffer, values[i], count * sizeof(*buffer));
        } else if (i == UMI_DECODER_NORM1_SCALE || i == UMI_DECODER_NORM2_SCALE) {
            for (size_t j = 0U; j < count; ++j) buffer[j] = 1.0F;
        } else if (i != UMI_DECODER_NORM1_BIAS && i != UMI_DECODER_NORM2_BIAS &&
                   i != UMI_DECODER_UP_BIAS && i != UMI_DECODER_DOWN_BIAS) {
            status = UmiAiTrainingRandomFillUniform(&rng, -0.25F, 0.25F,
                                                    buffer, count);
            if (status != UMI_STATUS_OK) goto failure;
        }
        status = UmiAiTrainingTensorCreate(newModel->tape, rows, cols,
                                          buffer, count, 1,
                                          &newModel->parameters[i]);
        if (status != UMI_STATUS_OK) goto failure;
    }
    newModel->completedSteps = completedSteps;
    *outModel = newModel;
    return UMI_STATUS_OK;
failure:
    UmiAiTrainingTapeDestroy(newModel->tape);
    free(newModel);
    return status;
}

UmiStatus UmiAiTrainingDecoderCreate(const UmiAiTrainingDecoderConfig *config,
                                    UmiAiTrainingDecoder **outModel)
{
    return UmiAiDecoderCreateFromValues(config, NULL, 0U, outModel);
}

void UmiAiTrainingDecoderDestroy(UmiAiTrainingDecoder *model)
{
    if (model == NULL) return;
    UmiAiTrainingTapeDestroy(model->tape);
    free(model);
}

UmiStatus UmiAiTrainingDecoderInspect(const UmiAiTrainingDecoder *model,
                                     UmiAiTrainingDecoderConfig *outConfig,
                                     uint64_t *outSteps)
{
    if (model == NULL || outConfig == NULL || outSteps == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outConfig = model->config;
    *outSteps = model->completedSteps;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderMemoryInspect(
    const UmiAiTrainingDecoder *model,
    UmiAiTrainingTapeMemory *outMemory)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingTapeMemoryInspect(model->tape, outMemory);
}

UmiStatus UmiAiTrainingDecoderSetMemoryBudget(
    UmiAiTrainingDecoder *model, size_t budgetBytes)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingTapeSetMemoryBudget(model->tape, budgetBytes);
}

static UmiStatus ValidateInputs(const UmiAiTrainingDecoder *model,
                                const size_t *tokens, const size_t *targets,
                                size_t tokenCount)
{
    size_t context;
    size_t widest;
    if (model == NULL || tokens == NULL || tokenCount == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    context = (size_t)model->config.contextLength;
    if (tokenCount % context != 0U) return UMI_STATUS_INVALID_ARGUMENT;
    widest = (size_t)model->config.channels;
    if ((size_t)model->config.vocabularySize > widest)
        widest = (size_t)model->config.vocabularySize;
    if ((size_t)model->config.hiddenChannels > widest)
        widest = (size_t)model->config.hiddenChannels;
    if (tokenCount > UMI_AI_TRAINING_MAX_ELEMENTS / widest ||
        (size_t)model->config.attentionHeads >
            UMI_AI_TRAINING_MAX_ELEMENTS / (tokenCount * context)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    for (size_t i = 0U; i < tokenCount; ++i) {
        if (tokens[i] >= (size_t)model->config.vocabularySize ||
            (targets != NULL && targets[i] >= (size_t)model->config.vocabularySize))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Caller must rewind even on failed construction: the checkpoint ensures
 * that the original parameter prefix survives a transient tensor failure. */
static UmiStatus ForwardLogits(UmiAiTrainingDecoder *model,
                               const size_t *tokens, size_t tokenCount,
                               UmiAiTrainingTensor **outLogits)
{
    UmiAiTrainingTensor *embedded = NULL, *positioned = NULL, *summed = NULL;
    UmiAiTrainingTensor *state = NULL, *logits = NULL;
    UmiAiTrainingTransformerBlockParameters block;
    UmiAiTrainingTransformerBlockConfig blockConfig;
    size_t positionIds[UMI_AI_TRAINING_MAX_ELEMENTS];
    UmiStatus status;
    status = ValidateInputs(model, tokens, NULL, tokenCount);
    if (status != UMI_STATUS_OK || outLogits == NULL) return
        status == UMI_STATUS_OK ? UMI_STATUS_INVALID_ARGUMENT : status;
    for (size_t i = 0U; i < tokenCount; ++i)
        positionIds[i] = i % (size_t)model->config.contextLength;

    block = (UmiAiTrainingTransformerBlockParameters){
        model->parameters[UMI_DECODER_NORM1_SCALE],
        model->parameters[UMI_DECODER_NORM1_BIAS],
        model->parameters[UMI_DECODER_QUERY],
        model->parameters[UMI_DECODER_KEY],
        model->parameters[UMI_DECODER_VALUE],
        model->parameters[UMI_DECODER_PROJECTION],
        model->parameters[UMI_DECODER_NORM2_SCALE],
        model->parameters[UMI_DECODER_NORM2_BIAS],
        model->parameters[UMI_DECODER_UP],
        model->parameters[UMI_DECODER_UP_BIAS],
        model->parameters[UMI_DECODER_DOWN],
        model->parameters[UMI_DECODER_DOWN_BIAS]
    };
    blockConfig = (UmiAiTrainingTransformerBlockConfig){
        (size_t)model->config.contextLength,
        (size_t)model->config.attentionHeads,
        1e-4F
    };
#define UMI_DECODER_TRY(expr) do { \
    status = (expr); if (status != UMI_STATUS_OK) return status; \
} while (0)
    UMI_DECODER_TRY(UmiAiTrainingEmbeddingLookup(model->tape,
        model->parameters[UMI_DECODER_TOKEN_EMBEDDING],
        tokens, tokenCount, &embedded));
    UMI_DECODER_TRY(UmiAiTrainingEmbeddingLookup(model->tape,
        model->parameters[UMI_DECODER_POSITION_EMBEDDING],
        positionIds, tokenCount, &positioned));
    UMI_DECODER_TRY(UmiAiTrainingAdd(model->tape, embedded, positioned, &summed));
    UMI_DECODER_TRY(UmiAiTrainingTransformerBlockForward(model->tape, summed,
        &block, &blockConfig, &state));
    UMI_DECODER_TRY(UmiAiTrainingMatmul(model->tape, state,
        model->parameters[UMI_DECODER_LOGITS], &logits));
#undef UMI_DECODER_TRY
    *outLogits = logits;
    return UMI_STATUS_OK;
}

/* Drop ALL temporary nodes after each request (including a failed forward).
 * The parameter prefix survives; no old graph or gradients are reused. */
static UmiStatus RewindParameters(UmiAiTrainingDecoder *model)
{
    return UmiAiTrainingTapeRewind(model->tape, UMI_DECODER_PARAMETER_COUNT);
}

UmiStatus UmiAiTrainingDecoderLogits(UmiAiTrainingDecoder *model,
                                    const size_t *inputIds,
                                    size_t tokenCount,
                                    float *outLogits,
                                    size_t capacity)
{
    UmiAiTrainingTensor *logits = NULL;
    UmiStatus status = ValidateInputs(model, inputIds, NULL, tokenCount);
    size_t needed;
    float values[UMI_AI_TRAINING_MAX_ELEMENTS];
    if (status != UMI_STATUS_OK) return status;
    if (outLogits == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    needed = tokenCount * (size_t)model->config.vocabularySize;
    if (capacity < needed) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ForwardLogits(model, inputIds, tokenCount, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(logits, values, needed);
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    memcpy(outLogits, values, needed * sizeof(*values));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderEvaluate(UmiAiTrainingDecoder *model,
                                      const size_t *inputIds,
                                      const size_t *targetIds,
                                      size_t tokenCount,
                                      float *outLoss)
{
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    UmiStatus status;
    float value = 0.0F;
    if (outLoss == NULL || targetIds == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ValidateInputs(model, inputIds, targetIds, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    status = ForwardLogits(model, inputIds, tokenCount, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingCrossEntropy(model->tape, logits, targetIds,
                                          tokenCount, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &value, 1U);
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    *outLoss = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderTrainStep(UmiAiTrainingDecoder *model,
                                       const size_t *inputIds,
                                       const size_t *targetIds,
                                       size_t tokenCount,
                                       float learningRate,
                                       float *outLoss)
{
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    UmiStatus status;
    float value = 0.0F;
    if (outLoss == NULL || targetIds == NULL || !isfinite(learningRate) ||
        learningRate <= 0.0F) return UMI_STATUS_INVALID_ARGUMENT;
    status = ValidateInputs(model, inputIds, targetIds, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    if (model->completedSteps == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ForwardLogits(model, inputIds, tokenCount, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingCrossEntropy(model->tape, logits, targetIds,
                                          tokenCount, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &value, 1U);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingBackward(model->tape, loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingSgdStepMany(model->tape, model->parameters,
                                         UMI_DECODER_PARAMETER_COUNT, learningRate);
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    model->completedSteps++;
    *outLoss = value;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderPredictNext(UmiAiTrainingDecoder *model,
                                         const size_t *inputIds,
                                         size_t tokenCount,
                                         size_t *outTokenId)
{
    float values[UMI_AI_TRAINING_MAX_ELEMENTS];
    UmiStatus status;
    size_t best = 0U, last;
    if (model == NULL || outTokenId == NULL ||
        tokenCount != (size_t)model->config.contextLength)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderLogits(model, inputIds, tokenCount, values,
                                       UMI_AI_TRAINING_MAX_ELEMENTS);
    if (status != UMI_STATUS_OK) return status;
    last = (tokenCount - 1U) * (size_t)model->config.vocabularySize;
    for (size_t i = 1U; i < (size_t)model->config.vocabularySize; ++i) {
        if (values[last + i] > values[last + best]) best = i;
    }
    *outTokenId = best;
    return UMI_STATUS_OK;
}

/*-----------------------------------------------------------------------------
 * Memory-bounded effective-batch training.
 *
 * Reuse ForwardLogits, the canonical cross-entropy/backward operators and the
 * established training tape. Each independent microbatch graph is discarded
 * immediately after copying gradients into temporary double-precision sums.
 * A single tape-owned, transactionally validated SGD update follows. Existing
 * one-batch TrainStep, checkpoint encoding, and prediction stay unchanged.
 *---------------------------------------------------------------------------*/
UmiStatus UmiAiTrainingDecoderTrainAccumulated(
    UmiAiTrainingDecoder *model,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    size_t microbatchSequences,
    float learningRate,
    float *outLoss)
{
    size_t counts[UMI_DECODER_PARAMETER_COUNT] = {0};
    size_t offsets[UMI_DECODER_PARAMETER_COUNT] = {0};
    const float *gradientPointers[UMI_DECODER_PARAMETER_COUNT] = {0};
    double *accumulated = NULL;
    float *reduced = NULL;
    float *scratch = NULL;
    size_t totalElements = 0U;
    size_t context;
    size_t microbatchTokens;
    double weightedLoss = 0.0;
    UmiStatus status = UMI_STATUS_OK;

    if (model == NULL || inputIds == NULL || targetIds == NULL ||
        outLoss == NULL || tokenCount == 0U ||
        tokenCount > UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS ||
        microbatchSequences == 0U || !isfinite(learningRate) ||
        learningRate <= 0.0F) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    context = (size_t)model->config.contextLength;
    if (context == 0U || tokenCount % context != 0U ||
        microbatchSequences > UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS / context)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    microbatchTokens = microbatchSequences * context;

    /* Reject invalid IDs and impossible microbatch shapes before doing any
     * mathematical work. The total effective batch itself may exceed one
     * tape tensor's capacity; each graph must still fit its normal limits. */
    for (size_t i = 0U; i < tokenCount; ++i) {
        if (inputIds[i] >= (size_t)model->config.vocabularySize ||
            targetIds[i] >= (size_t)model->config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t firstCount = tokenCount < microbatchTokens ? tokenCount : microbatchTokens;
    status = ValidateInputs(model, inputIds, targetIds, firstCount);
    if (status != UMI_STATUS_OK) return status;

    /* Query the existing parameter schema rather than introducing a second
     * list of tensor widths. Gradients are packed into two owned allocations
     * and the original model parameter buffers are never exposed. */
    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p) {
        size_t rows = 0U, columns = 0U, count;
        UmiAiDecoderParameterShape(&model->config, p, &rows, &columns);
        if (columns == 0U || rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto finish;
        }
        count = rows * columns;
        if (count > SIZE_MAX - totalElements) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto finish;
        }
        offsets[p] = totalElements;
        counts[p] = count;
        totalElements += count;
    }
    if (totalElements > SIZE_MAX / sizeof(double) ||
        totalElements > SIZE_MAX / sizeof(float)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto finish;
    }
    accumulated = calloc(totalElements, sizeof(*accumulated));
    reduced = malloc(totalElements * sizeof(*reduced));
    scratch = malloc(UMI_AI_TRAINING_MAX_ELEMENTS * sizeof(*scratch));
    if (accumulated == NULL || reduced == NULL || scratch == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto finish;
    }

    for (size_t start = 0U; start < tokenCount; ) {
        UmiAiTrainingTensor *logits = NULL;
        UmiAiTrainingTensor *loss = NULL;
        size_t remaining = tokenCount - start;
        size_t currentTokens = remaining < microbatchTokens ?
                               remaining : microbatchTokens;
        float currentLoss = 0.0F;
        UmiStatus rewindStatus;

        status = ForwardLogits(model, &inputIds[start], currentTokens, &logits);
        if (status == UMI_STATUS_OK)
            status = UmiAiTrainingCrossEntropy(model->tape, logits,
                       &targetIds[start], currentTokens, &loss);
        if (status == UMI_STATUS_OK)
            status = UmiAiTrainingTensorValues(loss, &currentLoss, 1U);
        if (status == UMI_STATUS_OK)
            status = UmiAiTrainingBackward(model->tape, loss);
        if (status == UMI_STATUS_OK) {
            weightedLoss += (double)currentLoss * (double)currentTokens;
            if (!isfinite(weightedLoss)) status = UMI_STATUS_INVALID_STATE;
        }
        for (size_t p = 0U;
             status == UMI_STATUS_OK && p < UMI_DECODER_PARAMETER_COUNT; ++p) {
            status = UmiAiTrainingTensorGradients(model->parameters[p], scratch,
                                                   counts[p]);
            for (size_t i = 0U; status == UMI_STATUS_OK && i < counts[p]; ++i) {
                double weighted = accumulated[offsets[p] + i] +
                                  (double)scratch[i] * (double)currentTokens;
                if (!isfinite(weighted)) status = UMI_STATUS_INVALID_STATE;
                else accumulated[offsets[p] + i] = weighted;
            }
        }
        /* The parameter leaves survive, while the graph and all its large
         * intermediate buffers are released on success AND on failure. */
        rewindStatus = RewindParameters(model);
        if (status == UMI_STATUS_OK) status = rewindStatus;
        if (status != UMI_STATUS_OK) goto finish;
        start += currentTokens;
    }

    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p) {
        for (size_t i = 0U; i < counts[p]; ++i) {
            double mean = accumulated[offsets[p] + i] / (double)tokenCount;
            if (!isfinite(mean) || mean > (double)FLT_MAX ||
                mean < -(double)FLT_MAX) {
                status = UMI_STATUS_INVALID_STATE;
                goto finish;
            }
            reduced[offsets[p] + i] = (float)mean;
        }
        gradientPointers[p] = &reduced[offsets[p]];
    }
    /* Exactly one optimiser step, with all weights validated before any
     * mutation. This is the canonical tape's provided-gradient SGD API. */
    status = UmiAiTrainingSgdStepWithGradients(model->tape, model->parameters,
             gradientPointers, UMI_DECODER_PARAMETER_COUNT, learningRate);
    if (status != UMI_STATUS_OK) goto finish;
    ++model->completedSteps;
    *outLoss = (float)(weightedLoss / (double)tokenCount);

finish:
    free(scratch);
    free(reduced);
    free(accumulated);
    return status;
}
