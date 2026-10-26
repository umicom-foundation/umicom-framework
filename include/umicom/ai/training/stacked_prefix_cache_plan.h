/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_prefix_cache_plan.h
 * PURPOSE: Estimate the conditional work of a future, qualified growing-
 *          prefix multi-layer KV cache without replacing existing inference.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_PREFIX_CACHE_PLAN_H
#define UMICOM_AI_TRAINING_STACKED_PREFIX_CACHE_PLAN_H

#include <stdint.h>
#include "umicom/ai/training/stacked_decoder.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedPrefixCachePlan {
    size_t contextTokens;
    size_t promptTokens;
    size_t generatedTokens;
    size_t transformerLayers;
    uint64_t canonicalTokenLayerRows;
    uint64_t candidateTokenLayerRows;
    uint64_t avoidedTokenLayerRows;
    uint64_t prefixSingleRowAppends;
    uint64_t fullWindowRebuilds;
    uint64_t kvTensorFp32Bytes;
} UmiAiTrainingStackedPrefixCachePlan;

/* Forecast two numerical *execution strategies* for a fixed already-defined
 * stacked-decoder configuration. Canonical uses L*C token-layer rows per
 * sampling step; the hypothetical prefix cache starts with P rows, appends
 * one row per layer while positions remain fixed, then rebuilds ALL layers
 * after each context eviction. The final generated token is not appended.
 *
 * This is NOT an implemented stacked KV cache or a performance benchmark.
 * In the existing learned-absolute model, old keys MUST NOT survive a moved
 * window, and upper-layer hidden states may carry evicted history. This
 * planner makes full-window rebuilding an explicit invariant. Numerical
 * equivalence and timing require separate real-decoder experiments before
 * any optimisation is adopted. The KV-memory figure accounts for only raw
 * retained FP32 key/value tensors and excludes allocator/activation overhead.
 *
 * Delegates prospective shape validation and KV sizing to the established
 * ModelScalingEstimateBuild. Also enforces the stacked model's own limits.
 * Input G is 1..64, P is 1..C. No I/O, allocations or model mutations.
 * On every error *outPlan stays byte-for-byte unchanged. */
UmiStatus UmiAiTrainingStackedPrefixCachePlanBuild(
    const UmiAiTrainingStackedDecoderConfig *config,
    size_t promptTokens, size_t generatedTokens,
    UmiAiTrainingStackedPrefixCachePlan *outPlan);

#ifdef __cplusplus
}
#endif
#endif
