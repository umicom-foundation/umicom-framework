/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/capacity_plan.c
 *
 * PURPOSE:
 *   Predict resource feasibility and directly attributable float32 payloads
 *   for the existing educational decoder, without duplicating model shapes.
 *
 * VALIDATION:
 *   The existing decoder validates semantic configuration. Its private
 *   parameter-shape schema remains authoritative. The same tape limits that
 *   protect activation matrices and causal probabilities are checked here.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/tape.h"
#include "decoder_private.h"

#include <stdint.h>
#include <stddef.h>

static size_t Smaller(size_t a, size_t b)
{
    return a < b ? a : b;
}

UmiStatus UmiAiTrainingCapacityPlanCompute(
    const UmiAiTrainingDecoderConfig *config,
    size_t batchSequences,
    UmiAiTrainingCapacityPlan *outPlan)
{
    UmiAiTrainingCapacityPlan plan = {0};
    size_t biggestWidth;
    size_t attentionFactor;
    size_t maximumBatchTokens;
    size_t context;
    size_t heads;
    size_t vocabulary;
    size_t channels;
    size_t hidden;

    if (outPlan == NULL || batchSequences == 0U ||
        UmiAiDecoderValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;

    context = (size_t)config->contextLength;
    heads = (size_t)config->attentionHeads;
    vocabulary = (size_t)config->vocabularySize;
    channels = (size_t)config->channels;
    hidden = (size_t)config->hiddenChannels;

    /* Each persistent parameter is the same shape that the decoder creates.
     * The tensor maximum is checked BEFORE any multiplication in the planner. */
    for (size_t id = 0U; id < (size_t)UMI_DECODER_PARAMETER_COUNT; ++id) {
        size_t rows = 0U;
        size_t columns = 0U;
        UmiAiDecoderParameterShape(config, id, &rows, &columns);
        if (rows == 0U || columns == 0U ||
            columns > UMI_AI_TRAINING_MAX_ELEMENTS ||
            rows > UMI_AI_TRAINING_MAX_ELEMENTS / columns)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        plan.parameterElements += rows * columns;
    }
    plan.parameterTensors = (size_t)UMI_DECODER_PARAMETER_COUNT;

    /* The decoder evaluates all rows, so its limiting width is the largest
     * of its output projection, expansion layer and residual channels.
     * Attention saves one [heads, tokens, context] probability allocation. */
    biggestWidth = vocabulary;
    if (hidden > biggestWidth) biggestWidth = hidden;
    if (channels > biggestWidth) biggestWidth = channels;
    if (heads > UMI_AI_TRAINING_MAX_ELEMENTS / context)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    attentionFactor = heads * context;
    maximumBatchTokens = Smaller(
        UMI_AI_TRAINING_MAX_ELEMENTS / biggestWidth,
        UMI_AI_TRAINING_MAX_ELEMENTS / attentionFactor);
    plan.maximumBatchSequences = maximumBatchTokens / context;
    if (plan.maximumBatchSequences == 0U ||
        batchSequences > plan.maximumBatchSequences)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    plan.tokensPerBatch = batchSequences * context;
    plan.logitsElements = plan.tokensPerBatch * vocabulary;
    plan.attentionProbabilityElements = plan.tokensPerBatch * attentionFactor;
    plan.parameterValueBytes = plan.parameterElements * sizeof(float);
    plan.parameterGradientBytes = plan.parameterElements * sizeof(float);
    plan.logitsBytes = plan.logitsElements * sizeof(float);
    plan.attentionProbabilityBytes = plan.attentionProbabilityElements * sizeof(float);
    plan.minimumRetainedPayloadBytes =
        plan.parameterValueBytes + plan.parameterGradientBytes +
        plan.logitsBytes + plan.attentionProbabilityBytes;
    plan.maximumTensorElements = UMI_AI_TRAINING_MAX_ELEMENTS;
    *outPlan = plan;
    return UMI_STATUS_OK;
}
