/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/decoder_logit_adapter.c
 * PURPOSE:
 *   Adapt the frozen decoder's actual logits without forking its Transformer,
 *   tensor mathematics, tokenizer, random generator or parameter writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/decoder_logit_adapter.h"
#include "umicom/ai/training/initialisation.h"
#include "umicom/ai/training/tape.h"
#include "umicom/ai/training/selected_loss.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingDecoderLogitAdapter {
    UmiAiTrainingDecoder *decoder;
    UmiAiTrainingDecoderConfig decoderConfig;
    uint64_t decoderSteps;
    uint64_t adapterSteps;
    size_t rank;
    size_t retainNodes;
    UmiAiTrainingTape *tape;
    UmiAiTrainingTensor *down;
    UmiAiTrainingTensor *up;
};

static int AdapterConfigEqual(const UmiAiTrainingDecoderConfig *a,
                              const UmiAiTrainingDecoderConfig *b)
{
    return a->vocabularySize == b->vocabularySize &&
           a->contextLength == b->contextLength &&
           a->channels == b->channels &&
           a->hiddenChannels == b->hiddenChannels &&
           a->attentionHeads == b->attentionHeads &&
           a->initialSeed == b->initialSeed;
}

static UmiStatus AdapterCurrent(const UmiAiTrainingDecoderLogitAdapter *adapter)
{
    UmiAiTrainingDecoderConfig observed = {0};
    uint64_t steps = 0U;
    UmiStatus status;
    if (adapter == NULL || adapter->decoder == NULL || adapter->tape == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(adapter->decoder, &observed, &steps);
    if (status != UMI_STATUS_OK) return status;
    if (!AdapterConfigEqual(&observed, &adapter->decoderConfig) ||
        steps != adapter->decoderSteps)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}

static UmiStatus AdapterValidateInput(
    const UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputIds, const size_t *targetIds,
    size_t tokenCount, int requireTargets)
{
    UmiStatus status = AdapterCurrent(adapter);
    if (status != UMI_STATUS_OK) return status;
    size_t vocabulary = (size_t)adapter->decoderConfig.vocabularySize;
    if (inputIds == NULL || (requireTargets && targetIds == NULL) ||
        tokenCount != (size_t)adapter->decoderConfig.contextLength ||
        tokenCount > UMI_AI_TRAINING_MAX_ELEMENTS / vocabulary ||
        tokenCount > UMI_AI_TRAINING_MAX_ELEMENTS / adapter->rank)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < tokenCount; ++i) {
        if (inputIds[i] >= vocabulary ||
            (requireTargets && targetIds[i] >= vocabulary))
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

void UmiAiTrainingDecoderLogitAdapterDestroy(
    UmiAiTrainingDecoderLogitAdapter *adapter)
{
    if (adapter == NULL) return;
    UmiAiTrainingTapeDestroy(adapter->tape);
    free(adapter);
}

UmiStatus UmiAiTrainingDecoderLogitAdapterCreate(
    UmiAiTrainingDecoder *decoder, size_t rank, uint64_t initialSeed,
    UmiAiTrainingDecoderLogitAdapter **outAdapter)
{
    UmiAiTrainingDecoderLogitAdapter *created = NULL;
    UmiAiTrainingDecoderConfig config = {0};
    uint64_t steps = 0U;
    UmiAiTrainingRandomState random = {0};
    float *downValues = NULL;
    float *upValues = NULL;
    UmiStatus status;
    size_t vocabulary, downCount, upCount;

    if (decoder == NULL || outAdapter == NULL || *outAdapter != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(decoder, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    vocabulary = (size_t)config.vocabularySize;
    if (vocabulary < 2U || config.contextLength == 0U || rank == 0U ||
        rank > vocabulary || rank > UMI_AI_TRAINING_MAX_ELEMENTS / vocabulary ||
        (size_t)config.contextLength > UMI_AI_TRAINING_MAX_ELEMENTS / vocabulary ||
        (size_t)config.contextLength > UMI_AI_TRAINING_MAX_ELEMENTS / rank)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    downCount = vocabulary * rank;
    upCount = rank * vocabulary;

    created = calloc(1U, sizeof(*created));
    downValues = malloc(downCount * sizeof(*downValues));
    upValues = calloc(upCount, sizeof(*upValues));
    if (created == NULL || downValues == NULL || upValues == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto failure;
    }
    created->decoder = decoder;
    created->decoderConfig = config;
    created->decoderSteps = steps;
    created->rank = rank;

    status = UmiAiTrainingRandomSeed(&random, initialSeed);
    if (status != UMI_STATUS_OK) goto failure;
    float limit = 1.0F / sqrtf((float)vocabulary);
    if (!isfinite(limit) || limit <= 0.0F) {
        status = UMI_STATUS_INVALID_ARGUMENT;
        goto failure;
    }
    status = UmiAiTrainingRandomFillUniform(
        &random, -limit, limit, downValues, downCount);
    if (status != UMI_STATUS_OK) goto failure;

    status = UmiAiTrainingTapeCreate(&created->tape);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingTensorCreate(created->tape, vocabulary, rank,
        downValues, downCount, 1, &created->down);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingTensorCreate(created->tape, rank, vocabulary,
        upValues, upCount, 1, &created->up);
    if (status != UMI_STATUS_OK) goto failure;
    status = UmiAiTrainingTapeCheckpoint(created->tape, &created->retainNodes);
    if (status != UMI_STATUS_OK) goto failure;
    *outAdapter = created;
    free(downValues);
    free(upValues);
    return UMI_STATUS_OK;

failure:
    free(downValues);
    free(upValues);
    UmiAiTrainingDecoderLogitAdapterDestroy(created);
    return status;
}

UmiStatus UmiAiTrainingDecoderLogitAdapterInspect(
    const UmiAiTrainingDecoderLogitAdapter *adapter,
    UmiAiTrainingDecoderLogitInfo *outInfo)
{
    UmiStatus status;
    if (outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterCurrent(adapter);
    if (status != UMI_STATUS_OK) return status;
    UmiAiTrainingDecoderLogitInfo candidate = {
        .vocabularySize = (size_t)adapter->decoderConfig.vocabularySize,
        .contextLength = (size_t)adapter->decoderConfig.contextLength,
        .rank = adapter->rank,
        .trainableElements = 2U * (size_t)adapter->decoderConfig.vocabularySize * adapter->rank,
        .frozenDecoderSteps = adapter->decoderSteps,
        .completedAdapterSteps = adapter->adapterSteps
    };
    *outInfo = candidate;
    return UMI_STATUS_OK;
}

/* The native decoder produces frozen logits on its own authoritative tape.
 * The adapter creates a separate graph, applying a low-rank RESIDUAL directly
 * to those logits rather than changing any decoder projection internally. */
static UmiStatus AdapterForward(UmiAiTrainingDecoderLogitAdapter *adapter,
                                 const size_t *inputs, size_t tokenCount,
                                 UmiAiTrainingTensor **outAdjusted)
{
    UmiAiTrainingTensor *frozen = NULL, *hidden = NULL, *delta = NULL;
    UmiAiTrainingTensor *adjusted = NULL;
    const size_t vocabulary = (size_t)adapter->decoderConfig.vocabularySize;
    const size_t count = tokenCount * vocabulary;
    float *logits = malloc(count * sizeof(*logits));
    UmiStatus status;
    if (logits == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiAiTrainingDecoderLogits(adapter->decoder, inputs, tokenCount,
                                       logits, count);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorCreate(adapter->tape, tokenCount,
                   vocabulary, logits, count, 0, &frozen);
    free(logits);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingMatmul(adapter->tape, frozen, adapter->down, &hidden);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingMatmul(adapter->tape, hidden, adapter->up, &delta);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingAdd(adapter->tape, frozen, delta, &adjusted);
    if (status == UMI_STATUS_OK) *outAdjusted = adjusted;
    return status;
}

UmiStatus UmiAiTrainingDecoderLogitAdapterLogits(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, size_t tokenCount,
    float *outLogits, size_t capacity)
{
    UmiStatus status = AdapterValidateInput(adapter, inputTokenIds,
                                           NULL, tokenCount, 0);
    if (status != UMI_STATUS_OK) return status;
    if (outLogits == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t count = tokenCount * (size_t)adapter->decoderConfig.vocabularySize;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    float *candidate = malloc(count * sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiAiTrainingTensor *predicted = NULL;
    status = AdapterForward(adapter, inputTokenIds, tokenCount, &predicted);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(predicted, candidate, count);
    UmiStatus clean = UmiAiTrainingTapeRollback(adapter->tape, adapter->retainNodes);
    if (clean != UMI_STATUS_OK) status = clean;
    if (status == UMI_STATUS_OK) memcpy(outLogits, candidate, count * sizeof(*candidate));
    free(candidate);
    return status;
}

/* One forward pass is shared by read-only evaluation and adapter-only SGD.
 * Validate all token IDs before creating a transient graph or touching RNG. */
static UmiStatus AdapterLossStep(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputs, const size_t *targets, size_t tokenCount,
    const uint8_t *lossMask, size_t maskCount,
    int train, float learningRate, float *outLoss)
{
    UmiStatus status = AdapterValidateInput(adapter, inputs, targets,
                                           tokenCount, 1);
    if (status != UMI_STATUS_OK) return status;
    if (outLoss == NULL || (train && (!isfinite(learningRate) ||
                                      learningRate <= 0.0F)))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (train && adapter->adapterSteps == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiTrainingTensor *predicted = NULL, *loss = NULL;
    float candidate = 0.0F;
    /* The original full-context objective is unchanged. Explicitly selected
     * targets opt in to the Framework's tape-owned row-selection composition.
     * All submitted target IDs are validated by AdapterValidateInput, even
     * for ignored rows, matching the historical decoder contract. */
    if (lossMask != NULL && maskCount != tokenCount)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = AdapterForward(adapter, inputs, tokenCount, &predicted);
    if (status == UMI_STATUS_OK && lossMask == NULL)
        status = UmiAiTrainingCrossEntropy(adapter->tape, predicted,
                                          targets, tokenCount, &loss);
    else if (status == UMI_STATUS_OK)
        status = UmiAiTrainingSelectedCrossEntropy(adapter->tape, predicted,
            targets, tokenCount, lossMask, maskCount, &loss);
    if (status == UMI_STATUS_OK)
        status = UmiAiTrainingTensorValues(loss, &candidate, 1U);
    if (status == UMI_STATUS_OK && (!isfinite(candidate) || candidate < 0.0F))
        status = UMI_STATUS_INVALID_STATE;
    if (train && status == UMI_STATUS_OK)
        status = UmiAiTrainingBackward(adapter->tape, loss);
    if (train && status == UMI_STATUS_OK) {
        UmiAiTrainingTensor *weights[2] = {adapter->down, adapter->up};
        /* Only adapter parameters receive updates. The frozen decoder is
         * not a tape parameter and cannot be written by this optimiser. */
        status = UmiAiTrainingSgdStepMany(adapter->tape, weights, 2U, learningRate);
    }
    UmiStatus clean = UmiAiTrainingTapeRollback(adapter->tape, adapter->retainNodes);
    if (clean != UMI_STATUS_OK) return clean;
    if (status != UMI_STATUS_OK) return status;
    if (train) ++adapter->adapterSteps;
    *outLoss = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingDecoderLogitAdapterEvaluate(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    size_t tokenCount, float *outLoss)
{
    return AdapterLossStep(adapter, inputTokenIds, targetTokenIds,
                           tokenCount, NULL, 0U, 0, 0.0F, outLoss);
}

UmiStatus UmiAiTrainingDecoderLogitAdapterTrainStep(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    size_t tokenCount, float learningRate,
    UmiAiTrainingDecoderLogitStep *outReport)
{
    float prior = 0.0F;
    if (outReport == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = AdapterLossStep(adapter, inputTokenIds, targetTokenIds,
                                       tokenCount, NULL, 0U, 1,
                                       learningRate, &prior);
    if (status != UMI_STATUS_OK) return status;
    *outReport = (UmiAiTrainingDecoderLogitStep){
        .crossEntropyBeforeUpdate = prior,
        .tokenCount = tokenCount,
        .completedAdapterSteps = adapter->adapterSteps
    };
    return UMI_STATUS_OK;
}

/* An explicit assistant-response mask is applied AFTER the existing frozen
 * decoder has produced its canonical logits. The original unmasked entry
 * points retain the exact previously published cross-entropy semantics. */
UmiStatus UmiAiTrainingDecoderLogitAdapterEvaluateSelected(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    size_t tokenCount, const uint8_t *lossMask, size_t maskCount,
    float *outLoss)
{
    if (lossMask == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return AdapterLossStep(adapter, inputTokenIds, targetTokenIds,
        tokenCount, lossMask, maskCount, 0, 0.0F, outLoss);
}

UmiStatus UmiAiTrainingDecoderLogitAdapterTrainSelected(
    UmiAiTrainingDecoderLogitAdapter *adapter,
    const size_t *inputTokenIds, const size_t *targetTokenIds,
    size_t tokenCount, const uint8_t *lossMask, size_t maskCount,
    float learningRate, UmiAiTrainingDecoderLogitStep *outReport)
{
    float prior = 0.0F;
    if (outReport == NULL || lossMask == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = AdapterLossStep(adapter, inputTokenIds, targetTokenIds,
        tokenCount, lossMask, maskCount, 1, learningRate, &prior);
    if (status != UMI_STATUS_OK) return status;
    *outReport = (UmiAiTrainingDecoderLogitStep){
        .crossEntropyBeforeUpdate = prior,
        .tokenCount = tokenCount,
        .completedAdapterSteps = adapter->adapterSteps
    };
    return UMI_STATUS_OK;
}
