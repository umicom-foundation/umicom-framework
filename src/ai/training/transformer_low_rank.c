/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/transformer_low_rank.c
 * PURPOSE:
 *   Compose the existing low-rank tape primitive with the established six
 *   Transformer projection roles, retaining canonical attention/MLP ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/transformer_low_rank.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PROJECTION_COUNT 6U

static const UmiAiTrainingLowRankProjection *BindingAt(
    const UmiAiTrainingTransformerLowRankBindings *bindings, size_t index)
{
    if (bindings == NULL) return NULL;
    switch (index) {
    case 0U: return bindings->query;
    case 1U: return bindings->key;
    case 2U: return bindings->value;
    case 3U: return bindings->attentionOutput;
    case 4U: return bindings->feedForwardUp;
    case 5U: return bindings->feedForwardDown;
    default: return NULL;
    }
}

static const UmiAiTrainingTensor *CanonicalWeightAt(
    const UmiAiTrainingTransformerBlockParameters *parameters,
    size_t index)
{
    if (parameters == NULL) return NULL;
    switch (index) {
    case 0U: return parameters->queryWeights;
    case 1U: return parameters->keyWeights;
    case 2U: return parameters->valueWeights;
    case 3U: return parameters->outputWeights;
    case 4U: return parameters->upWeights;
    case 5U: return parameters->downWeights;
    default: return NULL;
    }
}

/* LowRankCreate copies a frozen base. Verify against current canonical values:
 * merely matching dimensions is insufficient when the original weights have
 * changed or an adapter was accidentally paired with another checkpoint.
 * This does not authenticate a publisher or approve an external model. */
static UmiStatus VerifyFrozenBase(
    const UmiAiTrainingLowRankProjection *projection,
    const UmiAiTrainingTensor *canonical)
{
    size_t rows = 0U, columns = 0U;
    size_t baseRows = 0U, baseColumns = 0U;
    size_t downRows = 0U, downColumns = 0U;
    size_t upRows = 0U, upColumns = 0U;
    float *original = NULL, *copied = NULL;
    UmiStatus status;

    if (projection == NULL || canonical == NULL ||
        projection->baseWeights == NULL ||
        projection->adapterDown == NULL || projection->adapterUp == NULL ||
        projection->rank == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(canonical, &rows, &columns);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorShape(projection->baseWeights, &baseRows, &baseColumns);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorShape(projection->adapterDown, &downRows, &downColumns);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingTensorShape(projection->adapterUp, &upRows, &upColumns);
    if (status != UMI_STATUS_OK) return status;
    if (rows == 0U || columns == 0U ||
        rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns ||
        rows != projection->inputChannels ||
        columns != projection->outputChannels ||
        baseRows != rows || baseColumns != columns ||
        downRows != rows || downColumns != projection->rank ||
        upRows != projection->rank || upColumns != columns)
        return UMI_STATUS_INVALID_STATE;

    const size_t count = rows * columns;
    original = malloc(count * sizeof(*original));
    copied = malloc(count * sizeof(*copied));
    if (original == NULL || copied == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }
    status = UmiAiTrainingTensorValues(canonical, original, count);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingTensorValues(projection->baseWeights, copied, count);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (memcmp(original, copied, count * sizeof(*original)) != 0)
        status = UMI_STATUS_INVALID_STATE;
cleanup:
    free(copied);
    free(original);
    return status;
}

static UmiStatus ForwardProjection(
    void *context, UmiAiTrainingTape *tape,
    UmiAiTrainingTransformerProjection projection,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTensor *canonicalWeights,
    UmiAiTrainingTensor **outTensor)
{
    const UmiAiTrainingTransformerLowRankBindings *bindings = context;
    if ((unsigned)projection < (unsigned)UMI_AI_TRANSFORMER_PROJECTION_QUERY ||
        (unsigned)projection > (unsigned)UMI_AI_TRANSFORMER_PROJECTION_FEED_FORWARD_DOWN)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = (size_t)((unsigned)projection -
                            (unsigned)UMI_AI_TRANSFORMER_PROJECTION_QUERY);
    const UmiAiTrainingLowRankProjection *selected = BindingAt(bindings, index);
    if (selected == NULL)
        return UmiAiTrainingMatmul(tape, input, canonicalWeights, outTensor);
    return UmiAiTrainingLowRankForward(tape, input, selected, outTensor);
}

UmiStatus UmiAiTrainingTransformerLowRankTrainables(
    const UmiAiTrainingTransformerLowRankBindings *bindings,
    UmiAiTrainingTensor **outTrainables, size_t capacity, size_t *outCount)
{
    UmiAiTrainingTensor *candidate[UMI_AI_TRANSFORMER_LOW_RANK_MAX_TRAINABLES] = {0};
    size_t count = 0U;
    if (bindings == NULL || outTrainables == NULL || outCount == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < PROJECTION_COUNT; ++index) {
        const UmiAiTrainingLowRankProjection *selected = BindingAt(bindings, index);
        if (selected == NULL) continue;
        UmiAiTrainingTensor *down = NULL, *up = NULL;
        UmiStatus status = UmiAiTrainingLowRankTrainables(selected, &down, &up);
        if (status != UMI_STATUS_OK) return status;
        if (down == NULL || up == NULL || down == up)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t earlier = 0U; earlier < count; ++earlier)
            if (candidate[earlier] == down || candidate[earlier] == up)
                return UMI_STATUS_INVALID_ARGUMENT;
        candidate[count++] = down;
        candidate[count++] = up;
    }
    if (count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < count) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(outTrainables, candidate, count * sizeof(*candidate));
    *outCount = count;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingTransformerBlockForwardLowRank(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTransformerBlockParameters *parameters,
    const UmiAiTrainingTransformerBlockConfig *config,
    const UmiAiTrainingTransformerLowRankBindings *bindings,
    UmiAiTrainingTensor **outTensor)
{
    UmiAiTrainingTensor *trainables[UMI_AI_TRANSFORMER_LOW_RANK_MAX_TRAINABLES];
    size_t trainableCount = 0U;
    UmiStatus status;
    if (bindings == NULL)
        return UmiAiTrainingTransformerBlockForward(
            tape, input, parameters, config, outTensor);
    if (tape == NULL || input == NULL || parameters == NULL ||
        config == NULL || outTensor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* Reject repeated adapter leaves before graph construction. The existing
     * multi-parameter optimiser similarly requires distinct trainable handles. */
    status = UmiAiTrainingTransformerLowRankTrainables(
        bindings, trainables, UMI_AI_TRANSFORMER_LOW_RANK_MAX_TRAINABLES,
        &trainableCount);
    if (status != UMI_STATUS_OK) return status;
    (void)trainableCount;

    for (size_t index = 0U; index < PROJECTION_COUNT; ++index) {
        const UmiAiTrainingLowRankProjection *selected = BindingAt(bindings, index);
        if (selected == NULL) continue;
        status = VerifyFrozenBase(selected, CanonicalWeightAt(parameters, index));
        if (status != UMI_STATUS_OK) return status;
    }
    return UmiAiTrainingTransformerBlockForwardWithProjections(
        tape, input, parameters, config, ForwardProjection, (void *)bindings,
        outTensor);
}
