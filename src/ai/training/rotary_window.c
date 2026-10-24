/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/rotary_window.c
 * PURPOSE:
 *   Bounded, transactional ring cache for fixed single-block attention Q/K/V.
 *   Uses the Framework-owned experimental rotary primitive; older key/value
 *   rows retain original absolute token positions across context eviction.
 *   Existing learned-absolute decoder and checkpoint paths are untouched.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/rotary_window.h"

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiTrainingRotaryWindow {
    UmiAiTrainingRotaryWindowConfig config;
    uint64_t nextPosition;
    uint64_t computedRows;
    uint64_t evictedRows;
    size_t oldest;
    size_t count;
    float rotatedKeys[UMI_AI_ROTARY_MAX_CONTEXT][UMI_AI_ROTARY_MAX_CHANNELS];
    float values[UMI_AI_ROTARY_MAX_CONTEXT][UMI_AI_ROTARY_MAX_CHANNELS];
};

static UmiStatus ValidateWindowConfig(
    const UmiAiTrainingRotaryWindowConfig *config)
{
    if (config == NULL || config->contextCapacity == 0U ||
        config->contextCapacity > UMI_AI_ROTARY_MAX_CONTEXT)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UmiAiTrainingRotaryConfigValidate(&config->rotary);
}

UmiStatus UmiAiTrainingRotaryWindowCreate(
    const UmiAiTrainingRotaryWindowConfig *config,
    UmiAiTrainingRotaryWindow **outWindow)
{
    UmiAiTrainingRotaryWindow *candidate;
    UmiStatus status;
    if (outWindow == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = ValidateWindowConfig(config);
    if (status != UMI_STATUS_OK) return status;
    candidate = calloc(1U, sizeof(*candidate));
    if (candidate == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    candidate->config = *config;
    *outWindow = candidate;
    return UMI_STATUS_OK;
}

void UmiAiTrainingRotaryWindowDestroy(UmiAiTrainingRotaryWindow *window)
{
    free(window);
}

UmiStatus UmiAiTrainingRotaryWindowAppend(
    UmiAiTrainingRotaryWindow *window,
    const float *query, const float *key, const float *value,
    size_t channelCount, float *outContext, size_t capacity)
{
    UmiAiTrainingRotaryWindow proposed;
    float queryRotated[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float keyRotated[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    float context[UMI_AI_ROTARY_MAX_CHANNELS] = {0};
    double scores[UMI_AI_ROTARY_MAX_CONTEXT] = {0};
    double maximum = -INFINITY, denominator = 0.0;
    size_t index;
    UmiStatus status;
    if (window == NULL || query == NULL || key == NULL ||
        value == NULL || outContext == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (channelCount != window->config.rotary.headChannels)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (capacity < channelCount) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (window->nextPosition > UMI_AI_ROTARY_MAX_POSITION)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (window->computedRows == UINT64_MAX || window->evictedRows == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < channelCount; ++i)
        if (!isfinite(query[i]) || !isfinite(key[i]) || !isfinite(value[i]))
            return UMI_STATUS_INVALID_ARGUMENT;

    /* The shared pure RoPE primitive owns all angular mathematics.
     * Rotating once on insertion is essential: old K rows keep their true
     * absolute coordinates even if the oldest row is later evicted. */
    status = UmiAiTrainingRotaryRotate(&window->config.rotary, query,
        channelCount, window->nextPosition, queryRotated, channelCount);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingRotaryRotate(&window->config.rotary, key,
        channelCount, window->nextPosition, keyRotated, channelCount);
    if (status != UMI_STATUS_OK) return status;

    /* All changes remain private until every score, probability and output
     * component is finite. This includes ring-index and eviction updates. */
    proposed = *window;
    if (proposed.count == proposed.config.contextCapacity) {
        proposed.oldest = (proposed.oldest + 1U) % proposed.config.contextCapacity;
        --proposed.count;
        ++proposed.evictedRows;
    }
    index = (proposed.oldest + proposed.count) % proposed.config.contextCapacity;
    memcpy(proposed.rotatedKeys[index], keyRotated,
           channelCount * sizeof(float));
    memcpy(proposed.values[index], value,
           channelCount * sizeof(float));
    ++proposed.count;

    const double scale = sqrt((double)channelCount);
    for (size_t row = 0U; row < proposed.count; ++row) {
        const size_t slot = (proposed.oldest + row) % proposed.config.contextCapacity;
        double dot = 0.0;
        for (size_t ch = 0U; ch < channelCount; ++ch)
            dot += (double)queryRotated[ch] * (double)proposed.rotatedKeys[slot][ch];
        scores[row] = dot / scale;
        if (!isfinite(scores[row])) return UMI_STATUS_INVALID_STATE;
        if (scores[row] > maximum) maximum = scores[row];
    }
    for (size_t row = 0U; row < proposed.count; ++row)
        denominator += exp(scores[row] - maximum);
    if (!isfinite(denominator) || denominator <= 0.0)
        return UMI_STATUS_INVALID_STATE;
    for (size_t ch = 0U; ch < channelCount; ++ch) {
        double attended = 0.0;
        for (size_t row = 0U; row < proposed.count; ++row) {
            const size_t slot = (proposed.oldest + row) % proposed.config.contextCapacity;
            const double probability = exp(scores[row] - maximum) / denominator;
            attended += probability * (double)proposed.values[slot][ch];
        }
        if (!isfinite(attended) ||
            attended < -(double)FLT_MAX || attended > (double)FLT_MAX)
            return UMI_STATUS_INVALID_STATE;
        context[ch] = (float)attended;
    }
    ++proposed.computedRows;
    ++proposed.nextPosition;
    memcpy(outContext, context, channelCount * sizeof(float));
    *window = proposed;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRotaryWindowInspect(
    const UmiAiTrainingRotaryWindow *window,
    UmiAiTrainingRotaryWindowStats *outStats)
{
    UmiAiTrainingRotaryWindowStats proposal = {0};
    if (window == NULL || outStats == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    proposal.computedRows = window->computedRows;
    proposal.evictedRows = window->evictedRows;
    proposal.fullWindowRebuilds = 0U;
    proposal.nextAbsolutePosition = window->nextPosition;
    proposal.retainedRows = window->count;
    proposal.contextCapacity = window->config.contextCapacity;
    *outStats = proposal;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingRotaryWindowReset(UmiAiTrainingRotaryWindow *window)
{
    UmiAiTrainingRotaryWindowConfig config;
    if (window == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    config = window->config;
    /* Discard earlier Q/K/V and restart absolute coordinates at position 0;
     * doing so is an explicit new sequence, not an in-stream context rebase. */
    memset(window, 0, sizeof(*window));
    window->config = config;
    return UMI_STATUS_OK;
}
