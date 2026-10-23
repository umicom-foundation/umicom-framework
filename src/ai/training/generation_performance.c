/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/generation_performance.c
 *
 * PURPOSE:
 *   Quantify the work and observed CPU time of native generation using
 *   existing canonical and KV-cache APIs, without a second inference engine.
 *
 * ARCHITECTURE:
 *   The Framework's decoder, cache allocation planner, nucleus sampler and
 *   generation session are the only owners of their respective behaviour.
 *   Entire result is staged until all parity and state checks have passed.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/generation_performance.h"
#include "umicom/ai/training/kv_cache.h"
#include "umicom/ai/training/byte_tokenizer.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

/* In the qualified learned-absolute-position decoder, prefill covers the
 * entire context. Each subsequently generated token displaces the oldest
 * position and requires a full window rebuild. This is a predictive count,
 * not a benchmark or a hypothetical future relative-position cache model. */
UmiStatus UmiAiTrainingGenerationWorkEstimateBuild(
    const UmiAiTrainingDecoderConfig *config,
    size_t promptTokenCount, size_t generatedTokenCount,
    UmiAiTrainingGenerationWorkEstimate *outEstimate)
{
    UmiAiTrainingGenerationWorkEstimate candidate = {0};
    size_t bytes = 0U;
    UmiStatus status;

    if (config == NULL || outEstimate == NULL ||
        promptTokenCount > UMI_AI_GENERATION_MAX_PROMPT_TOKENS ||
        generatedTokenCount == 0U ||
        generatedTokenCount > UMI_AI_GENERATION_MAX_NEW_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* This canonical Framework service both checks the decoder dimensions
     * and calculates its REAL allocation layout, with overflow protection. */
    status = UmiAiTrainingKvCacheAllocationForConfig(config, &bytes);
    if (status != UMI_STATUS_OK) return status;
    if (config->contextLength == 0U ||
        config->contextLength > UMI_AI_DECODER_MAX_CONTEXT ||
        config->vocabularySize <= UMI_AI_BYTE_OFFSET ||
        config->vocabularySize > UMI_AI_DECODER_MAX_VOCABULARY)
        return UMI_STATUS_INVALID_ARGUMENT;

    candidate.promptTokens = promptTokenCount;
    candidate.generatedTokens = generatedTokenCount;
    candidate.contextTokens = (size_t)config->contextLength;
    candidate.cacheHeapBytes = bytes;
    candidate.referenceComputedRows =
        (uint64_t)config->contextLength * (uint64_t)generatedTokenCount;
    candidate.cachedComputedRows = candidate.referenceComputedRows;
    candidate.cachedWindowRebuilds = (uint64_t)(generatedTokenCount - 1U);
    *outEstimate = candidate;
    return UMI_STATUS_OK;
}

/* clock() measures process CPU time and may have insufficient resolution for
 * this small teaching model. Never turn absent/invalid readings into a speedup
 * claim; retain a separately reported clockAvailable indicator. */
static bool CpuSeconds(clock_t start, clock_t stop, double *outSeconds)
{
    if (start == (clock_t)-1 || stop == (clock_t)-1 || stop < start ||
        CLOCKS_PER_SEC <= 0)
        return false;
    double seconds = (double)(stop - start) / (double)CLOCKS_PER_SEC;
    if (!isfinite(seconds) || seconds < 0.0) return false;
    *outSeconds = seconds;
    return true;
}

UmiStatus UmiAiTrainingGenerationPerformanceCompare(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    UmiAiTrainingGenerationPerformance *outComparison)
{
    UmiAiTrainingGenerationPerformance candidate = {0};
    UmiAiTrainingDecoderConfig before = {0};
    UmiAiTrainingDecoderConfig after = {0};
    UmiAiTrainingGenerationSession *session = NULL;
    UmiAiTrainingGenerationCacheStats observed = {0};
    uint64_t priorSteps = 0U, laterSteps = 0U;
    size_t reference[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t cached[UMI_AI_GENERATION_MAX_NEW_TOKENS];
    size_t generated = 0U, inspected = 0U;
    clock_t startReference, endReference, startCache, endCache;
    UmiStatus status;

    if (model == NULL || policy == NULL || outComparison == NULL ||
        (promptTokenCount > 0U && promptTokenIds == NULL))
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingDecoderInspect(model, &before, &priorSteps);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingGenerationWorkEstimateBuild(&before,
        promptTokenCount, generatedTokenCount, &candidate.estimate);
    if (status != UMI_STATUS_OK) return status;
    if (policy->topK == 0U ||
        policy->topK > (size_t)before.vocabularySize - UMI_AI_BYTE_OFFSET ||
        !isfinite(policy->temperature) || policy->temperature <= 0.0F ||
        !isfinite(policy->topP) || policy->topP <= 0.0F ||
        policy->topP > 1.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < promptTokenCount; ++i)
        if (promptTokenIds[i] >= (size_t)before.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;

    startReference = clock();
    status = UmiAiTrainingDecoderGenerateNucleus(model,
        promptTokenIds, promptTokenCount, policy, generatedTokenCount,
        seed, reference, generatedTokenCount, &generated);
    endReference = clock();
    if (status != UMI_STATUS_OK) return status;
    if (generated != generatedTokenCount) return UMI_STATUS_INVALID_STATE;

    /* All pulls use one persistent cache and RNG stream. The final token is
     * intentionally pending, matching the existing cache-inspection policy. */
    startCache = clock();
    status = UmiAiTrainingGenerationSessionCreate(model,
        promptTokenIds, promptTokenCount, policy, seed, &session);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < generatedTokenCount; ++i) {
        status = UmiAiTrainingGenerationSessionNext(session, &cached[i]);
        if (status != UMI_STATUS_OK) goto cleanup;
    }
    status = UmiAiTrainingGenerationSessionInspect(session, &inspected, &observed);
    if (status != UMI_STATUS_OK) goto cleanup;
    UmiAiTrainingGenerationSessionDestroy(session);
    session = NULL;
    endCache = clock();

    if (inspected != generatedTokenCount ||
        observed.computedRows != candidate.estimate.cachedComputedRows ||
        observed.windowRebuilds != candidate.estimate.cachedWindowRebuilds ||
        observed.retainedTokens != candidate.estimate.contextTokens)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < generatedTokenCount; ++i)
        if (cached[i] != reference[i]) return UMI_STATUS_INVALID_STATE;

    status = UmiAiTrainingDecoderInspect(model, &after, &laterSteps);
    if (status != UMI_STATUS_OK) return status;
    if (laterSteps != priorSteps ||
        after.vocabularySize != before.vocabularySize ||
        after.contextLength != before.contextLength ||
        after.channels != before.channels ||
        after.hiddenChannels != before.hiddenChannels ||
        after.attentionHeads != before.attentionHeads ||
        after.initialSeed != before.initialSeed)
        return UMI_STATUS_INVALID_STATE;

    candidate.matchingTokens = generatedTokenCount;
    candidate.observedCachedRows = observed.computedRows;
    candidate.observedWindowRebuilds = observed.windowRebuilds;
    candidate.observedRetainedTokens = observed.retainedTokens;

    /* These times include the complete algorithms and cache lifecycle. A
     * zero-duration reading can be a timer-resolution limitation. */
    double referenceSeconds = 0.0, cacheSeconds = 0.0;
    if (CpuSeconds(startReference, endReference, &referenceSeconds) &&
        CpuSeconds(startCache, endCache, &cacheSeconds)) {
        candidate.cpuClockAvailable = 1;
        candidate.referenceProcessCpuSeconds = referenceSeconds;
        candidate.cachedProcessCpuSeconds = cacheSeconds;
    }
    *outComparison = candidate;
    return UMI_STATUS_OK;

cleanup:
    UmiAiTrainingGenerationSessionDestroy(session);
    return status;
}
