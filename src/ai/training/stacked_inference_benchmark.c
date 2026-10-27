/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_inference_benchmark.c
 * PURPOSE: Native, read-only paired inference measurement with exact ID parity.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#if !defined(_WIN32)
# if !defined(_POSIX_C_SOURCE)
#  define _POSIX_C_SOURCE 200809L
# endif
#endif
#include "umicom/ai/training/stacked_inference_benchmark.h"

#include <stdint.h>
#include <string.h>
#include <time.h>
#if defined(_WIN32)
# include <windows.h>
#endif

/* Monotonic wall time is preferable to a CPU-time clock for elapsed latency.
 * Explicit failure prevents reporting negative intervals after a clock error.
 * No global timing state, non-standard script or external library is needed. */
static int BenchmarkNowNanoseconds(uint64_t *out)
{
#if defined(_WIN32)
    LARGE_INTEGER frequency;
    LARGE_INTEGER ticks;
    if (!QueryPerformanceFrequency(&frequency) ||
        !QueryPerformanceCounter(&ticks) ||
        frequency.QuadPart <= 0 || ticks.QuadPart < 0) return 0;
    uint64_t tickCount = (uint64_t)ticks.QuadPart;
    uint64_t rate = (uint64_t)frequency.QuadPart;
    uint64_t seconds = tickCount / rate;
    uint64_t remainder = tickCount % rate;
    if (seconds > UINT64_MAX / UINT64_C(1000000000)) return 0;
    /* Long double avoids overflow when (remainder * 1e9) is too large. */
    uint64_t subsecond = (uint64_t)(((long double)remainder *
        1000000000.0L) / (long double)rate);
    *out = seconds * UINT64_C(1000000000) + subsecond;
#else
    struct timespec value;
    if (clock_gettime(CLOCK_MONOTONIC, &value) != 0 ||
        value.tv_sec < 0 || value.tv_nsec < 0 ||
        value.tv_nsec >= 1000000000L) return 0;
    if ((uint64_t)value.tv_sec > UINT64_MAX / UINT64_C(1000000000))
        return 0;
    *out = (uint64_t)value.tv_sec * UINT64_C(1000000000) +
        (uint64_t)value.tv_nsec;
#endif
    return 1;
}

static uint64_t BenchmarkMedian(uint64_t *values, size_t count)
{
    /* Insertion sort is bounded to 21 samples. For even counts avoid an
     * overflow-prone sum by averaging halves plus the remaining bits. */
    for (size_t i = 1U; i < count; ++i) {
        uint64_t next = values[i];
        size_t cursor = i;
        while (cursor != 0U && values[cursor - 1U] > next) {
            values[cursor] = values[cursor - 1U];
            --cursor;
        }
        values[cursor] = next;
    }
    if ((count & 1U) != 0U) return values[count / 2U];
    const uint64_t a = values[count / 2U - 1U];
    const uint64_t b = values[count / 2U];
    return a / 2U + b / 2U + ((a & 1U) + (b & 1U)) / 2U;
}

UmiStatus UmiAiTrainingStackedBenchmarkRun(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingStackedBenchmarkConfig *config,
    UmiAiTrainingStackedBenchmarkReport *outReport)
{
    if (model == NULL || promptIds == NULL || config == NULL ||
        outReport == NULL || promptCount == 0U ||
        config->repetitions == 0U ||
        config->repetitions > UMI_AI_STACKED_BENCHMARK_MAX_REPETITIONS ||
        config->warmups > UMI_AI_STACKED_BENCHMARK_MAX_WARMUPS ||
        config->generatedTokens == 0U ||
        config->generatedTokens > UMI_AI_STACKED_MAX_GENERATED_TOKENS)
        return UMI_STATUS_INVALID_ARGUMENT;

    UmiAiTrainingStackedDecoderInfo info = {0};
    UmiStatus status = UmiAiTrainingStackedDecoderInspect(model, &info);
    if (status != UMI_STATUS_OK) return status;
    if (promptCount > (size_t)info.config.contextLength ||
        info.config.transformerLayers == 0U ||
        info.config.contextLength == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < promptCount; ++i)
        if (promptIds[i] >= (size_t)info.config.vocabularySize)
            return UMI_STATUS_INVALID_ARGUMENT;

    const size_t total = config->warmups + config->repetitions;
    const size_t generation = config->generatedTokens;
    uint64_t referenceDurations[UMI_AI_STACKED_BENCHMARK_MAX_REPETITIONS] = {0};
    uint64_t candidateDurations[UMI_AI_STACKED_BENCHMARK_MAX_REPETITIONS] = {0};
    size_t referenceTokens[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    size_t candidateTokens[UMI_AI_STACKED_MAX_GENERATED_TOKENS] = {0};
    UmiAiTrainingStackedPrefixStats reference = {0};
    UmiAiTrainingStackedKvIncrementalStats candidate = {0};
    UmiAiTrainingStackedBenchmarkReport staged = {0};

    for (size_t run = 0U; run < total; ++run) {
        size_t first = (run & 1U); /* Counterbalance systematic warm-cache bias. */
        for (size_t order = 0U; order < 2U; ++order) {
            size_t resultCount = 0U;
            uint64_t began = 0U, ended = 0U;
            if (!BenchmarkNowNanoseconds(&began)) return UMI_STATUS_UNAVAILABLE;
            if ((order ^ first) == 0U) {
                status = UmiAiTrainingStackedPrefixGenerateNucleus(model,
                    promptIds, promptCount, &config->sampling, generation,
                    config->samplerSeed, referenceTokens, generation,
                    &resultCount, &reference);
            } else {
                status = UmiAiTrainingStackedKvIncrementalGenerate(model,
                    promptIds, promptCount, &config->sampling, generation,
                    config->samplerSeed, candidateTokens, generation,
                    &resultCount, &candidate);
            }
            if (!BenchmarkNowNanoseconds(&ended)) return UMI_STATUS_UNAVAILABLE;
            if (status != UMI_STATUS_OK) return status;
            if (resultCount != generation || ended < began)
                return UMI_STATUS_INVALID_STATE;
            if (run >= config->warmups) {
                const size_t slot = run - config->warmups;
                if ((order ^ first) == 0U)
                    referenceDurations[slot] = ended - began;
                else
                    candidateDurations[slot] = ended - began;
            }
        }
        if (memcmp(referenceTokens, candidateTokens,
                   generation * sizeof(size_t)) != 0)
            return UMI_STATUS_INVALID_STATE;
    }

    staged.warmups = config->warmups;
    staged.repetitions = config->repetitions;
    staged.promptTokens = promptCount;
    staged.generatedTokens = generation;
    staged.contextTokens = (size_t)info.config.contextLength;
    staged.transformerLayers = (size_t)info.config.transformerLayers;
    staged.completeMatchingPairs = total;
    staged.canonicalMedianNanoseconds = BenchmarkMedian(referenceDurations,
        config->repetitions);
    staged.cachedMedianNanoseconds = BenchmarkMedian(candidateDurations,
        config->repetitions);
    staged.canonicalMinimumNanoseconds = referenceDurations[0];
    staged.cachedMinimumNanoseconds = candidateDurations[0];
    staged.canonicalMaximumNanoseconds = referenceDurations[config->repetitions-1U];
    staged.cachedMaximumNanoseconds = candidateDurations[config->repetitions-1U];
    staged.canonicalComputedTokenLayerRows = reference.computedTransformerTokenLayerRows;
    staged.cachedComputedFullContextTokenLayerRows =
        candidate.observedFullContextTokenLayerRows;
    staged.cachedIncrementalProjectionTokenLayerRows =
        candidate.observedIncrementalProjectionTokenLayerRows;
    staged.cachedAttentionQueryTokenLayerRows =
        candidate.observedAttentionQueryTokenLayerRows;
    staged.cachedWindowRebuilds = candidate.observedFullWindowRebuilds;
    staged.cachedReservedKvBytes = candidate.reservedKvBytes;
    *outReport = staged;
    return UMI_STATUS_OK;
}
