/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/kv_prefix_reuse.h
 *
 * PURPOSE:
 *   Prove the computation saved by reusing attention K/V while a sequence
 *   grows inside the decoder's fixed context, before any eviction occurs.
 *
 * IMPORTANT DISTINCTION:
 *   This is a growing-prefix logit workload, NOT legacy autoregressive
 *   generation. The existing generator left-pads BOS to a full context and
 *   rebuilds the cache on each rollover. Its output and checkpoint semantics
 *   are not changed by these opt-in measurement interfaces.
 *
 * ARCHITECTURE:
 *   The canonical decoder and existing Framework KV-cache are the only
 *   mathematical and memory owners. Repeated Prefill is the reference;
 *   Prefill once followed by Append is the streaming candidate. Both use
 *   identical token positions, weights and cache kernels. No duplicate
 *   attention kernel, RNG, tokenizer or new positional encoding is provided.
 *
 * OWNERSHIP:
 *   The caller retains the model and token IDs and serialises decoder use.
 *   Both functions leave caller output unchanged on all failed calls.
 *   CPU time observations may be unavailable or too coarse to interpret.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_KV_PREFIX_REUSE_H
#define UMICOM_AI_TRAINING_KV_PREFIX_REUSE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/kv_cache.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingKvPrefixWorkEstimate {
    size_t prefixTokens;
    size_t contextTokens;
    size_t singleCacheHeapBytes;
    uint64_t referenceComputedRows;
    uint64_t incrementalComputedRows;
    uint64_t computedRowsAvoided;
} UmiAiTrainingKvPrefixWorkEstimate;

typedef struct UmiAiTrainingKvPrefixPerformance {
    UmiAiTrainingKvPrefixWorkEstimate estimate;
    size_t matchingPrefixes;
    uint64_t observedReferenceRows;
    uint64_t observedIncrementalRows;
    uint64_t observedWindowRebuilds;
    float maximumAbsoluteLogitDifference;
    double referenceProcessCpuSeconds;
    double incrementalProcessCpuSeconds;
    int cpuClockAvailable;
} UmiAiTrainingKvPrefixPerformance;

/* An N-token sequence is inspected at prefixes 1, 2, ..., N, where
 * 1 <= N <= config->contextLength. Independently rebuilding each prefix
 * requires N(N+1)/2 token rows; retaining keys/values needs only N rows.
 * No claim is made for a sliding window after context capacity is reached.
 * Allocation is for ONE model-shaped cache and excludes temporary stack,
 * decoder storage, allocator metadata and process resident memory. */
UmiStatus UmiAiTrainingKvPrefixWorkEstimateBuild(
    const UmiAiTrainingDecoderConfig *config,
    size_t tokenCount,
    UmiAiTrainingKvPrefixWorkEstimate *outEstimate);

/* Run BOTH approaches on the same real decoder, with no checkpoint write.
 * Compare every vocabulary logit at every prefix within an absolute
 * difference <= 0.00004F (the existing KV-context regression tolerance).
 * Cache workloads and model configuration must also agree with the estimate.
 * If they do not, return INVALID_STATE without publishing a report.
 *
 * Each cache is created once before timing; the reported process CPU times
 * cover repeated Prefill calls and Prefill+Append calls respectively, but
 * exclude cache construction/destruction, validation and parity comparison.
 * They are observations, not speedup guarantees. */
UmiStatus UmiAiTrainingKvPrefixPerformanceCompare(
    UmiAiTrainingDecoder *model,
    const size_t *tokenIds, size_t tokenCount,
    UmiAiTrainingKvPrefixPerformance *outPerformance);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_KV_PREFIX_REUSE_H */
