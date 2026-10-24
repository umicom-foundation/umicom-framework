/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/prefix_benchmark.h
 *
 * PURPOSE:
 *   Qualify and time the SAME learned-absolute-position decoder under an
 *   explicitly unpadded generation policy. Compare canonical full-context
 *   logits against the existing incremental KV session, sample for sample.
 *
 * ENGINEERING CONTRACT:
 *   This benchmark never compares the historical BOS-left-padded generator
 *   with an unpadded prefix (their semantics differ). Its reference uses
 *   the original UmiAiTrainingDecoderLogits and nucleus sampler, with future
 *   context positions right-padded so they cannot influence causal logits.
 *   Both paths use the SAME model, prompt, sampling seed and generation count.
 *   Every trial must reproduce identical samples, match the expected KV work,
 *   and leave the portable decoder checkpoint byte-for-byte unchanged.
 *
 *   Timings are observations, not performance guarantees: wall clocks may
 *   be unavailable or coarse; process CPU time is shared with other threads.
 *   Neither timing nor computed-row count proves general model throughput.
 *
 * OWNERSHIP:
 *   The caller owns all inputs and the result. No file, checkpoint, weight,
 *   training state, or global RNG is modified. The decoder MUST NOT be used,
 *   trained or destroyed concurrently. On ANY rejection or internal failure,
 *   *outResult is unchanged. The API is a bounded educational diagnostic.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_PREFIX_BENCHMARK_H
#define UMICOM_AI_TRAINING_PREFIX_BENCHMARK_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/prefix_generation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_PREFIX_BENCHMARK_MAX_TOKENS 64U
#define UMI_AI_PREFIX_BENCHMARK_MAX_WARMUPS 5U
#define UMI_AI_PREFIX_BENCHMARK_MAX_PAIRS 9U

typedef struct UmiAiTrainingPrefixBenchmarkPlan {
    size_t generatedTokens;  /* 1..64, always exactly this many */
    size_t warmupPairs;      /* 0..5 complete, parity-checked pairs */
    size_t measuredPairs;    /* 1..9 complete, parity-checked pairs */
    uint64_t seed;           /* identical non-cryptographic sampling seed */
} UmiAiTrainingPrefixBenchmarkPlan;

typedef struct UmiAiTrainingPrefixBenchmarkTiming {
    double minimumWallSeconds;
    double medianWallSeconds;
    double maximumWallSeconds;
    double medianProcessCpuSeconds;
} UmiAiTrainingPrefixBenchmarkTiming;

typedef struct UmiAiTrainingPrefixBenchmarkResult {
    UmiAiTrainingPrefixGenerationWorkEstimate estimate;
    UmiAiTrainingGenerationCacheStats observedCache;
    size_t verifiedPairs;       /* warmups + measured pairs */
    size_t comparedTokens;      /* verifiedPairs * generatedTokens */
    uint64_t tokenFingerprint;  /* NON-cryptographic FNV-1a of token IDs */
    size_t checkpointBytes;     /* verified portable decoder checkpoint size */
    int checkpointUnchanged;    /* 1 on successful publication */
    int wallClockAvailable;     /* 0 if any measured reading is invalid */
    int wallClockMonotonic;     /* 1 for QPC / CLOCK_MONOTONIC readings */
    int cpuClockAvailable;      /* 0 for unavailable / sub-resolution clock() */
    UmiAiTrainingPrefixBenchmarkTiming canonical;
    UmiAiTrainingPrefixBenchmarkTiming incremental;
} UmiAiTrainingPrefixBenchmarkResult;

/* Run complete ordered pairs. Each measured pair alternates execution order
 * (canonical first, then incremental; next pair reverses the order).
 * Warmup pairs follow the same parity requirements but never enter timings.
 * A call rejects mismatched tokens, nonreproducible trials, wrong cache rows,
 * stale/altered decoder steps, or changed portable checkpoint bytes.
 * The report is staged until EVERY check succeeds. Timings do not affect
 * result correctness: unavailable timings are explicitly flagged as such.
 * All output memory belongs to the caller; no execution or model-loading
 * side effects are performed. */
UmiStatus UmiAiTrainingPrefixBenchmarkRun(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    const UmiAiTrainingPrefixBenchmarkPlan *plan,
    UmiAiTrainingPrefixBenchmarkResult *outResult);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_PREFIX_BENCHMARK_H */
