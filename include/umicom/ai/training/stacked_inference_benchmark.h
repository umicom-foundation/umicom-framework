/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_inference_benchmark.h
 *
 * PURPOSE:
 *   Compare the canonical growing-prefix generator and the existing cached
 *   multi-layer generator under identical prompts, policies and random seeds.
 *   This is a bounded inference qualification and measurement SERVICE, not a
 *   competing Transformer, benchmark script, sampler or application model.
 *
 * CONTRACT:
 *   Successful runs demand exact token-ID parity for every generated position.
 *   Reference and cached runs alternate order, with separate warmups and
 *   repeated measurements. Timings cover entire generation calls (including
 *   setup, allocations, prefill, rollover and teardown), NOT TTFT/TPOT.
 *   A smaller work counter does not imply a faster model or deployment.
 *
 * OWNERSHIP:
 *   The decoder is borrowed and must be unchanged and caller-serialised.
 *   The service owns all temporary buffers and leaves *outReport untouched
 *   if validation, numerical comparison, clock access or allocation fails.
 *   Neither the existing full-context nor the cached engine is modified.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_INFERENCE_BENCHMARK_H
#define UMICOM_AI_TRAINING_STACKED_INFERENCE_BENCHMARK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_kv_incremental.h"
#include "umicom/ai/training/stacked_prefix_inference.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_STACKED_BENCHMARK_MAX_REPETITIONS 21U
#define UMI_AI_STACKED_BENCHMARK_MAX_WARMUPS 8U

typedef struct UmiAiTrainingStackedBenchmarkConfig {
    size_t warmups;              /* 0..8: full canonical + cached pairs. */
    size_t repetitions;          /* 1..21 paired measured comparisons. */
    size_t generatedTokens;      /* 1..64. */
    uint64_t samplerSeed;
    UmiAiTrainingNucleusOptions sampling;
} UmiAiTrainingStackedBenchmarkConfig;

typedef struct UmiAiTrainingStackedBenchmarkReport {
    size_t promptTokens;
    size_t generatedTokens;
    size_t contextTokens;
    size_t transformerLayers;
    size_t warmups;
    size_t repetitions;
    size_t completeMatchingPairs;
    uint64_t canonicalMedianNanoseconds;
    uint64_t cachedMedianNanoseconds;
    uint64_t canonicalMinimumNanoseconds;
    uint64_t cachedMinimumNanoseconds;
    uint64_t canonicalMaximumNanoseconds;
    uint64_t cachedMaximumNanoseconds;
    uint64_t canonicalComputedTokenLayerRows;
    uint64_t cachedComputedFullContextTokenLayerRows;
    uint64_t cachedIncrementalProjectionTokenLayerRows;
    uint64_t cachedAttentionQueryTokenLayerRows;
    uint64_t cachedWindowRebuilds;
    size_t cachedReservedKvBytes;
} UmiAiTrainingStackedBenchmarkReport;

/* Repetition count and output sizes are bounded. Each paired comparison uses
 * the SAME mutable decoder, but all public generators are synchronous and
 * their checkpoint-identity guards must pass. This function neither trains
 * nor changes model parameters. Timings are elapsed monotonic clock intervals;
 * cross-process comparisons require environment controls supplied separately.
 * No public report is returned unless every paired token sequence agrees. */
UmiStatus UmiAiTrainingStackedBenchmarkRun(
    UmiAiTrainingStackedDecoder *model,
    const size_t *promptIds, size_t promptCount,
    const UmiAiTrainingStackedBenchmarkConfig *config,
    UmiAiTrainingStackedBenchmarkReport *outReport);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_INFERENCE_BENCHMARK_H */
