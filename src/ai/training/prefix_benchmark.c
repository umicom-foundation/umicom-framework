/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/prefix_benchmark.c
 *
 * PURPOSE:
 *   Measure native C23 prefix generation against the independent canonical
 *   decoder while proving token parity, cache accounting, and model immutability.
 *
 * ARCHITECTURE:
 *   No second neural kernel, tokenizer, optimiser or sampler is introduced.
 *   The canonical decoder owns the full-context reference computation; the
 *   already-qualified KV cache owns incremental computation. This service
 *   owns only experiment orchestration, clocks, statistics and evidence.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif
#include "umicom/ai/training/prefix_benchmark.h"
#include "umicom/ai/training/byte_tokenizer.h"
#include "umicom/ai/training/checkpoint.h"

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

/* Each clock reading records its own source; comparing unlike sources is an
 * error, not an implied high-resolution timing. Windows QPC and POSIX
 * CLOCK_MONOTONIC are monotonic. C23 timespec_get is a fallback and must NOT
 * be described as a monotonic clock. This is native platform code in C23. */
typedef struct UmiAiPrefixWallTick {
#ifdef _WIN32
    LARGE_INTEGER counter;
#else
    struct timespec stamp;
#endif
    int monotonic;
    int available;
} UmiAiPrefixWallTick;

typedef struct UmiAiPrefixObservation {
    double wallSeconds;
    double processCpuSeconds;
    int wallValid;
    int wallMonotonic;
    int cpuValid;
} UmiAiPrefixObservation;

static UmiAiPrefixWallTick ReadWallTick(void)
{
    UmiAiPrefixWallTick tick = {0};
#ifdef _WIN32
    LARGE_INTEGER frequency = {0};
    if (QueryPerformanceFrequency(&frequency) != 0 &&
        frequency.QuadPart > 0 && QueryPerformanceCounter(&tick.counter) != 0) {
        tick.available = 1;
        tick.monotonic = 1;
    }
#else
#if defined(CLOCK_MONOTONIC)
    if (clock_gettime(CLOCK_MONOTONIC, &tick.stamp) == 0) {
        tick.available = 1;
        tick.monotonic = 1;
        return tick;
    }
#endif
    if (timespec_get(&tick.stamp, TIME_UTC) == TIME_UTC) {
        tick.available = 1;
        tick.monotonic = 0;
    }
#endif
    return tick;
}

static bool WallElapsed(UmiAiPrefixWallTick begin, UmiAiPrefixWallTick end,
                        double *outSeconds)
{
    double duration;
    if (!begin.available || !end.available ||
        begin.monotonic != end.monotonic || outSeconds == NULL)
        return false;
#ifdef _WIN32
    LARGE_INTEGER frequency = {0};
    if (!QueryPerformanceFrequency(&frequency) || frequency.QuadPart <= 0 ||
        end.counter.QuadPart < begin.counter.QuadPart)
        return false;
    duration = (double)(end.counter.QuadPart - begin.counter.QuadPart) /
               (double)frequency.QuadPart;
#else
    duration = (double)(end.stamp.tv_sec - begin.stamp.tv_sec) +
               (double)(end.stamp.tv_nsec - begin.stamp.tv_nsec) / 1.0e9;
#endif
    /* Exactly zero is sub-resolution, not evidence of a zero-cost operation. */
    if (!isfinite(duration) || duration <= 0.0) return false;
    *outSeconds = duration;
    return true;
}

static bool CpuElapsed(clock_t begin, clock_t end, double *outSeconds)
{
    double duration;
    if (outSeconds == NULL || begin == (clock_t)-1 ||
        end == (clock_t)-1 || end < begin || CLOCKS_PER_SEC <= 0)
        return false;
    duration = (double)(end - begin) / (double)CLOCKS_PER_SEC;
    if (!isfinite(duration) || duration <= 0.0) return false;
    *outSeconds = duration;
    return true;
}

static UmiAiPrefixObservation TakeObservation(UmiAiPrefixWallTick wallBegin,
                                               UmiAiPrefixWallTick wallEnd,
                                               clock_t cpuBegin, clock_t cpuEnd)
{
    UmiAiPrefixObservation observation = {0};
    observation.wallValid = WallElapsed(wallBegin, wallEnd,
                                        &observation.wallSeconds) ? 1 : 0;
    observation.wallMonotonic = wallBegin.monotonic && wallEnd.monotonic;
    observation.cpuValid = CpuElapsed(cpuBegin, cpuEnd,
                                      &observation.processCpuSeconds) ? 1 : 0;
    return observation;
}

/* This is the independent ORCHESTRATION reference: one canonical full
 * contextLength decoder call per sampled token, with right BOS-padding.
 * Future padding is causally invisible to the last retained prefix row.
 * No attention, softmax, sampling, RNG or neural arithmetic is duplicated. */
static UmiStatus GenerateCanonicalUnpadded(
    UmiAiTrainingDecoder *model, const UmiAiTrainingDecoderConfig *config,
    const size_t *prompt, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedCount, uint64_t seed, size_t *outTokens)
{
    size_t history[UMI_AI_DECODER_MAX_CONTEXT];
    size_t context[UMI_AI_DECODER_MAX_CONTEXT];
    float allLogits[UMI_AI_DECODER_MAX_CONTEXT * UMI_AI_DECODER_MAX_VOCABULARY];
    UmiAiTrainingRandomState random = {0};
    size_t retained = promptCount;
    const size_t contextLimit = (size_t)config->contextLength;
    const size_t vocabulary = (size_t)config->vocabularySize;
    UmiStatus status;

    status = UmiAiTrainingRandomSeed(&random, seed);
    if (status != UMI_STATUS_OK) return status;
    memcpy(history, prompt, retained * sizeof(*prompt));
    for (size_t step = 0U; step < generatedCount; ++step) {
        size_t next = 0U;
        for (size_t pos = 0U; pos < contextLimit; ++pos)
            context[pos] = pos < retained ? history[pos] : UMI_AI_BYTE_BOS;
        status = UmiAiTrainingDecoderLogits(model, context, contextLimit,
                    allLogits, contextLimit * vocabulary);
        if (status != UMI_STATUS_OK) return status;
        status = UmiAiTrainingSampleLogitsNucleus(
            &allLogits[(retained - 1U) * vocabulary], vocabulary,
            UMI_AI_BYTE_OFFSET, vocabulary, policy, &random, &next, NULL);
        if (status != UMI_STATUS_OK) return status;
        outTokens[step] = next;
        /* The last generated sample is deliberately pending, as in the KV
         * session, and is not part of the observed cache work. */
        if (step + 1U < generatedCount) {
            if (retained < contextLimit) {
                history[retained] = next;
                ++retained;
            } else {
                memmove(history, history + 1U,
                        (contextLimit - 1U) * sizeof(*history));
                history[contextLimit - 1U] = next;
            }
        }
    }
    return UMI_STATUS_OK;
}

static UmiStatus ExecuteCanonical(
    UmiAiTrainingDecoder *model, const UmiAiTrainingDecoderConfig *config,
    const size_t *prompt, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    const UmiAiTrainingPrefixBenchmarkPlan *plan,
    size_t *outTokens, UmiAiPrefixObservation *outObservation)
{
    UmiAiPrefixWallTick wallBegin = ReadWallTick();
    clock_t cpuBegin = clock();
    UmiStatus status = GenerateCanonicalUnpadded(model, config, prompt,
        promptCount, policy, plan->generatedTokens, plan->seed, outTokens);
    clock_t cpuEnd = clock();
    UmiAiPrefixWallTick wallEnd = ReadWallTick();
    if (status != UMI_STATUS_OK) return status;
    *outObservation = TakeObservation(wallBegin, wallEnd, cpuBegin, cpuEnd);
    return UMI_STATUS_OK;
}

static UmiStatus ExecuteIncremental(
    UmiAiTrainingDecoder *model,
    const size_t *prompt, size_t promptCount,
    const UmiAiTrainingNucleusOptions *policy,
    const UmiAiTrainingPrefixBenchmarkPlan *plan,
    size_t *outTokens, UmiAiTrainingGenerationCacheStats *outStats,
    UmiAiPrefixObservation *outObservation)
{
    size_t outCount = 0U;
    UmiAiPrefixWallTick wallBegin = ReadWallTick();
    clock_t cpuBegin = clock();
    /* Include the cache allocation, weight snapshot, every generated token,
     * all reporting, and session destruction in the measured interval. */
    UmiStatus status = UmiAiTrainingDecoderGeneratePrefixNucleus(
        model, prompt, promptCount, policy, plan->generatedTokens, plan->seed,
        outTokens, plan->generatedTokens, &outCount, outStats);
    clock_t cpuEnd = clock();
    UmiAiPrefixWallTick wallEnd = ReadWallTick();
    if (status != UMI_STATUS_OK) return status;
    if (outCount != plan->generatedTokens) return UMI_STATUS_INVALID_STATE;
    *outObservation = TakeObservation(wallBegin, wallEnd, cpuBegin, cpuEnd);
    return UMI_STATUS_OK;
}

static UmiStatus CheckPolicy(const UmiAiTrainingDecoderConfig *config,
                             const UmiAiTrainingNucleusOptions *policy)
{
    if (policy == NULL ||
        policy->topK == 0U ||
        policy->topK > (size_t)config->vocabularySize - UMI_AI_BYTE_OFFSET ||
        !isfinite(policy->topP) || policy->topP <= 0.0F ||
        policy->topP > 1.0F || !isfinite(policy->temperature) ||
        policy->temperature <= 0.0F)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

static uint64_t Fingerprint(const size_t *tokenIds, size_t count)
{
    uint64_t hash = UINT64_C(14695981039346656037);
    for (size_t index = 0U; index < count; ++index) {
        const uint64_t token = (uint64_t)tokenIds[index];
        for (size_t octet = 0U; octet < 8U; ++octet) {
            const unsigned shift = (unsigned)(octet * 8U);
            hash ^= (token >> shift) & UINT64_C(0xff);
            hash *= UINT64_C(1099511628211);
        }
    }
    return hash;
}

static void SortSeconds(double *values, size_t count)
{
    for (size_t index = 1U; index < count; ++index) {
        const double current = values[index];
        size_t insert = index;
        while (insert > 0U && values[insert - 1U] > current) {
            values[insert] = values[insert - 1U];
            --insert;
        }
        values[insert] = current;
    }
}

static double MedianSorted(const double *values, size_t count)
{
    if ((count & 1U) != 0U) return values[count / 2U];
    return 0.5 * values[count / 2U - 1U] + 0.5 * values[count / 2U];
}

static void PublishTiming(const UmiAiPrefixObservation observations[],
                          size_t count, UmiAiTrainingPrefixBenchmarkTiming *timing,
                          int *wallValid, int *monotonic, int *cpuValid)
{
    double walls[UMI_AI_PREFIX_BENCHMARK_MAX_PAIRS] = {0};
    double cpus[UMI_AI_PREFIX_BENCHMARK_MAX_PAIRS] = {0};
    int allWall = 1, allMonotonic = 1, allCpu = 1;
    for (size_t index = 0U; index < count; ++index) {
        allWall &= observations[index].wallValid;
        allMonotonic &= observations[index].wallMonotonic;
        allCpu &= observations[index].cpuValid;
        walls[index] = observations[index].wallSeconds;
        cpus[index] = observations[index].processCpuSeconds;
    }
    if (allWall) {
        SortSeconds(walls, count);
        timing->minimumWallSeconds = walls[0U];
        timing->medianWallSeconds = MedianSorted(walls, count);
        timing->maximumWallSeconds = walls[count - 1U];
    }
    if (allCpu) {
        SortSeconds(cpus, count);
        timing->medianProcessCpuSeconds = MedianSorted(cpus, count);
    }
    *wallValid = allWall;
    *monotonic = allMonotonic;
    *cpuValid = allCpu;
}

UmiStatus UmiAiTrainingPrefixBenchmarkRun(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    const UmiAiTrainingPrefixBenchmarkPlan *plan,
    UmiAiTrainingPrefixBenchmarkResult *outResult)
{
    UmiAiTrainingPrefixBenchmarkResult candidate = {0};
    UmiAiTrainingDecoderConfig before = {0};
    UmiAiTrainingDecoderConfig after = {0};
    UmiAiTrainingGenerationCacheStats observed = {0};
    UmiAiPrefixObservation referenceMeasurements[UMI_AI_PREFIX_BENCHMARK_MAX_PAIRS] = {0};
    UmiAiPrefixObservation cacheMeasurements[UMI_AI_PREFIX_BENCHMARK_MAX_PAIRS] = {0};
    uint8_t *checkpointBefore = NULL, *checkpointAfter = NULL;
    size_t checkpointSize = 0U, written = 0U;
    uint64_t earlierSteps = 0U, laterSteps = 0U;
    size_t reference[UMI_AI_PREFIX_BENCHMARK_MAX_TOKENS] = {0};
    size_t cached[UMI_AI_PREFIX_BENCHMARK_MAX_TOKENS] = {0};
    size_t firstReference[UMI_AI_PREFIX_BENCHMARK_MAX_TOKENS] = {0};
    UmiStatus status;
    int referenceWall = 0, cacheWall = 0, referenceMonotonic = 0;
    int cacheMonotonic = 0, referenceCpu = 0, cacheCpu = 0;

    if (model == NULL || promptTokenIds == NULL || policy == NULL ||
        plan == NULL || outResult == NULL ||
        plan->generatedTokens == 0U ||
        plan->generatedTokens > UMI_AI_PREFIX_BENCHMARK_MAX_TOKENS ||
        plan->warmupPairs > UMI_AI_PREFIX_BENCHMARK_MAX_WARMUPS ||
        plan->measuredPairs == 0U ||
        plan->measuredPairs > UMI_AI_PREFIX_BENCHMARK_MAX_PAIRS)
        return UMI_STATUS_INVALID_ARGUMENT;

    status = UmiAiTrainingDecoderInspect(model, &before, &earlierSteps);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingPrefixGenerationWorkEstimateBuild(
        &before, promptTokenCount, plan->generatedTokens, &candidate.estimate);
    if (status != UMI_STATUS_OK) return status;
    status = CheckPolicy(&before, policy);
    if (status != UMI_STATUS_OK) return status;
    for (size_t i = 0U; i < promptTokenCount; ++i)
        if (promptTokenIds[i] >= (size_t)before.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;

    /* The canonical, portable checkpoint encoding is stronger than a check
     * of optimiser steps alone: accidentally changed weights are rejected. */
    status = UmiAiTrainingDecoderCheckpointSize(model, &checkpointSize);
    if (status != UMI_STATUS_OK) return status;
    if (checkpointSize == 0U ||
        checkpointSize > UMI_AI_DECODER_MAX_CHECKPOINT_BYTES)
        return UMI_STATUS_INVALID_STATE;
    checkpointBefore = malloc(checkpointSize);
    checkpointAfter = malloc(checkpointSize);
    if (checkpointBefore == NULL || checkpointAfter == NULL) {
        status = UMI_STATUS_OUT_OF_MEMORY;
        goto cleanup;
    }
    status = UmiAiTrainingDecoderCheckpointEncode(model, checkpointBefore,
        checkpointSize, &written);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (written != checkpointSize) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }

    /* Alternate path ordering between PAIRS to reduce systematic cache/CPU
     * warming bias; never change the within-pair model, seed or prompt. */
    const size_t totalPairs = plan->warmupPairs + plan->measuredPairs;
    for (size_t pair = 0U; pair < totalPairs; ++pair) {
        UmiAiPrefixObservation refObservation = {0};
        UmiAiPrefixObservation cacheObservation = {0};
        if ((pair & 1U) == 0U) {
            status = ExecuteCanonical(model, &before, promptTokenIds,
                promptTokenCount, policy, plan, reference, &refObservation);
            if (status != UMI_STATUS_OK) goto cleanup;
            status = ExecuteIncremental(model, promptTokenIds,
                promptTokenCount, policy, plan, cached, &observed,
                &cacheObservation);
        } else {
            status = ExecuteIncremental(model, promptTokenIds,
                promptTokenCount, policy, plan, cached, &observed,
                &cacheObservation);
            if (status != UMI_STATUS_OK) goto cleanup;
            status = ExecuteCanonical(model, &before, promptTokenIds,
                promptTokenCount, policy, plan, reference, &refObservation);
        }
        if (status != UMI_STATUS_OK) goto cleanup;
        if (memcmp(cached, reference,
                    plan->generatedTokens * sizeof(size_t)) != 0 ||
            observed.computedRows != candidate.estimate.cachedComputedRows ||
            observed.windowRebuilds != candidate.estimate.cachedWindowRebuilds ||
            observed.retainedTokens !=
                ((promptTokenCount + plan->generatedTokens - 1U <
                  candidate.estimate.contextTokens) ?
                   promptTokenCount + plan->generatedTokens - 1U :
                   candidate.estimate.contextTokens)) {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
        if (pair == 0U)
            memcpy(firstReference, reference,
                   plan->generatedTokens * sizeof(size_t));
        else if (memcmp(firstReference, reference,
                        plan->generatedTokens * sizeof(size_t)) != 0) {
            status = UMI_STATUS_INVALID_STATE;
            goto cleanup;
        }
        ++candidate.verifiedPairs;
        candidate.comparedTokens += plan->generatedTokens;
        if (pair >= plan->warmupPairs) {
            const size_t index = pair - plan->warmupPairs;
            referenceMeasurements[index] = refObservation;
            cacheMeasurements[index] = cacheObservation;
        }
    }

    /* Ensure even a silent weight update (without changing the step counter)
     * cannot produce a successful benchmark certificate. No file is written. */
    written = 0U;
    status = UmiAiTrainingDecoderCheckpointEncode(model, checkpointAfter,
        checkpointSize, &written);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (written != checkpointSize ||
        memcmp(checkpointBefore, checkpointAfter, checkpointSize) != 0) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    status = UmiAiTrainingDecoderInspect(model, &after, &laterSteps);
    if (status != UMI_STATUS_OK) goto cleanup;
    if (laterSteps != earlierSteps ||
        before.vocabularySize != after.vocabularySize ||
        before.contextLength != after.contextLength ||
        before.channels != after.channels ||
        before.hiddenChannels != after.hiddenChannels ||
        before.attentionHeads != after.attentionHeads ||
        before.initialSeed != after.initialSeed) {
        status = UMI_STATUS_INVALID_STATE;
        goto cleanup;
    }
    candidate.checkpointUnchanged = 1;
    candidate.checkpointBytes = checkpointSize;
    candidate.observedCache = observed;
    candidate.tokenFingerprint = Fingerprint(firstReference, plan->generatedTokens);
    PublishTiming(referenceMeasurements, plan->measuredPairs,
        &candidate.canonical, &referenceWall, &referenceMonotonic,
        &referenceCpu);
    PublishTiming(cacheMeasurements, plan->measuredPairs,
        &candidate.incremental, &cacheWall, &cacheMonotonic,
        &cacheCpu);
    candidate.wallClockAvailable = referenceWall && cacheWall;
    candidate.wallClockMonotonic = referenceMonotonic && cacheMonotonic;
    candidate.cpuClockAvailable = referenceCpu && cacheCpu;
    /* A missing clock invalidates only the corresponding observation;
     * it must not invalidate proven output parity and checkpoint integrity. */
    if (!candidate.wallClockAvailable) {
        candidate.canonical.minimumWallSeconds = 0.0;
        candidate.canonical.medianWallSeconds = 0.0;
        candidate.canonical.maximumWallSeconds = 0.0;
        candidate.incremental.minimumWallSeconds = 0.0;
        candidate.incremental.medianWallSeconds = 0.0;
        candidate.incremental.maximumWallSeconds = 0.0;
    }
    if (!candidate.cpuClockAvailable) {
        candidate.canonical.medianProcessCpuSeconds = 0.0;
        candidate.incremental.medianProcessCpuSeconds = 0.0;
    }
    *outResult = candidate;
    status = UMI_STATUS_OK;
cleanup:
    free(checkpointAfter);
    free(checkpointBefore);
    return status;
}
