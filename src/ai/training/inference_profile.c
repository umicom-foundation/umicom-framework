/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/inference_profile.c
 *
 * PURPOSE:
 *   Benchmark canonical decoder forward passes and the qualified key/value
 *   cache without duplicating a single attention, projection or sampling
 *   algorithm. Record workload units and numerical parity independently of
 *   process CPU time.
 *
 * OWNERSHIP:
 *   Model and token stream are borrowed. A fresh Framework cache is owned
 *   only during this call. Every result is committed after all comparisons
 *   pass, and the model's trained parameters are never modified.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/inference_profile.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/kv_cache.h"
#include "umicom/ai/training/tape.h"
#include "decoder_private.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

static bool MultiplyFits(size_t left, size_t right)
{
    return right == 0U || left <= SIZE_MAX / right;
}

UmiStatus UmiAiTrainingInferenceEstimate(
    const UmiAiTrainingDecoderConfig *config,
    UmiAiTrainingInferenceFootprint *outFootprint)
{
    UmiAiTrainingInferenceFootprint candidate = {0};
    UmiStatus status;
    if (config == NULL || outFootprint == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiDecoderValidateConfig(config);
    if (status != UMI_STATUS_OK) return status;

    /* The existing decoder owns the parameter order and shapes. Never
     * replicate that schema in the profiler or the LLM application. */
    for (size_t id = 0U; id < (size_t)UMI_DECODER_PARAMETER_COUNT; ++id) {
        size_t rows = 0U;
        size_t columns = 0U;
        UmiAiDecoderParameterShape(config, id, &rows, &columns);
        if (rows == 0U || columns == 0U ||
            !MultiplyFits(rows, columns) ||
            rows * columns > UMI_AI_TRAINING_MAX_ELEMENTS ||
            candidate.parameterElements > SIZE_MAX - rows * columns)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        candidate.parameterElements += rows * columns;
    }
    candidate.parameterTensors = (size_t)UMI_DECODER_PARAMETER_COUNT;
    if (!MultiplyFits(candidate.parameterElements, sizeof(float)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.logicalParameterBytes =
        candidate.parameterElements * sizeof(float);
    if (!MultiplyFits((size_t)config->contextLength,
                      (size_t)config->channels) ||
        !MultiplyFits((size_t)config->contextLength,
                      (size_t)config->vocabularySize))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t keyValueElements = (size_t)config->contextLength *
                              (size_t)config->channels;
    size_t logitsElements = (size_t)config->contextLength *
                            (size_t)config->vocabularySize;
    if (!MultiplyFits(keyValueElements, 2U * sizeof(float)) ||
        !MultiplyFits(logitsElements, sizeof(float)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.keyValuePayloadBytes = keyValueElements * 2U * sizeof(float);
    candidate.fullContextLogitsBytes = logitsElements * sizeof(float);
    /* Report the shape-dependent allocation, not the compatibility bound.
     * This is the same layout checked by cache creation before it allocates. */
#if 0
    status = UmiAiTrainingKvCacheAllocationSize(&candidate.cacheAllocationBytes);
#endif
    status = UmiAiTrainingKvCacheAllocationForConfig(
        config, &candidate.cacheAllocationBytes);
    if (status != UMI_STATUS_OK) return status;
    *outFootprint = candidate;
    return UMI_STATUS_OK;
}

static void AccumulateClock(clock_t start, clock_t end, double *outSeconds,
                            int *outAvailable)
{
    if (start == (clock_t)-1 || end == (clock_t)-1 || end < start ||
        CLOCKS_PER_SEC <= 0) {
        *outAvailable = 0;
        return;
    }
    *outSeconds += (double)(end - start) / (double)CLOCKS_PER_SEC;
}

/* Recompute complete decoder context as a deliberate baseline. Trailing BOS
 * tokens are masked from earlier rows. This lets short-prefix results be
 * compared at the actual prefix position, without falsely left-padding and
 * changing learned absolute position indices. */
static UmiStatus CompareReference(UmiAiTrainingDecoder *decoder,
                                 const size_t *stream, size_t consumed,
                                 size_t contextLength, size_t vocabularySize,
                                 const float *cachedLogits,
                                 double *maximumDeviation,
                                 double *cpuSeconds, int *clockAvailable)
{
    size_t context[UMI_AI_DECODER_MAX_CONTEXT];
    float logits[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
    const size_t retained = consumed < contextLength ? consumed : contextLength;
    const size_t startIndex = consumed - retained;
    UmiStatus status;
    clock_t begin;
    clock_t finish;

    for (size_t index = 0U; index < contextLength; ++index) {
        context[index] = index < retained ?
            stream[startIndex + index] : (size_t)UMI_AI_BYTE_BOS;
    }
    begin = clock();
    status = UmiAiTrainingDecoderLogits(decoder, context, contextLength,
                                        logits, contextLength * vocabularySize);
    finish = clock();
    AccumulateClock(begin, finish, cpuSeconds, clockAvailable);
    if (status != UMI_STATUS_OK) return status;
    const size_t row = (retained - 1U) * vocabularySize;
    for (size_t token = 0U; token < vocabularySize; ++token) {
        if (!isfinite(logits[row + token]) || !isfinite(cachedLogits[token]))
            return UMI_STATUS_INVALID_STATE;
        const double error = fabs((double)logits[row + token] -
                                  (double)cachedLogits[token]);
        if (error > *maximumDeviation) *maximumDeviation = error;
        if (error > 0.00005) return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingInferenceProfileStream(
    UmiAiTrainingDecoder *decoder,
    const size_t *streamIds, size_t streamTokens,
    size_t prefillTokens,
    UmiAiTrainingInferenceProfile *outProfile)
{
    UmiAiTrainingDecoderConfig config;
    UmiAiTrainingInferenceProfile candidate = {0};
    UmiAiTrainingKvCache *cache = NULL;
    uint64_t steps = 0U;
    float cachedLogits[UMI_AI_DECODER_MAX_VOCABULARY];
    UmiStatus status;
    clock_t begin;
    clock_t finish;
    size_t retained = 0U;

    if (decoder == NULL || streamIds == NULL || outProfile == NULL ||
        streamTokens == 0U ||
        streamTokens > UMI_AI_INFERENCE_PROFILE_MAX_TOKENS ||
        prefillTokens == 0U || prefillTokens > streamTokens)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(decoder, &config, &steps);
    if (status != UMI_STATUS_OK) return status;
    if (prefillTokens > (size_t)config.contextLength)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t index = 0U; index < streamTokens; ++index) {
        if (streamIds[index] >= (size_t)config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = UmiAiTrainingInferenceEstimate(&config, &candidate.footprint);
    if (status != UMI_STATUS_OK) return status;
    candidate.streamTokens = streamTokens;
    candidate.prefillTokens = prefillTokens;
    candidate.cpuClockAvailable = 1;
    (void)steps;
    status = UmiAiTrainingKvCacheCreate(decoder, &cache);
    if (status != UMI_STATUS_OK) return status;

    begin = clock();
    status = UmiAiTrainingKvCachePrefill(cache, streamIds, prefillTokens,
                                         cachedLogits, (size_t)config.vocabularySize);
    finish = clock();
    AccumulateClock(begin, finish, &candidate.cachedProcessCpuSeconds,
                    &candidate.cpuClockAvailable);
    if (status != UMI_STATUS_OK) goto release;
    status = CompareReference(decoder, streamIds, prefillTokens,
                              (size_t)config.contextLength,
                              (size_t)config.vocabularySize, cachedLogits,
                              &candidate.maximumLogitDeviation,
                              &candidate.referenceProcessCpuSeconds,
                              &candidate.cpuClockAvailable);
    if (status != UMI_STATUS_OK) goto release;
    ++candidate.referenceQueries;

    for (size_t index = prefillTokens; index < streamTokens; ++index) {
        begin = clock();
        status = UmiAiTrainingKvCacheAppend(cache, streamIds[index],
                   cachedLogits, (size_t)config.vocabularySize);
        finish = clock();
        AccumulateClock(begin, finish, &candidate.cachedProcessCpuSeconds,
                        &candidate.cpuClockAvailable);
        if (status != UMI_STATUS_OK) goto release;
        status = CompareReference(decoder, streamIds, index + 1U,
                                  (size_t)config.contextLength,
                                  (size_t)config.vocabularySize, cachedLogits,
                                  &candidate.maximumLogitDeviation,
                                  &candidate.referenceProcessCpuSeconds,
                                  &candidate.cpuClockAvailable);
        if (status != UMI_STATUS_OK) goto release;
        ++candidate.referenceQueries;
    }
    status = UmiAiTrainingKvCacheInspect(cache, &retained,
                                         &candidate.cacheComputedRows,
                                         &candidate.cacheWindowRebuilds);
    if (status != UMI_STATUS_OK) goto release;
    candidate.retainedTokens = retained;
    candidate.referenceComputedRows = (uint64_t)candidate.referenceQueries *
                                      (uint64_t)config.contextLength;
    *outProfile = candidate;
release:
    UmiAiTrainingKvCacheDestroy(cache);
    return status;
}
