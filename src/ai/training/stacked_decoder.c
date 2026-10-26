/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_decoder.c
 * PURPOSE:
 *   Compose a separately identified multi-layer decoder from the existing
 *   Framework embedding, Transformer, training tape and selected loss owners.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_decoder.h"
#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/selected_loss.h"
#include "umicom/ai/training/transformer.h"
#include "stacked_decoder_internal.h"
#include "umicom/ai/training/stacked_projection_trace.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static size_t StackedParameterCount(const UmiAiTrainingStackedDecoderConfig *config)
{
    return 3U + (size_t)config->transformerLayers *
        UMI_AI_STACKED_PARAMETERS_PER_LAYER;
}

UmiStatus UmiAiStackedValidateConfig(
    const UmiAiTrainingStackedDecoderConfig *config)
{
    UmiAiTrainingModelScalingShape shape;
    UmiAiTrainingModelScalingEstimate estimate;
    if (config == NULL || config->vocabularySize < 5U ||
        config->vocabularySize > UMI_AI_STACKED_MAX_VOCABULARY ||
        config->contextLength == 0U ||
        config->contextLength > UMI_AI_STACKED_MAX_CONTEXT ||
        config->channels < 2U ||
        config->channels > UMI_AI_STACKED_MAX_CHANNELS ||
        config->hiddenChannels == 0U ||
        config->hiddenChannels > UMI_AI_STACKED_MAX_HIDDEN_CHANNELS ||
        config->attentionHeads == 0U ||
        config->channels % config->attentionHeads != 0U ||
        config->transformerLayers == 0U ||
        config->transformerLayers > UMI_AI_STACKED_MAX_LAYERS ||
        (size_t)config->vocabularySize >
            UMI_AI_TRAINING_MAX_ELEMENTS / (size_t)config->channels ||
        (size_t)config->contextLength >
            UMI_AI_TRAINING_MAX_ELEMENTS / (size_t)config->vocabularySize ||
        (size_t)config->contextLength >
            UMI_AI_TRAINING_MAX_ELEMENTS / (size_t)config->contextLength ||
        (size_t)config->channels >
            UMI_AI_TRAINING_MAX_ELEMENTS / (size_t)config->hiddenChannels ||
        StackedParameterCount(config) > UMI_AI_TRAINING_MAX_TENSORS)
        return UMI_STATUS_INVALID_ARGUMENT;
    shape = (UmiAiTrainingModelScalingShape){
        (uint64_t)config->vocabularySize,
        (uint64_t)config->contextLength,
        (uint64_t)config->channels,
        (uint64_t)config->hiddenChannels,
        (uint64_t)config->attentionHeads,
        (uint64_t)config->transformerLayers, 1U, 0U
    };
    return UmiAiTrainingModelScalingEstimateBuild(&shape, &estimate);
}

UmiStatus UmiAiTrainingStackedDecoderPlan(
    const UmiAiTrainingStackedDecoderConfig *config,
    UmiAiTrainingModelScalingEstimate *outEstimate)
{
    UmiAiTrainingModelScalingShape shape;
    if (outEstimate == NULL ||
        UmiAiStackedValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    shape = (UmiAiTrainingModelScalingShape){
        (uint64_t)config->vocabularySize,
        (uint64_t)config->contextLength,
        (uint64_t)config->channels,
        (uint64_t)config->hiddenChannels,
        (uint64_t)config->attentionHeads,
        (uint64_t)config->transformerLayers, 1U, 0U
    };
    return UmiAiTrainingModelScalingEstimateBuild(&shape, outEstimate);
}

void UmiAiStackedParameterShape(
    const UmiAiTrainingStackedDecoderConfig *config, size_t index,
    size_t *outRows, size_t *outColumns)
{
    size_t rows = (size_t)config->channels;
    size_t columns = (size_t)config->channels;
    if (index == UMI_STACKED_TOKEN_EMBEDDING)
        rows = (size_t)config->vocabularySize;
    else if (index == UMI_STACKED_POSITION_EMBEDDING)
        rows = (size_t)config->contextLength;
    else if (index == StackedParameterCount(config) - 1U)
        columns = (size_t)config->vocabularySize;
    else {
        const size_t role = (index - UMI_STACKED_LAYER_START) %
                            UMI_AI_STACKED_PARAMETERS_PER_LAYER;
        if (role == UMI_STACKED_NORM1_SCALE || role == UMI_STACKED_NORM1_BIAS ||
            role == UMI_STACKED_NORM2_SCALE || role == UMI_STACKED_NORM2_BIAS ||
            role == UMI_STACKED_FEED_FORWARD_DOWN_BIAS)
            rows = 1U;
        else if (role == UMI_STACKED_FEED_FORWARD_DOWN)
            rows = (size_t)config->hiddenChannels;
        if (role == UMI_STACKED_FEED_FORWARD_UP ||
            role == UMI_STACKED_FEED_FORWARD_UP_BIAS)
            columns = (size_t)config->hiddenChannels;
        if (role == UMI_STACKED_FEED_FORWARD_UP_BIAS)
            rows = 1U;
    }
    *outRows = rows;
    *outColumns = columns;
}

void UmiAiTrainingStackedDecoderDestroy(UmiAiTrainingStackedDecoder *model)
{
    if (model == NULL) return;
    UmiAiTrainingTapeDestroy(model->tape);
    free(model);
}

UmiStatus UmiAiStackedCreateFromValues(
    const UmiAiTrainingStackedDecoderConfig *config,
    const float *const *valuePointers, uint64_t completedSteps,
    UmiAiTrainingStackedDecoder **outModel)
{
    UmiAiTrainingStackedDecoder *created = NULL;
    UmiAiTrainingRandomState randomState = {0};
    UmiStatus status;
    if (outModel == NULL || *outModel != NULL ||
        UmiAiStackedValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    created = calloc(1U, sizeof(*created));
    if (created == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    created->config = *config;
    created->parameterCount = StackedParameterCount(config);
    created->completedSteps = completedSteps;
    status = UmiAiTrainingTapeCreate(&created->tape);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingRandomSeed(&randomState, config->initialSeed);
    if (status != UMI_STATUS_OK) goto failure;
    for (size_t index = 0U; index < created->parameterCount; ++index) {
        size_t rows = 0U, columns = 0U;
        float *values;
        size_t count;
        UmiAiStackedParameterShape(config, index, &rows, &columns);
        if (rows == 0U || columns == 0U ||
            rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto failure;
        }
        count = rows * columns;
        values = calloc(count, sizeof(*values));
        if (values == NULL) { status = UMI_STATUS_OUT_OF_MEMORY; goto failure; }
        if (valuePointers != NULL) {
            if (valuePointers[index] == NULL) {
                free(values);
                status = UMI_STATUS_INVALID_ARGUMENT;
                goto failure;
            }
            memcpy(values, valuePointers[index], count * sizeof(*values));
            for (size_t j = 0U; j < count; ++j) {
                if (!isfinite(values[j])) {
                    free(values);
                    status = UMI_STATUS_INVALID_ARGUMENT;
                    goto failure;
                }
            }
        } else {
            int normScale = 0;
            int zeroBias = 0;
            if (index >= UMI_STACKED_LAYER_START &&
                index + 1U < created->parameterCount) {
                size_t role = (index - UMI_STACKED_LAYER_START) %
                              UMI_AI_STACKED_PARAMETERS_PER_LAYER;
                normScale = role == UMI_STACKED_NORM1_SCALE ||
                            role == UMI_STACKED_NORM2_SCALE;
                zeroBias = role == UMI_STACKED_NORM1_BIAS ||
                           role == UMI_STACKED_NORM2_BIAS ||
                           role == UMI_STACKED_FEED_FORWARD_UP_BIAS ||
                           role == UMI_STACKED_FEED_FORWARD_DOWN_BIAS;
            }
            if (normScale) {
                for (size_t j = 0U; j < count; ++j) values[j] = 1.0F;
            } else if (!zeroBias) {
                status = UmiAiTrainingRandomFillUniform(
                    &randomState, -0.15F, 0.15F, values, count);
                if (status != UMI_STATUS_OK) {
                    free(values);
                    goto failure;
                }
            }
        }
        status = UmiAiTrainingTensorCreate(created->tape, rows, columns,
                    values, count, 1, &created->parameters[index]);
        free(values);
        if (status != UMI_STATUS_OK) goto failure;
        if (created->parameterElements > SIZE_MAX - count) {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            goto failure;
        }
        created->parameterElements += count;
    }
    *outModel = created;
    return UMI_STATUS_OK;

failure:
    UmiAiTrainingStackedDecoderDestroy(created);
    return status;
}

UmiStatus UmiAiTrainingStackedDecoderCreate(
    const UmiAiTrainingStackedDecoderConfig *config,
    UmiAiTrainingStackedDecoder **outModel)
{
    return UmiAiStackedCreateFromValues(config, NULL, 0U, outModel);
}

UmiStatus UmiAiTrainingStackedDecoderInspect(
    const UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingStackedDecoderInfo *outInfo)
{
    UmiAiTrainingStackedDecoderInfo candidate = {0};
    if (model == NULL || model->tape == NULL || outInfo == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    candidate.config = model->config;
    candidate.completedTrainingSteps = model->completedSteps;
    candidate.trainableParameterTensors = model->parameterCount;
    candidate.trainableParameterElements = model->parameterElements;
    *outInfo = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedDecoderMemoryInspect(
    const UmiAiTrainingStackedDecoder *model,
    UmiAiTrainingTapeMemory *outMemory)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingTapeMemoryInspect(model->tape, outMemory);
}

static UmiStatus StackedValidateInputs(
    const UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, const size_t *targets, const uint8_t *mask,
    size_t count, int selected)
{
    size_t chosen = 0U;
    if (model == NULL || model->tape == NULL || inputs == NULL ||
        count != (size_t)model->config.contextLength ||
        (selected && (targets == NULL || mask == NULL)))
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < count; ++i) {
        if (inputs[i] >= (size_t)model->config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (selected) {
            if (mask[i] > 1U ||
                (mask[i] == 1U &&
                 targets[i] >= (size_t)model->config.vocabularySize))
                return UMI_STATUS_INVALID_ARGUMENT;
            chosen += (size_t)(mask[i] == 1U);
        }
    }
    if (selected && chosen == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* The projection observer sees EXACTLY the tensors used by the canonical
 * Transformer block. Capturing rows is deliberately not a parallel kernel. */
typedef struct StackedProjectionObserver {
    size_t currentLayer;
    size_t rows;
    size_t channels;
    float *keys;
    float *values;
    size_t capturedRoles;
} StackedProjectionObserver;

static UmiStatus StackedObserveProjection(
    void *context, UmiAiTrainingTape *tape,
    UmiAiTrainingTransformerProjection projection,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTensor *weights,
    UmiAiTrainingTensor **outTensor)
{
    StackedProjectionObserver *observer = context;
    UmiAiTrainingTensor *result = NULL;
    UmiStatus status = UmiAiTrainingMatmul(tape, input, weights, &result);
    if (status != UMI_STATUS_OK) return status;
    if (projection == UMI_AI_TRANSFORMER_PROJECTION_KEY ||
        projection == UMI_AI_TRANSFORMER_PROJECTION_VALUE) {
        size_t rows = 0U, channels = 0U;
        status = UmiAiTrainingTensorShape(result, &rows, &channels);
        if (status != UMI_STATUS_OK || rows != observer->rows ||
            channels != observer->channels) return UMI_STATUS_INVALID_STATE;
        const size_t offset = observer->currentLayer * rows * channels;
        float *destination = projection == UMI_AI_TRANSFORMER_PROJECTION_KEY
            ? observer->keys + offset : observer->values + offset;
        status = UmiAiTrainingTensorValues(result, destination, rows * channels);
        if (status != UMI_STATUS_OK) return status;
        observer->capturedRoles |= projection == UMI_AI_TRANSFORMER_PROJECTION_KEY
            ? 1U : 2U;
    }
    *outTensor = result;
    return UMI_STATUS_OK;
}

static UmiStatus StackedForwardWithObservation(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, size_t count,
    StackedProjectionObserver *observer,
    UmiAiTrainingTensor **outLogits)
{
    size_t positions[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    UmiAiTrainingTensor *token = NULL, *position = NULL, *state = NULL;
    UmiAiTrainingTensor *sum = NULL, *next = NULL, *logits = NULL;
    UmiStatus status;
    UmiAiTrainingTransformerBlockConfig blockConfig = {
        (size_t)model->config.contextLength,
        (size_t)model->config.attentionHeads, 1e-4F
    };
    for (size_t i = 0U; i < count; ++i) positions[i] = i;
#define STACKED_CALL(expression) do { \
    status = (expression); if (status != UMI_STATUS_OK) return status; \
} while (0)
    STACKED_CALL(UmiAiTrainingEmbeddingLookup(model->tape,
        model->parameters[UMI_STACKED_TOKEN_EMBEDDING], inputs, count, &token));
    STACKED_CALL(UmiAiTrainingEmbeddingLookup(model->tape,
        model->parameters[UMI_STACKED_POSITION_EMBEDDING], positions, count, &position));
    STACKED_CALL(UmiAiTrainingAdd(model->tape, token, position, &sum));
    state = sum;
    for (size_t layer = 0U; layer < (size_t)model->config.transformerLayers;
         ++layer) {
        const size_t first = UMI_STACKED_LAYER_START +
                             layer * UMI_AI_STACKED_PARAMETERS_PER_LAYER;
        const UmiAiTrainingTransformerBlockParameters parameters = {
            model->parameters[first + UMI_STACKED_NORM1_SCALE],
            model->parameters[first + UMI_STACKED_NORM1_BIAS],
            model->parameters[first + UMI_STACKED_QUERY],
            model->parameters[first + UMI_STACKED_KEY],
            model->parameters[first + UMI_STACKED_VALUE],
            model->parameters[first + UMI_STACKED_ATTENTION_OUTPUT],
            model->parameters[first + UMI_STACKED_NORM2_SCALE],
            model->parameters[first + UMI_STACKED_NORM2_BIAS],
            model->parameters[first + UMI_STACKED_FEED_FORWARD_UP],
            model->parameters[first + UMI_STACKED_FEED_FORWARD_UP_BIAS],
            model->parameters[first + UMI_STACKED_FEED_FORWARD_DOWN],
            model->parameters[first + UMI_STACKED_FEED_FORWARD_DOWN_BIAS]
        };
        if (observer != NULL) {
            observer->currentLayer = layer;
            observer->capturedRoles = 0U;
            STACKED_CALL(UmiAiTrainingTransformerBlockForwardWithProjections(
                model->tape, state, &parameters, &blockConfig,
                StackedObserveProjection, observer, &next));
            if (observer->capturedRoles != 3U)
                return UMI_STATUS_INVALID_STATE;
        } else {
            /* The historical forward retains its exact original entry point
             * and operation order when observation is not requested. */
            STACKED_CALL(UmiAiTrainingTransformerBlockForward(
                model->tape, state, &parameters, &blockConfig, &next));
        }
        state = next;
    }
    STACKED_CALL(UmiAiTrainingMatmul(model->tape, state,
        model->parameters[model->parameterCount - 1U], &logits));
#undef STACKED_CALL
    *outLogits = logits;
    return UMI_STATUS_OK;
}

/* Preserve the existing internal canonical forward for training, evaluation
 * and all pre-existing inference APIs. No hooks are installed in that path. */
static UmiStatus StackedForward(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, size_t count, UmiAiTrainingTensor **outLogits)
{
    return StackedForwardWithObservation(model, inputs, count, NULL, outLogits);
}

/* Capture copies all outputs privately before publishing. It intentionally
 * runs the same model with the same positions and heads as DecoderLogits. */
static int StackedTraceRangesOverlap(
    const void *a, size_t aBytes, const void *b, size_t bBytes)
{
    const uintptr_t x = (uintptr_t)a;
    const uintptr_t y = (uintptr_t)b;
    if (x > UINTPTR_MAX - aBytes || y > UINTPTR_MAX - bBytes) return 1;
    return x < y + bBytes && y < x + aBytes;
}

UmiStatus UmiAiTrainingStackedDecoderTraceProjections(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputIds, size_t tokenCount,
    float *outKeys, size_t keyCapacity,
    float *outValues, size_t valueCapacity,
    float *outLogits, size_t logitCapacity)
{
    UmiAiTrainingTensor *logits = NULL;
    StackedProjectionObserver observer = {0};
    UmiStatus status, clean;
    float *candidate = NULL;
    size_t keyCount, logitCount;
    status = StackedValidateInputs(model, inputIds, NULL, NULL, tokenCount, 0);
    if (status != UMI_STATUS_OK) return status;
    if (outKeys == NULL || outValues == NULL || outLogits == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    keyCount = (size_t)model->config.transformerLayers * tokenCount *
               (size_t)model->config.channels;
    logitCount = tokenCount * (size_t)model->config.vocabularySize;
    if (keyCapacity < keyCount || valueCapacity < keyCount ||
        logitCapacity < logitCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (StackedTraceRangesOverlap(outKeys, keyCount * sizeof(float),
                                  outValues, keyCount * sizeof(float)) ||
        StackedTraceRangesOverlap(outKeys, keyCount * sizeof(float),
                                  outLogits, logitCount * sizeof(float)) ||
        StackedTraceRangesOverlap(outValues, keyCount * sizeof(float),
                                  outLogits, logitCount * sizeof(float)) ||
        StackedTraceRangesOverlap(inputIds, tokenCount * sizeof(size_t),
                                  outKeys, keyCount * sizeof(float)) ||
        StackedTraceRangesOverlap(inputIds, tokenCount * sizeof(size_t),
                                  outValues, keyCount * sizeof(float)) ||
        StackedTraceRangesOverlap(inputIds, tokenCount * sizeof(size_t),
                                  outLogits, logitCount * sizeof(float)))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* The model's established bounds make this addition safe. A diagnostic
     * check also protects future extensions from inadvertent arithmetic. */
    if (keyCount > (SIZE_MAX / sizeof(float) - logitCount) / 2U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate = malloc((keyCount * 2U + logitCount) * sizeof(float));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    observer.rows = tokenCount;
    observer.channels = (size_t)model->config.channels;
    observer.keys = candidate;
    observer.values = candidate + keyCount;
    status = StackedForwardWithObservation(model, inputIds, tokenCount,
                                            &observer, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(logits,
                  candidate + keyCount * 2U, logitCount);
    if (status == UMI_STATUS_OK) {
        for (size_t index = 0U; index < keyCount * 2U + logitCount; ++index) {
            if (!isfinite(candidate[index])) {
                status = UMI_STATUS_INVALID_STATE;
                break;
            }
        }
    }
    clean = UmiAiTrainingTapeRollback(model->tape, model->parameterCount);
    if (clean != UMI_STATUS_OK) status = clean;
    if (status == UMI_STATUS_OK) {
        memcpy(outKeys, candidate, keyCount * sizeof(float));
        memcpy(outValues, candidate + keyCount, keyCount * sizeof(float));
        memcpy(outLogits, candidate + keyCount * 2U,
               logitCount * sizeof(float));
    }
    free(candidate);
    return status;
}

UmiStatus UmiAiTrainingStackedDecoderLogits(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, size_t tokenCount,
    float *outLogits, size_t capacity)
{
    UmiAiTrainingTensor *logits = NULL;
    float *staged;
    size_t needed;
    UmiStatus status, clean;
    status = StackedValidateInputs(model, inputs, NULL, NULL, tokenCount, 0);
    if (status != UMI_STATUS_OK) return status;
    if (outLogits == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    needed = tokenCount * (size_t)model->config.vocabularySize;
    if (capacity < needed) return UMI_STATUS_CAPACITY_EXCEEDED;
    staged = malloc(needed * sizeof(*staged));
    if (staged == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = StackedForward(model, inputs, tokenCount, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(logits, staged, needed);
    clean = UmiAiTrainingTapeRollback(model->tape, model->parameterCount);
    if (clean != UMI_STATUS_OK) status = clean;
    if (status == UMI_STATUS_OK)
        memcpy(outLogits, staged, needed * sizeof(*staged));
    free(staged);
    return status;
}

static UmiStatus StackedLossOperation(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, const size_t *targets, const uint8_t *mask,
    size_t count, int training, float learningRate, float *outLoss)
{
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    float candidate = 0.0F;
    UmiStatus status, clean;
    status = StackedValidateInputs(model, inputs, targets, mask, count, 1);
    if (status != UMI_STATUS_OK) return status;
    if (outLoss == NULL || (training &&
        (!isfinite(learningRate) || learningRate <= 0.0F)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (training && model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = StackedForward(model, inputs, count, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingSelectedCrossEntropy(
            model->tape, logits, targets, count, mask, count, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &candidate, 1U);
    if (status == UMI_STATUS_OK &&
        (!isfinite(candidate) || candidate < 0.0F))
        status = UMI_STATUS_INVALID_STATE;
    if (training && status == UMI_STATUS_OK)
        status = UmiAiTrainingBackward(model->tape, loss);
    if (training && status == UMI_STATUS_OK)
        status = UmiAiTrainingSgdStepMany(model->tape,
            model->parameters, model->parameterCount, learningRate);
    clean = UmiAiTrainingTapeRollback(model->tape, model->parameterCount);
    if (clean != UMI_STATUS_OK) return clean;
    if (status != UMI_STATUS_OK) return status;
    if (training) ++model->completedSteps;
    *outLoss = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedDecoderEvaluateSelected(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, const size_t *targets, const uint8_t *mask,
    size_t count, float *outLoss)
{
    return StackedLossOperation(model, inputs, targets, mask, count,
                                0, 0.0F, outLoss);
}

UmiStatus UmiAiTrainingStackedDecoderTrainSelected(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, const size_t *targets, const uint8_t *mask,
    size_t count, float rate, UmiAiTrainingStackedStep *outStep)
{
    UmiAiTrainingStackedStep candidate = {0};
    UmiStatus status;
    if (outStep == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = StackedLossOperation(model, inputs, targets, mask, count,
                                  1, rate, &candidate.selectedCrossEntropyBeforeUpdate);
    if (status != UMI_STATUS_OK) return status;
    candidate.completedTrainingSteps = model->completedSteps;
    for (size_t i = 0U; i < count; ++i)
        candidate.supervisedTokens += (size_t)(mask[i] == 1U);
    *outStep = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedDecoderPredictNext(
    UmiAiTrainingStackedDecoder *model,
    const size_t *inputs, size_t count, size_t *outTokenId)
{
    float *logits;
    size_t valuesCount, offset, vocabulary, best = 0U;
    UmiStatus status;
    if (outTokenId == NULL || model == NULL ||
        count != (size_t)model->config.contextLength)
        return UMI_STATUS_INVALID_ARGUMENT;
    vocabulary = (size_t)model->config.vocabularySize;
    valuesCount = count * vocabulary;
    logits = malloc(valuesCount * sizeof(*logits));
    if (logits == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingStackedDecoderLogits(
        model, inputs, count, logits, valuesCount);
    if (status != UMI_STATUS_OK) { free(logits); return status; }
    offset = (count - 1U) * vocabulary;
    for (size_t id = 0U; id < vocabulary; ++id) {
        if (!isfinite(logits[offset + id])) {
            free(logits);
            return UMI_STATUS_INVALID_STATE;
        }
        if (logits[offset + id] > logits[offset + best]) best = id;
    }
    *outTokenId = best;
    free(logits);
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedDecoderGenerateGreedy(
    UmiAiTrainingStackedDecoder *model,
    const size_t *fullContext, size_t contextLength,
    size_t requestedTokens, size_t *outTokens, size_t capacity,
    size_t *outCount)
{
    size_t window[UMI_AI_STACKED_MAX_CONTEXT] = {0};
    size_t staged[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    UmiStatus status;
    if (model == NULL || fullContext == NULL || outTokens == NULL ||
        outCount == NULL || contextLength == 0U ||
        contextLength != (size_t)model->config.contextLength ||
        requestedTokens == 0U ||
        requestedTokens > UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < requestedTokens) return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < contextLength; ++i) {
        if (fullContext[i] >= (size_t)model->config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
        window[i] = fullContext[i];
    }
    for (size_t i = 0U; i < requestedTokens; ++i) {
        status = UmiAiTrainingStackedDecoderPredictNext(
            model, window, contextLength, &staged[i]);
        if (status != UMI_STATUS_OK) return status;
        if (i + 1U < requestedTokens) {
            memmove(window, window + 1U,
                    (contextLength - 1U) * sizeof(*window));
            window[contextLength - 1U] = staged[i];
        }
    }
    memcpy(outTokens, staged, requestedTokens * sizeof(*staged));
    *outCount = requestedTokens;
    return UMI_STATUS_OK;
}
