/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/transformer.c
 *
 * PURPOSE:
 *   Compose a learnable pre-LN decoder Transformer block in native C23.
 *
 * ARCHITECTURE:
 *   Reuse Framework tape arithmetic and derivatives; no duplicate tensor
 *   engine, attention algorithm, optimiser or direct GUI dependency.
 *   Parameter shapes are checked before any new graph node is created.
 *   A tape checkpoint makes intermediate publication failure-atomic.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/transformer.h"

#include <math.h>

/* Do not dereference an opaque model object outside its public shape contract. */
static int ShapeMatches(const UmiAiTrainingTensor *tensor,
                        size_t rows, size_t columns)
{
    size_t actualRows = 0U, actualColumns = 0U;
    return UmiAiTrainingTensorShape(tensor, &actualRows, &actualColumns) == UMI_STATUS_OK &&
           actualRows == rows && actualColumns == columns;
}

UmiStatus UmiAiTrainingTransformerBlockForward(
    UmiAiTrainingTape *tape,
    const UmiAiTrainingTensor *input,
    const UmiAiTrainingTransformerBlockParameters *parameters,
    const UmiAiTrainingTransformerBlockConfig *config,
    UmiAiTrainingTensor **outTensor)
{
    size_t rows = 0U, channels = 0U;
    size_t hiddenRows = 0U, hiddenChannels = 0U;
    size_t checkpoint = 0U;
    UmiStatus status;
    UmiAiTrainingTensor *norm1 = NULL, *query = NULL, *key = NULL;
    UmiAiTrainingTensor *value = NULL, *attended = NULL, *projected = NULL;
    UmiAiTrainingTensor *residual1 = NULL, *norm2 = NULL;
    UmiAiTrainingTensor *expanded = NULL, *expandedBias = NULL;
    UmiAiTrainingTensor *activated = NULL, *contracted = NULL;
    UmiAiTrainingTensor *contractedBias = NULL, *residual2 = NULL;

    if (tape == NULL || input == NULL || parameters == NULL || config == NULL ||
        outTensor == NULL || config->sequenceLength == 0U ||
        config->attentionHeads == 0U || !isfinite(config->normEpsilon) ||
        config->normEpsilon <= 0.0F) return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingTensorShape(input, &rows, &channels);
    if (status != UMI_STATUS_OK || rows == 0U || channels == 0U ||
        rows % config->sequenceLength != 0U ||
        channels % config->attentionHeads != 0U) return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingTensorShape(parameters->upWeights, &hiddenRows,
                                      &hiddenChannels);
    if (status != UMI_STATUS_OK || hiddenRows != channels || hiddenChannels == 0U ||
        !ShapeMatches(parameters->normScale1, 1U, channels) ||
        !ShapeMatches(parameters->normBias1, 1U, channels) ||
        !ShapeMatches(parameters->queryWeights, channels, channels) ||
        !ShapeMatches(parameters->keyWeights, channels, channels) ||
        !ShapeMatches(parameters->valueWeights, channels, channels) ||
        !ShapeMatches(parameters->outputWeights, channels, channels) ||
        !ShapeMatches(parameters->normScale2, 1U, channels) ||
        !ShapeMatches(parameters->normBias2, 1U, channels) ||
        !ShapeMatches(parameters->upBias, 1U, hiddenChannels) ||
        !ShapeMatches(parameters->downWeights, hiddenChannels, channels) ||
        !ShapeMatches(parameters->downBias, 1U, channels)) return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingTapeCheckpoint(tape, &checkpoint);
    if (status != UMI_STATUS_OK) return status;

#define UMI_TRANSFORMER_STEP(call) do { \
    status = (call); \
    if (status != UMI_STATUS_OK) goto rollback; \
} while (0)
    /* Pre-LN attention sublayer. Distinct Q/K/V projections share one normalised
     * input; tape backprop correctly accumulates those three gradient paths. */
    UMI_TRANSFORMER_STEP(UmiAiTrainingLayerNorm(tape, input,
        parameters->normScale1, parameters->normBias1, config->normEpsilon, &norm1));
    UMI_TRANSFORMER_STEP(UmiAiTrainingMatmul(tape, norm1, parameters->queryWeights, &query));
    UMI_TRANSFORMER_STEP(UmiAiTrainingMatmul(tape, norm1, parameters->keyWeights, &key));
    UMI_TRANSFORMER_STEP(UmiAiTrainingMatmul(tape, norm1, parameters->valueWeights, &value));
    UMI_TRANSFORMER_STEP(UmiAiTrainingMultiHeadCausalAttention(tape, query, key, value,
        config->sequenceLength, config->attentionHeads, &attended));
    UMI_TRANSFORMER_STEP(UmiAiTrainingMatmul(tape, attended,
        parameters->outputWeights, &projected));
    UMI_TRANSFORMER_STEP(UmiAiTrainingAdd(tape, input, projected, &residual1));

    /* Pre-LN feed-forward sublayer with a documented ReLU (not GELU) and a
     * learned broadcast bias on each dense projection. */
    UMI_TRANSFORMER_STEP(UmiAiTrainingLayerNorm(tape, residual1,
        parameters->normScale2, parameters->normBias2, config->normEpsilon, &norm2));
    UMI_TRANSFORMER_STEP(UmiAiTrainingMatmul(tape, norm2, parameters->upWeights, &expanded));
    UMI_TRANSFORMER_STEP(UmiAiTrainingAdd(tape, expanded, parameters->upBias, &expandedBias));
    UMI_TRANSFORMER_STEP(UmiAiTrainingRelu(tape, expandedBias, &activated));
    UMI_TRANSFORMER_STEP(UmiAiTrainingMatmul(tape, activated,
        parameters->downWeights, &contracted));
    UMI_TRANSFORMER_STEP(UmiAiTrainingAdd(tape, contracted,
        parameters->downBias, &contractedBias));
    UMI_TRANSFORMER_STEP(UmiAiTrainingAdd(tape, residual1, contractedBias, &residual2));
#undef UMI_TRANSFORMER_STEP
    *outTensor = residual2;
    return UMI_STATUS_OK;

rollback:
    /* The original prefix is retained; caller-owned inputs and outTensor are
     * untouched. Rollback only frees nodes created by this composition. */
    (void)UmiAiTrainingTapeRollback(tape, checkpoint);
    return status;
}
