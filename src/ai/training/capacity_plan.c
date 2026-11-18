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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/capacity_plan.h"
#include "umicom/ai/training/tape.h"
#include "umicom/ai/training/gradient_accumulation.h"
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

/* Planning uses the same authoritative parameter schema as allocation. Keeping
 * graph payload and reducer payload separate avoids implying that a tape limit
 * also bounds optimizer or host reduction memory. */
UmiStatus UmiAiTrainingAccumulationPlanCompute(
    const UmiAiTrainingDecoderConfig *config,
    size_t effectiveSequences, size_t microbatchSequences,
    UmiAiTrainingAccumulationPlan *outPlan)
{
    UmiAiTrainingAccumulationPlan plan = {0};
    size_t context, maximumSequences, graphSequences;
    UmiStatus status;
    if (outPlan == NULL || effectiveSequences == 0U || microbatchSequences == 0U ||
        UmiAiDecoderValidateConfig(config) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    context = (size_t)config->contextLength;
    maximumSequences = UMI_AI_TRAINING_MAX_ACCUMULATED_TOKENS / context;
    if (effectiveSequences > maximumSequences || microbatchSequences > maximumSequences)
        return UMI_STATUS_INVALID_ARGUMENT;
    graphSequences = Smaller(effectiveSequences, microbatchSequences);
    status = UmiAiTrainingCapacityPlanCompute(config, graphSequences, &plan.graph);
    if (status != UMI_STATUS_OK) return status;
    for (size_t id = 0U; id < (size_t)UMI_DECODER_PARAMETER_COUNT; ++id) {
        size_t rows = 0U, columns = 0U;
        UmiAiDecoderParameterShape(config, id, &rows, &columns);
        /* CapacityPlanCompute has already checked these exact dimensions. */
        size_t elements = rows * columns;
        if (elements > plan.largestParameterElements)
            plan.largestParameterElements = elements;
    }
    if (plan.graph.parameterElements > SIZE_MAX / sizeof(double) ||
        plan.graph.parameterElements > SIZE_MAX / sizeof(float) ||
        plan.largestParameterElements > SIZE_MAX / sizeof(float))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    plan.accumulatorBytes = plan.graph.parameterElements * sizeof(double);
    plan.reducedGradientBytes = plan.graph.parameterElements * sizeof(float);
    plan.scratchBytes = plan.largestParameterElements * sizeof(float);
    if (plan.accumulatorBytes > SIZE_MAX - plan.reducedGradientBytes ||
        plan.accumulatorBytes + plan.reducedGradientBytes > SIZE_MAX - plan.scratchBytes)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    plan.peakReductionPayloadBytes =
        plan.accumulatorBytes + plan.reducedGradientBytes + plan.scratchBytes;
    plan.effectiveSequences = effectiveSequences;
    plan.effectiveTokens = effectiveSequences * context;
    plan.requestedMicrobatchSequences = microbatchSequences;
    /* Quotient and remainder avoid an overflow-prone ceil(n / d) addition. */
    plan.maximumGraphs = effectiveSequences / microbatchSequences +
                         (effectiveSequences % microbatchSequences != 0U);
    plan.lastGraphSequences = effectiveSequences % microbatchSequences;
    if (plan.lastGraphSequences == 0U) plan.lastGraphSequences = microbatchSequences;
    *outPlan = plan;
    return UMI_STATUS_OK;
}
