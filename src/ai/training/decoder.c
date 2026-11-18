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
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder.h"
#include "umicom/ai/training/gradient_accumulation.h"
#include "umicom/ai/training/optimizer.h"
#include "optimizer_internal.h"
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
/* The selection-aware composition below reuses the same embeddings, positions, transformer and logits projection. Retain the original graph builder for review; the compatibility wrapper preserves its unmasked behavior. The former implementation is retained for owner review. */
#if 0
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
#endif
static UmiStatus ForwardLogitsSelected(UmiAiTrainingDecoder *model,
                               const size_t *tokens, size_t tokenCount,
                               const unsigned char *attention, size_t attentionCount,
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
    UMI_DECODER_TRY(attention != NULL
        ? UmiAiTrainingTransformerBlockForwardMasked(model->tape, summed,
            &block, &blockConfig, attention, attentionCount, &state)
        : UmiAiTrainingTransformerBlockForward(model->tape, summed,
            &block, &blockConfig, &state));
    UMI_DECODER_TRY(UmiAiTrainingMatmul(model->tape, state,
        model->parameters[UMI_DECODER_LOGITS], &logits));
#undef UMI_DECODER_TRY
    *outLogits = logits;
    return UMI_STATUS_OK;
}

/* Unmasked callers retain their existing graph and numerical order. */
static UmiStatus ForwardLogits(UmiAiTrainingDecoder *model,
    const size_t *tokens, size_t tokenCount, UmiAiTrainingTensor **outLogits)
{
    return ForwardLogitsSelected(model, tokens, tokenCount, NULL, 0U, outLogits);
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

/* Shared selected-objective composition replaces the separate evaluation and SGD lifecycle blocks. Existing all-position entry points retain their contract through the same forward, backward and rewind services; the previous implementations remain for owner review. The former implementation is retained for owner review. */
#if 0
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


#endif
/* Masked and all-position objectives share the same forward graph and update
 * transaction. Inspect the entire selection before building tensors so a bad
 * record cannot consume graph memory or silently hide an invalid target. */
/* The combined policy implementation below retains this loss-only transaction and adds optional attention selection. Legacy APIs still choose the unmasked forward path; padded APIs validate that scored rows contain real context. The former implementation is retained for owner review. */
#if 0
static UmiStatus EvaluateOrTrainSelected(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    int train, float learningRate, float *outLoss)
{
    if (outLoss == NULL || targetIds == NULL ||
        (train && (!isfinite(learningRate) || learningRate <= 0.0F)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ValidateInputs(model, inputIds, targetIds, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    if (selected != NULL) {
        if (selectedCount != tokenCount) return UMI_STATUS_INVALID_ARGUMENT;
        size_t active = 0U;
        for (size_t i = 0U; i < tokenCount; ++i) {
            if (selected[i] > 1U) return UMI_STATUS_INVALID_ARGUMENT;
            active += selected[i];
        }
        if (active == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (train && model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    float value = 0.0F;
    status = ForwardLogits(model, inputIds, tokenCount, &logits);
    if (status == UMI_STATUS_OK) {
        status = selected != NULL ?
            UmiAiTrainingMaskedCrossEntropy(model->tape, logits, targetIds,
                tokenCount, selected, selectedCount, &loss) :
            UmiAiTrainingCrossEntropy(model->tape, logits, targetIds,
                tokenCount, &loss);
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &value, 1U);
    if (status == UMI_STATUS_OK && train)
        status = UmiAiTrainingBackward(model->tape, loss);
    if (status == UMI_STATUS_OK && train)
        status = UmiAiTrainingSgdStepMany(model->tape, model->parameters,
            UMI_DECODER_PARAMETER_COUNT, learningRate);
    /* The decoder exclusively owns an unchanged parameter prefix, making
     * this rewind allocation-free after either a failed or successful graph. */
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    if (train) model->completedSteps++;
    *outLoss = value;
    return UMI_STATUS_OK;
}
#endif

/* Padding is an input policy, independent of objective selection. Validate all
 * bytes before graph construction and require the objective to be a subset.
 * All token/target IDs remain valid vocabulary entries, including placeholders. */
static UmiStatus ValidateAttentionSelection(const unsigned char *attention,
    size_t attentionCount, size_t tokenCount)
{
    if (attention == NULL || attentionCount != tokenCount)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0; i < tokenCount; ++i)
        if (attention[i] > 1U) return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/* Share token-policy admission between SGD, evaluation and AdamW. A missing
 * objective mask denotes the legacy all-token objective; a padding mask always
 * requires an explicit nonempty objective contained within its attended rows. */
static UmiStatus ValidateTrainingSelection(size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    const unsigned char *attention, size_t attentionCount)
{
    UmiStatus status;
    if (attention != NULL) {
        status = ValidateAttentionSelection(attention, attentionCount, tokenCount);
        if (status != UMI_STATUS_OK || selected == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (selected != NULL) {
        if (selectedCount != tokenCount) return UMI_STATUS_INVALID_ARGUMENT;
        size_t active = 0U;
        for (size_t i = 0U; i < tokenCount; ++i) {
            if (selected[i] > 1U ||
                (attention != NULL && selected[i] && !attention[i]))
                return UMI_STATUS_INVALID_ARGUMENT;
            active += selected[i];
        }
        if (active == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

static UmiStatus EvaluateOrTrainSelections(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    const unsigned char *attention, size_t attentionCount,
    int train, float learningRate, float *outLoss)
{
    if (outLoss == NULL || targetIds == NULL ||
        (train && (!isfinite(learningRate) || learningRate <= 0.0F)))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = ValidateInputs(model, inputIds, targetIds, tokenCount);
    if (status != UMI_STATUS_OK) return status;
/* Token admission is shared with the new AdamW path, preserving SGD and evaluation policy without duplicating the subset rules. The former implementation is retained for owner review. */
#if 0
    if (attention != NULL) {
        status = ValidateAttentionSelection(attention, attentionCount, tokenCount);
        if (status != UMI_STATUS_OK || selected == NULL)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (selected != NULL) {
        if (selectedCount != tokenCount) return UMI_STATUS_INVALID_ARGUMENT;
        size_t active = 0U;
        for (size_t i = 0U; i < tokenCount; ++i) {
            if (selected[i] > 1U ||
                (attention != NULL && selected[i] && !attention[i]))
                return UMI_STATUS_INVALID_ARGUMENT;
            active += selected[i];
        }
        if (active == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    }

#endif
    status = ValidateTrainingSelection(tokenCount, selected, selectedCount,
        attention, attentionCount);
    if (status != UMI_STATUS_OK) return status;
    if (train && model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    float value = 0.0F;
    status = ForwardLogitsSelected(model, inputIds, tokenCount,
        attention, attentionCount, &logits);
    if (status == UMI_STATUS_OK) {
        status = selected != NULL ?
            UmiAiTrainingMaskedCrossEntropy(model->tape, logits, targetIds,
                tokenCount, selected, selectedCount, &loss) :
            UmiAiTrainingCrossEntropy(model->tape, logits, targetIds,
                tokenCount, &loss);
    }
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &value, 1U);
    if (status == UMI_STATUS_OK && train)
        status = UmiAiTrainingBackward(model->tape, loss);
    if (status == UMI_STATUS_OK && train)
        status = UmiAiTrainingSgdStepMany(model->tape, model->parameters,
            UMI_DECODER_PARAMETER_COUNT, learningRate);
    /* The decoder exclusively owns an unchanged parameter prefix, making
     * this rewind allocation-free after either a failed or successful graph. */
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    if (train) model->completedSteps++;
    *outLoss = value;
    return UMI_STATUS_OK;
}

static UmiStatus EvaluateOrTrainSelected(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    int train, float learningRate, float *outLoss)
{
    return EvaluateOrTrainSelections(model, inputIds, targetIds, tokenCount,
        selected, selectedCount, NULL, 0U, train, learningRate, outLoss);
}

UmiStatus UmiAiTrainingDecoderEvaluate(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    float *outLoss)
{
    return EvaluateOrTrainSelected(model, inputIds, targetIds, tokenCount,
        NULL, 0U, 0, 0.0F, outLoss);
}

UmiStatus UmiAiTrainingDecoderTrainStep(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    float learningRate, float *outLoss)
{
    return EvaluateOrTrainSelected(model, inputIds, targetIds, tokenCount,
        NULL, 0U, 1, learningRate, outLoss);
}

UmiStatus UmiAiTrainingDecoderEvaluateMasked(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount, float *outLoss)
{
    if (selected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return EvaluateOrTrainSelected(model, inputIds, targetIds, tokenCount,
        selected, selectedCount, 0, 0.0F, outLoss);
}

UmiStatus UmiAiTrainingDecoderTrainMaskedStep(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss)
{
    if (selected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return EvaluateOrTrainSelected(model, inputIds, targetIds, tokenCount,
        selected, selectedCount, 1, learningRate, outLoss);
}


/* The decoder owns all temporary tensors. Read into private storage and rewind
 * before publishing, so a refused graph leaves the caller buffer unchanged. */
UmiStatus UmiAiTrainingDecoderLogitsPadded(UmiAiTrainingDecoder *model,
    const size_t *inputIds, size_t tokenCount,
    const unsigned char *attention, size_t attentionCount,
    float *outLogits, size_t capacity)
{
    UmiStatus status = ValidateInputs(model, inputIds, NULL, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    if (outLogits == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ValidateAttentionSelection(attention, attentionCount, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    const size_t columns = (size_t)model->config.vocabularySize;
    const size_t needed = tokenCount * columns;
    if (capacity < needed) return UMI_STATUS_CAPACITY_EXCEEDED;
    float values[UMI_AI_TRAINING_MAX_ELEMENTS];
    UmiAiTrainingTensor *logits = NULL;
    status = ForwardLogitsSelected(model, inputIds, tokenCount,
        attention, attentionCount, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(logits, values, needed);
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    /* Residual paths compute padded rows internally. Do not expose those as
     * predictions: zero-filled rows are placeholders, never probability data. */
    for (size_t row = 0; row < tokenCount; ++row)
        if (!attention[row])
            for (size_t col = 0; col < columns; ++col)
                values[row * columns + col] = 0.0F;
    memcpy(outLogits, values, needed * sizeof(*values));
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderEvaluatePadded(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount, float *outLoss)
{
    if (attention == NULL || selected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return EvaluateOrTrainSelections(model, inputIds, targetIds, tokenCount,
        selected, selectedCount, attention, attentionCount, 0, 0.0F, outLoss);
}

UmiStatus UmiAiTrainingDecoderTrainPaddedStep(UmiAiTrainingDecoder *model,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss)
{
    if (attention == NULL || selected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return EvaluateOrTrainSelections(model, inputIds, targetIds, tokenCount,
        selected, selectedCount, attention, attentionCount, 1, learningRate, outLoss);
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
/* Shared, read-only effective-batch gradient reduction for SGD and AdamW.
 * Keep the original SGD-only reduction below as an engineering reference,
 * rather than maintaining divergent active gradient-weighting algorithms.
 * The caller owns the returned float storage until it invokes one optimiser
 * transaction. On failure no parameter or optimiser state is updated. */
/* Accumulate means by scored rows, keeping complete sequence groups and shared selection admission. The unselected wrapper preserves the earlier objective. Temporary graphs and the final optimizer transaction retain their existing ownership. The former implementation is retained for owner review. */
#if 0
static UmiStatus CollectAccumulatedGradients(
    UmiAiTrainingDecoder *model,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    size_t microbatchSequences,
    float **outStorage,
    const float **outGradientPointers,
    float *outMeanLoss)
{
    size_t counts[UMI_DECODER_PARAMETER_COUNT] = {0};
    size_t offsets[UMI_DECODER_PARAMETER_COUNT] = {0};
    double *accumulated = NULL;
    float *reduced = NULL;
    float *scratch = NULL;
    size_t totalElements = 0U;
    size_t context, microbatchTokens;
    double weightedLoss = 0.0;
    UmiStatus status = UMI_STATUS_OK;

    if (model == NULL || inputIds == NULL || targetIds == NULL ||
        outStorage == NULL || outGradientPointers == NULL ||
        outMeanLoss == NULL || tokenCount == 0U ||
        tokenCount > UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS ||
        microbatchSequences == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    context = (size_t)model->config.contextLength;
    if (context == 0U || tokenCount % context != 0U ||
        microbatchSequences > UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS / context)
        return UMI_STATUS_INVALID_ARGUMENT;
    microbatchTokens = microbatchSequences * context;

    for (size_t i = 0U; i < tokenCount; ++i) {
        if (inputIds[i] >= (size_t)model->config.vocabularySize ||
            targetIds[i] >= (size_t)model->config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t firstCount = tokenCount < microbatchTokens ? tokenCount : microbatchTokens;
    status = ValidateInputs(model, inputIds, targetIds, firstCount);
    if (status != UMI_STATUS_OK) return status;

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
    for (size_t start = 0U; start < tokenCount;) {
        UmiAiTrainingTensor *logits = NULL, *loss = NULL;
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
                double sum = accumulated[offsets[p] + i] +
                             (double)scratch[i] * (double)currentTokens;
                if (!isfinite(sum)) status = UMI_STATUS_INVALID_STATE;
                else accumulated[offsets[p] + i] = sum;
            }
        }
        /* Always discard activation graphs. The parameter leaves survive. */
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
    }
    double meanLoss = weightedLoss / (double)tokenCount;
    if (!isfinite(meanLoss) || meanLoss > (double)FLT_MAX || meanLoss < 0.0) {
        status = UMI_STATUS_INVALID_STATE;
        goto finish;
    }
    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p)
        outGradientPointers[p] = &reduced[offsets[p]];
    *outMeanLoss = (float)meanLoss;
    *outStorage = reduced;
    reduced = NULL;
finish:
    free(scratch);
    free(reduced);
    free(accumulated);
    return status;
}

#endif
static UmiStatus CollectAccumulatedGradientsSelected(
    UmiAiTrainingDecoder *model,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    size_t microbatchSequences,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount,
    float **outStorage,
    const float **outGradientPointers,
    float *outMeanLoss)
{
    size_t counts[UMI_DECODER_PARAMETER_COUNT] = {0};
    size_t offsets[UMI_DECODER_PARAMETER_COUNT] = {0};
    double *accumulated = NULL;
    float *reduced = NULL;
    float *scratch = NULL;
    size_t totalElements = 0U, largestParameter = 0U;
    size_t scoredTokens = tokenCount;
    size_t context, microbatchTokens;
    double weightedLoss = 0.0;
    UmiStatus status = UMI_STATUS_OK;

    if (model == NULL || inputIds == NULL || targetIds == NULL ||
        outStorage == NULL || outGradientPointers == NULL ||
        outMeanLoss == NULL || tokenCount == 0U ||
        tokenCount > UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS ||
        microbatchSequences == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    context = (size_t)model->config.contextLength;
    if (context == 0U || tokenCount % context != 0U ||
        microbatchSequences > UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS / context)
        return UMI_STATUS_INVALID_ARGUMENT;
    microbatchTokens = microbatchSequences * context;
    status = ValidateTrainingSelection(tokenCount, selected, selectedCount,
                                       attention, attentionCount);
    if (status != UMI_STATUS_OK) return status;
    if (selected != NULL) {
        scoredTokens = 0U;
        for (size_t i = 0U; i < tokenCount; ++i) scoredTokens += selected[i];
    }

    for (size_t i = 0U; i < tokenCount; ++i) {
        if (inputIds[i] >= (size_t)model->config.vocabularySize ||
            targetIds[i] >= (size_t)model->config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    size_t firstCount = tokenCount < microbatchTokens ? tokenCount : microbatchTokens;
    status = ValidateInputs(model, inputIds, targetIds, firstCount);
    if (status != UMI_STATUS_OK) return status;

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
        if (count > largestParameter) largestParameter = count;
        totalElements += count;
    }
    if (totalElements > SIZE_MAX / sizeof(double) ||
        totalElements > SIZE_MAX / sizeof(float)) {
        status = UMI_STATUS_CAPACITY_EXCEEDED;
        goto finish;
    }
    accumulated = calloc(totalElements, sizeof(*accumulated));
    reduced = malloc(totalElements * sizeof(*reduced));
    /* One parameter is read at a time; scratch follows the largest actual
     * shape instead of reserving the global tensor limit for every model. */
    scratch = malloc(largestParameter * sizeof(*scratch));
    if (accumulated == NULL || reduced == NULL || scratch == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto finish;
    }
    for (size_t start = 0U; start < tokenCount;) {
        UmiAiTrainingTensor *logits = NULL, *loss = NULL;
        size_t remaining = tokenCount - start;
        size_t currentTokens = remaining < microbatchTokens ?
                               remaining : microbatchTokens;
        size_t currentScored = currentTokens;
        if (selected != NULL) {
            currentScored = 0U;
            for (size_t i = 0U; i < currentTokens; ++i)
                currentScored += selected[start + i];
        }
        /* All IDs and policies were admitted above. A whole independent
         * sequence group with no objective contributes no gradient or loss;
         * skip its graph without discarding context inside a scored group. */
        if (currentScored == 0U) { start += currentTokens; continue; }
        float currentLoss = 0.0F;
        UmiStatus rewindStatus;
        status = ForwardLogitsSelected(model, &inputIds[start], currentTokens,
            attention == NULL ? NULL : &attention[start],
            attention == NULL ? 0U : currentTokens, &logits);
        if (status == UMI_STATUS_OK)
            status = selected == NULL
                ? UmiAiTrainingCrossEntropy(model->tape, logits,
                    &targetIds[start], currentTokens, &loss)
                : UmiAiTrainingMaskedCrossEntropy(model->tape, logits,
                    &targetIds[start], currentTokens, &selected[start], currentTokens, &loss);
        if (status == UMI_STATUS_OK)
            status = UmiAiTrainingTensorValues(loss, &currentLoss, 1U);
        if (status == UMI_STATUS_OK)
            status = UmiAiTrainingBackward(model->tape, loss);
        if (status == UMI_STATUS_OK) {
            weightedLoss += (double)currentLoss * (double)currentScored;
            if (!isfinite(weightedLoss)) status = UMI_STATUS_INVALID_STATE;
        }
        for (size_t p = 0U;
             status == UMI_STATUS_OK && p < UMI_DECODER_PARAMETER_COUNT; ++p) {
            status = UmiAiTrainingTensorGradients(model->parameters[p], scratch,
                                                  counts[p]);
            for (size_t i = 0U; status == UMI_STATUS_OK && i < counts[p]; ++i) {
                double sum = accumulated[offsets[p] + i] +
                             (double)scratch[i] * (double)currentScored;
                if (!isfinite(sum)) status = UMI_STATUS_INVALID_STATE;
                else accumulated[offsets[p] + i] = sum;
            }
        }
        /* Always discard activation graphs. The parameter leaves survive. */
        rewindStatus = RewindParameters(model);
        if (status == UMI_STATUS_OK) status = rewindStatus;
        if (status != UMI_STATUS_OK) goto finish;
        start += currentTokens;
    }
    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p) {
        for (size_t i = 0U; i < counts[p]; ++i) {
            double mean = accumulated[offsets[p] + i] / (double)scoredTokens;
            if (!isfinite(mean) || mean > (double)FLT_MAX ||
                mean < -(double)FLT_MAX) {
                status = UMI_STATUS_INVALID_STATE;
                goto finish;
            }
            reduced[offsets[p] + i] = (float)mean;
        }
    }
    double meanLoss = weightedLoss / (double)scoredTokens;
    if (!isfinite(meanLoss) || meanLoss > (double)FLT_MAX || meanLoss < 0.0) {
        status = UMI_STATUS_INVALID_STATE;
        goto finish;
    }
    for (size_t p = 0U; p < UMI_DECODER_PARAMETER_COUNT; ++p)
        outGradientPointers[p] = &reduced[offsets[p]];
    *outMeanLoss = (float)meanLoss;
    *outStorage = reduced;
    reduced = NULL;
finish:
    free(scratch);
    free(reduced);
    free(accumulated);
    return status;
}

/* Preserve the all-token arithmetic order through the same reducer. */
static UmiStatus CollectAccumulatedGradients(
    UmiAiTrainingDecoder *model, const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences, float **outStorage,
    const float **outGradientPointers, float *outMeanLoss)
{
    return CollectAccumulatedGradientsSelected(model, inputIds, targetIds,
        tokenCount, microbatchSequences, NULL, 0U, NULL, 0U,
        outStorage, outGradientPointers, outMeanLoss);
}

UmiStatus UmiAiTrainingDecoderTrainAccumulated(
    UmiAiTrainingDecoder *model,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    size_t microbatchSequences,
    float learningRate,
    float *outLoss)
{
    float *storage = NULL;
    float candidateLoss = 0.0F;
    const float *gradients[UMI_DECODER_PARAMETER_COUNT] = {0};
    UmiStatus status;
    if (model == NULL || outLoss == NULL ||
        !isfinite(learningRate) || learningRate <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = CollectAccumulatedGradients(model, inputIds, targetIds,
                tokenCount, microbatchSequences, &storage, gradients,
                &candidateLoss);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingSgdStepWithGradients(model->tape, model->parameters,
                 gradients, UMI_DECODER_PARAMETER_COUNT, learningRate);
    free(storage);
    if (status != UMI_STATUS_OK) return status;
    ++model->completedSteps;
    *outLoss = candidateLoss;
    return UMI_STATUS_OK;
}

/* The original SGD-only implementation is retained without execution for
 * review of the common reducer transition. The new active reducer above is
 * shared by both SGD and AdamW; do not physically delete this source without
 * explicit owner approval. */
#if 0
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

#endif /* retained SGD-only gradient accumulator */

/*-----------------------------------------------------------------------------
 * Stateful AdamW training composes the same canonical forward, loss and
 * backward graph used by ordinary SGD. Only the update rule differs.
 *
 * Preserve the existing TrainStep and accumulated-SGD paths unchanged.
 * Model-only checkpoints intentionally do not claim exact AdamW continuation:
 * first/second moment state must eventually have its own persisted contract.
 *---------------------------------------------------------------------------*/
UmiStatus UmiAiTrainingDecoderAdamWCreate(
    UmiAiTrainingDecoder *model,
    const UmiAiTrainingAdamWConfig *config,
    UmiAiTrainingAdamW **outOptimizer)
{
    if (model == NULL || outOptimizer == NULL || model->completedSteps != 0U)
        return UMI_STATUS_INVALID_STATE;
    return UmiAiTrainingAdamWCreate(model->tape, model->parameters,
        UMI_DECODER_PARAMETER_COUNT, config, outOptimizer);
}

/* The canonical AdamW transaction now accepts explicit objective and attention selections. The legacy wrapper still selects all tokens and uses the original unmasked graph. All wrappers share binding validation, rewind and the failure-atomic optimizer commit. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiTrainingDecoderTrainAdamWStep(
    UmiAiTrainingDecoder *model,
    UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    float learningRate,
    float *outLoss)
{
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    UmiStatus status;
    float candidateLoss = 0.0F;
    if (model == NULL || optimizer == NULL || outLoss == NULL ||
        targetIds == NULL || !isfinite(learningRate) || learningRate <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdamWValidateBinding(optimizer,
        model->tape, model->parameters, UMI_DECODER_PARAMETER_COUNT,
        model->completedSteps);
    if (status != UMI_STATUS_OK) return status;
    if (model->completedSteps == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ValidateInputs(model, inputIds, targetIds, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    status = ForwardLogits(model, inputIds, tokenCount, &logits);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingCrossEntropy(model->tape, logits, targetIds,
                                          tokenCount, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &candidateLoss, 1U);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingBackward(model->tape, loss);
    /* The leaf gradients remain readable after the temporary graph is freed.
     * No parameter is modified until AdamW validates all proposed weights. */
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingAdamWStep(optimizer, learningRate);
    if (status != UMI_STATUS_OK) return status;
    ++model->completedSteps;
    *outLoss = candidateLoss;
    return UMI_STATUS_OK;
}


#endif
static UmiStatus TrainAdamWSelections(
    UmiAiTrainingDecoder *model,
    UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds,
    const size_t *targetIds,
    size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    const unsigned char *attention, size_t attentionCount,
    float learningRate,
    float *outLoss)
{
    UmiAiTrainingTensor *logits = NULL, *loss = NULL;
    UmiStatus status;
    float candidateLoss = 0.0F;
    if (model == NULL || optimizer == NULL || outLoss == NULL ||
        targetIds == NULL || !isfinite(learningRate) || learningRate <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdamWValidateBinding(optimizer,
        model->tape, model->parameters, UMI_DECODER_PARAMETER_COUNT,
        model->completedSteps);
    if (status != UMI_STATUS_OK) return status;
    if (model->completedSteps == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = ValidateInputs(model, inputIds, targetIds, tokenCount);
    if (status != UMI_STATUS_OK) return status;
    status = ValidateTrainingSelection(tokenCount, selected, selectedCount,
        attention, attentionCount);
    if (status != UMI_STATUS_OK) return status;
    status = ForwardLogitsSelected(model, inputIds, tokenCount,
        attention, attentionCount, &logits);
    if (status == UMI_STATUS_OK)
        status = selected != NULL ?
            UmiAiTrainingMaskedCrossEntropy(model->tape, logits, targetIds,
                tokenCount, selected, selectedCount, &loss) :
            UmiAiTrainingCrossEntropy(model->tape, logits, targetIds,
                tokenCount, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &candidateLoss, 1U);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingBackward(model->tape, loss);
    /* The leaf gradients remain readable after the temporary graph is freed.
     * No parameter is modified until AdamW validates all proposed weights. */
    UmiStatus rewindStatus = RewindParameters(model);
    if (status == UMI_STATUS_OK) status = rewindStatus;
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingAdamWStep(optimizer, learningRate);
    if (status != UMI_STATUS_OK) return status;
    ++model->completedSteps;
    *outLoss = candidateLoss;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderTrainAdamWStep(
    UmiAiTrainingDecoder *model, UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    float learningRate, float *outLoss)
{
    return TrainAdamWSelections(model, optimizer, inputIds, targetIds,
        tokenCount, NULL, 0, NULL, 0, learningRate, outLoss);
}

UmiStatus UmiAiTrainingDecoderTrainAdamWMaskedStep(
    UmiAiTrainingDecoder *model, UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss)
{
    if (selected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return TrainAdamWSelections(model, optimizer, inputIds, targetIds,
        tokenCount, selected, selectedCount, NULL, 0, learningRate, outLoss);
}

UmiStatus UmiAiTrainingDecoderTrainAdamWPaddedStep(
    UmiAiTrainingDecoder *model, UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds, size_t tokenCount,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss)
{
    if (selected == NULL || attention == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return TrainAdamWSelections(model, optimizer, inputIds, targetIds,
        tokenCount, selected, selectedCount, attention, attentionCount,
        learningRate, outLoss);
}

/* Stateful microbatch AdamW uses precisely the same gradient reduction as
 * accumulated SGD, but applies ONE transactional AdamW update. Validate the
 * borrowed optimiser binding BEFORE constructing any temporary graph. */
/* Share optimizer binding and the final commit across all-token, selected-target and padded microbatch APIs. No parameter or moment is updated while partial gradients are being collected. The former implementation is retained for owner review. */
#if 0
UmiStatus UmiAiTrainingDecoderTrainAdamWAccumulated(
    UmiAiTrainingDecoder *model,
    UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences,
    float learningRate, float *outLoss)
{
    float *storage = NULL, candidateLoss = 0.0F;
    const float *gradients[UMI_DECODER_PARAMETER_COUNT] = {0};
    UmiStatus status;
    if (model == NULL || optimizer == NULL || outLoss == NULL ||
        !isfinite(learningRate) || learningRate <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdamWValidateBinding(optimizer,
        model->tape, model->parameters, UMI_DECODER_PARAMETER_COUNT,
        model->completedSteps);
    if (status != UMI_STATUS_OK) return status;
    if (model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = CollectAccumulatedGradients(model, inputIds, targetIds,
                tokenCount, microbatchSequences, &storage, gradients,
                &candidateLoss);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingAdamWStepWithGradients(
        optimizer, gradients, learningRate);
    free(storage);
    if (status != UMI_STATUS_OK) return status;
    ++model->completedSteps;
    *outLoss = candidateLoss;
    return UMI_STATUS_OK;
}


#endif
static UmiStatus TrainAdamWAccumulatedSelections(
    UmiAiTrainingDecoder *model,
    UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss)
{
    float *storage = NULL, candidateLoss = 0.0F;
    const float *gradients[UMI_DECODER_PARAMETER_COUNT] = {0};
    UmiStatus status;
    if (model == NULL || optimizer == NULL || outLoss == NULL ||
        !isfinite(learningRate) || learningRate <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdamWValidateBinding(optimizer,
        model->tape, model->parameters, UMI_DECODER_PARAMETER_COUNT,
        model->completedSteps);
    if (status != UMI_STATUS_OK) return status;
    if (model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = CollectAccumulatedGradientsSelected(model, inputIds, targetIds,
                tokenCount, microbatchSequences, attention, attentionCount,
                selected, selectedCount, &storage, gradients,
                &candidateLoss);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingAdamWStepWithGradients(
        optimizer, gradients, learningRate);
    free(storage);
    if (status != UMI_STATUS_OK) return status;
    ++model->completedSteps;
    *outLoss = candidateLoss;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderTrainAdamWAccumulated(
    UmiAiTrainingDecoder *model, UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences,
    float learningRate, float *outLoss)
{
    return TrainAdamWAccumulatedSelections(model, optimizer, inputIds, targetIds,
        tokenCount, microbatchSequences, NULL, 0U, NULL, 0U, learningRate, outLoss);
}

/* Selected objectives are explicit at public entry points; internal NULL
 * selections remain reserved for the existing all-token compatibility API. */
UmiStatus UmiAiTrainingDecoderTrainAdamWMaskedAccumulated(
    UmiAiTrainingDecoder *model, UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss)
{
    if (selected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return TrainAdamWAccumulatedSelections(model, optimizer, inputIds, targetIds,
        tokenCount, microbatchSequences, NULL, 0U, selected, selectedCount,
        learningRate, outLoss);
}
UmiStatus UmiAiTrainingDecoderTrainAdamWPaddedAccumulated(
    UmiAiTrainingDecoder *model, UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences,
    const unsigned char *attention, size_t attentionCount,
    const unsigned char *selected, size_t selectedCount,
    float learningRate, float *outLoss)
{
    if (attention == NULL || selected == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return TrainAdamWAccumulatedSelections(model, optimizer, inputIds, targetIds,
        tokenCount, microbatchSequences, attention, attentionCount,
        selected, selectedCount, learningRate, outLoss);
}
/* A decoder loaded from a combined model/AdamW checkpoint is already stepped.
 * Fresh-start AdamWCreate still refuses such models. Only the codec invokes
 * this internal bridge after verifying the checkpoint's shape and identity. */
UmiStatus UmiAiDecoderAdamWRestoreCreate(
    UmiAiTrainingDecoder *model, const UmiAiTrainingAdamWConfig *config,
    UmiAiTrainingAdamW **outOptimizer)
{
    if (model == NULL || outOptimizer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingAdamWCreate(model->tape, model->parameters,
           UMI_DECODER_PARAMETER_COUNT, config, outOptimizer);
}

UmiStatus UmiAiDecoderAdamWCheckBinding(
    const UmiAiTrainingDecoder *model, const UmiAiTrainingAdamW *optimizer)
{
    if (model == NULL || optimizer == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingAdamWValidateBinding(optimizer, model->tape,
        (UmiAiTrainingTensor *const *)model->parameters,
        UMI_DECODER_PARAMETER_COUNT, model->completedSteps);
}

/* Clip once after weighted microbatch reduction. Clipping each microbatch
 * independently would change the optimiser update and checkpoint trajectory.
 * This overload retains the existing unmodified SGD/AdamW entry points. */
UmiStatus UmiAiTrainingDecoderTrainAdamWAccumulatedClipped(
    UmiAiTrainingDecoder *model,
    UmiAiTrainingAdamW *optimizer,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, size_t microbatchSequences,
    float learningRate, float maximumGradientNorm,
    float *outLoss, UmiAiTrainingGradientClipReport *outReport)
{
    float *storage = NULL, candidateLoss = 0.0F;
    const float *gradients[UMI_DECODER_PARAMETER_COUNT] = {0};
    UmiAiTrainingGradientClipReport candidateReport;
    UmiStatus status;
    if (model == NULL || optimizer == NULL || outLoss == NULL ||
        outReport == NULL || !isfinite(learningRate) ||
        learningRate <= 0.0F || !isfinite(maximumGradientNorm) ||
        maximumGradientNorm <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingAdamWValidateBinding(optimizer,
        model->tape, model->parameters, UMI_DECODER_PARAMETER_COUNT,
        model->completedSteps);
    if (status != UMI_STATUS_OK) return status;
    if (model->completedSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = CollectAccumulatedGradients(model, inputIds, targetIds,
                tokenCount, microbatchSequences, &storage, gradients,
                &candidateLoss);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingAdamWStepWithClippedGradients(
        optimizer, gradients, learningRate, maximumGradientNorm,
        &candidateReport);
    free(storage);
    if (status != UMI_STATUS_OK) return status;
    ++model->completedSteps;
    *outLoss = candidateLoss;
    *outReport = candidateReport;
    return UMI_STATUS_OK;
}
