/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/kv_prefix_reuse.c
 *
 * PURPOSE:
 *   Benchmark actual K/V prefix reuse before eviction, proving numerical
 *   parity with independent prefix rebuilds on the same Framework decoder.
 *
 * ARCHITECTURE:
 *   Compose existing decoder inspection, model-shaped cache allocation,
 *   cache Prefill/Append and inspection; do not duplicate neural mathematics.
 *   Publish reports only after every shape, work and logit check succeeds.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/kv_prefix_reuse.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define UMI_AI_KV_PREFIX_LOGIT_TOLERANCE 0.00004F

static bool SameConfig(const UmiAiTrainingDecoderConfig *a,
                       const UmiAiTrainingDecoderConfig *b)
{
    return a->vocabularySize == b->vocabularySize &&
           a->contextLength == b->contextLength &&
           a->channels == b->channels &&
           a->hiddenChannels == b->hiddenChannels &&
           a->attentionHeads == b->attentionHeads &&
           a->initialSeed == b->initialSeed;
}

static bool CpuSeconds(clock_t first, clock_t last, double *outSeconds)
{
    double seconds;
    if (first == (clock_t)-1 || last == (clock_t)-1 ||
        last < first || CLOCKS_PER_SEC <= 0) return false;
    seconds = (double)(last - first) / (double)CLOCKS_PER_SEC;
    if (!isfinite(seconds) || seconds < 0.0) return false;
    *outSeconds = seconds;
    return true;
}

UmiStatus UmiAiTrainingKvPrefixWorkEstimateBuild(
    const UmiAiTrainingDecoderConfig *config,
    size_t tokenCount,
    UmiAiTrainingKvPrefixWorkEstimate *outEstimate)
{
    UmiAiTrainingKvPrefixWorkEstimate candidate = {0};
    UmiStatus status;
    size_t heapBytes = 0U;

    if (config == NULL || outEstimate == NULL ||
        tokenCount == 0U || tokenCount > UMI_AI_DECODER_MAX_CONTEXT ||
        tokenCount > (size_t)config->contextLength)
        return UMI_STATUS_INVALID_ARGUMENT;

    /* The existing layout planner is the authoritative shape validator. */
    status = UmiAiTrainingKvCacheAllocationForConfig(config, &heapBytes);
    if (status != UMI_STATUS_OK) return status;

    candidate.prefixTokens = tokenCount;
    candidate.contextTokens = (size_t)config->contextLength;
    candidate.singleCacheHeapBytes = heapBytes;
    candidate.referenceComputedRows =
        (uint64_t)tokenCount * ((uint64_t)tokenCount + UINT64_C(1)) / UINT64_C(2);
    candidate.incrementalComputedRows = (uint64_t)tokenCount;
    candidate.computedRowsAvoided = candidate.referenceComputedRows -
                                    candidate.incrementalComputedRows;
    *outEstimate = candidate;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingKvPrefixPerformanceCompare(
    UmiAiTrainingDecoder *model,
    const size_t *tokenIds, size_t tokenCount,
    UmiAiTrainingKvPrefixPerformance *outPerformance)
{
    UmiAiTrainingKvPrefixPerformance candidate = {0};
    UmiAiTrainingDecoderConfig before = {0}, after = {0};
    UmiAiTrainingKvCache *reference = NULL, *incremental = NULL;
    /* Both dimensions are bounded by the decoder's public limits. */
    float prefixLogits[UMI_AI_DECODER_MAX_CONTEXT][UMI_AI_DECODER_MAX_VOCABULARY];
    float streamedPrefixLogits[UMI_AI_DECODER_MAX_CONTEXT][UMI_AI_DECODER_MAX_VOCABULARY];
    uint64_t beforeSteps = 0U, afterSteps = 0U;
    uint64_t rows = 0U, rebuilds = 0U;
    uint64_t referenceRows = 0U;
    size_t retained = 0U, bytes = 0U;
    size_t vocabulary;
    clock_t firstReference, lastReference, firstIncremental, lastIncremental;
    UmiStatus status;

    if (model == NULL || tokenIds == NULL || outPerformance == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    status = UmiAiTrainingDecoderInspect(model, &before, &beforeSteps);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingKvPrefixWorkEstimateBuild(&before, tokenCount,
                                                    &candidate.estimate);
    if (status != UMI_STATUS_OK) return status;
    vocabulary = (size_t)before.vocabularySize;
    if (vocabulary > UMI_AI_DECODER_MAX_VOCABULARY)
        return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < tokenCount; ++i)
        if (tokenIds[i] >= vocabulary) return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingKvCacheCreate(model, &reference);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingKvCacheCreate(model, &incremental);
    if (status != UMI_STATUS_OK) goto cleanup;
    status = UmiAiTrainingKvCacheAllocatedBytes(reference, &bytes);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (bytes != candidate.estimate.singleCacheHeapBytes) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    status = UmiAiTrainingKvCacheAllocatedBytes(incremental, &bytes);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (bytes != candidate.estimate.singleCacheHeapBytes) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }

    /* Repeatedly rebuild independent prefixes from position zero. The cache
     * owner deliberately resets its counters on every successful Prefill. */
    firstReference = clock();
    for (size_t length = 1U; length <= tokenCount; ++length) {
        status = UmiAiTrainingKvCachePrefill(reference, tokenIds, length,
            prefixLogits[length - 1U], vocabulary);
        if (status != UMI_STATUS_OK) goto cleanup;
        status = UmiAiTrainingKvCacheInspect(reference, &retained,
                                            &rows, &rebuilds);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (retained != length || rows != (uint64_t)length || rebuilds != 0U) {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
        referenceRows += rows;
    }
    lastReference = clock();

    /* One initial Prefill then one Append per new token. Nothing evicts, so
     * absolute positions 0..N-1 remain stable and K/V rows are reusable. */
    firstIncremental = clock();
    for (size_t length = 1U; length <= tokenCount; ++length) {
        if (length == 1U)
            status = UmiAiTrainingKvCachePrefill(incremental, tokenIds,
                1U, streamedPrefixLogits[length - 1U], vocabulary);
        else
            status = UmiAiTrainingKvCacheAppend(incremental,
                tokenIds[length - 1U], streamedPrefixLogits[length - 1U],
                vocabulary);
        if (status != UMI_STATUS_OK) goto cleanup;
        status = UmiAiTrainingKvCacheInspect(incremental, &retained,
                                            &rows, &rebuilds);
        if (status != UMI_STATUS_OK) goto cleanup;
        if (retained != length || rows != (uint64_t)length || rebuilds != 0U) {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
    }
    lastIncremental = clock();

    /* Parity qualification is performed OUTSIDE the two timed workloads;
     * both timings include equally scoped per-prefix cache inspection. */
    for (size_t length = 1U; length <= tokenCount; ++length) {
        for (size_t col = 0U; col < vocabulary; ++col) {
            float delta;
            if (!isfinite(streamedPrefixLogits[length - 1U][col]) ||
                !isfinite(prefixLogits[length - 1U][col])) {
                status = UMI_STATUS_INVALID_STATE;
                goto cleanup;
            }
            delta = fabsf(streamedPrefixLogits[length - 1U][col] -
                          prefixLogits[length - 1U][col]);
            if (!isfinite(delta) || delta > UMI_AI_KV_PREFIX_LOGIT_TOLERANCE) {
                status = UMI_STATUS_INVALID_STATE;
                goto cleanup;
            }
            if (delta > candidate.maximumAbsoluteLogitDifference)
                candidate.maximumAbsoluteLogitDifference = delta;
        }
        ++candidate.matchingPrefixes;
    }

    if (referenceRows != candidate.estimate.referenceComputedRows ||
        rows != candidate.estimate.incrementalComputedRows) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    status = UmiAiTrainingDecoderInspect(model, &after, &afterSteps);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (afterSteps != beforeSteps || !SameConfig(&before, &after)) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    candidate.observedReferenceRows = referenceRows;
    candidate.observedIncrementalRows = rows;
    candidate.observedWindowRebuilds = rebuilds;
    {
        double referenceSeconds = 0.0, incrementalSeconds = 0.0;
        if (CpuSeconds(firstReference, lastReference, &referenceSeconds) &&
            CpuSeconds(firstIncremental, lastIncremental, &incrementalSeconds)) {
            candidate.cpuClockAvailable = 1;
            candidate.referenceProcessCpuSeconds = referenceSeconds;
            candidate.incrementalProcessCpuSeconds = incrementalSeconds;
        }
    }
    *outPerformance = candidate;
    status = UMI_STATUS_OK;
cleanup:
    UmiAiTrainingKvCacheDestroy(incremental);
    UmiAiTrainingKvCacheDestroy(reference);
    return status;
}
