/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/generation_performance.h
 *
 * PURPOSE:
 *   Estimate and measure the actual work of canonical full-context versus
 *   persistent KV-cache generation on the SAME Framework-owned decoder.
 *
 * ENGINEERING CONTRACT:
 *   The introductory decoder uses learned absolute positions. Its qualified
 *   session always prefills a complete BOS-padded context, and rebuilding on
 *   each subsequent token is mandatory for numerical correctness. This API
 *   reports that work rather than claiming an unmeasured cache speedup.
 *   It introduces no second decoder, optimiser, RNG or tokeniser.
 *
 * OWNERSHIP:
 *   Caller owns the decoder, prompt and result. Comparison serialises all
 *   decoder use and never mutates model parameters or checkpoint state.
 *   Rejected calls leave *outComparison unchanged. CPU times are observations
 *   from clock(), NOT portable wall-clock latencies or speed guarantees.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_GENERATION_PERFORMANCE_H
#define UMICOM_AI_TRAINING_GENERATION_PERFORMANCE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/generation_session.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Heap bytes describe only the persistent KV cache reservation requested by
 * the existing Framework owner. Allocator metadata, training tape, temporary
 * stack and operating-system resident-set overhead are excluded. */
typedef struct UmiAiTrainingGenerationWorkEstimate {
    size_t promptTokens;
    size_t generatedTokens;
    size_t contextTokens;
    size_t cacheHeapBytes;
    uint64_t referenceComputedRows;
    uint64_t cachedComputedRows;
    uint64_t cachedWindowRebuilds;
} UmiAiTrainingGenerationWorkEstimate;

typedef struct UmiAiTrainingGenerationPerformance {
    UmiAiTrainingGenerationWorkEstimate estimate;
    size_t matchingTokens;
    uint64_t observedCachedRows;
    uint64_t observedWindowRebuilds;
    size_t observedRetainedTokens;
    double referenceProcessCpuSeconds;
    double cachedProcessCpuSeconds;
    int cpuClockAvailable;
} UmiAiTrainingGenerationPerformance;

/* Predict actual token-row work for the currently qualified decoder and KV
 * cache, using the identical prefill/rolling-window policy. promptTokenCount
 * may be zero; generatedTokenCount is 1..256. The cache allocation estimate
 * comes from the existing shape-aware cache owner. On error the output is
 * unchanged. This forecast is independent of CPU type and thread scheduling.
 * In the CURRENT absolute-position model, expected cached rows equal full
 * reference rows: a KV cache is NOT automatically a speed improvement. */
UmiStatus UmiAiTrainingGenerationWorkEstimateBuild(
    const UmiAiTrainingDecoderConfig *config,
    size_t promptTokenCount, size_t generatedTokenCount,
    UmiAiTrainingGenerationWorkEstimate *outEstimate);

/* Execute the canonical nucleus generator and the persistent KV-cache session
 * for the same model, prompt, seed, policy and token count. The results must
 * agree token-for-token, and observed row counts must match the estimate.
 * Otherwise INVALID_STATE is returned without publishing misleading evidence.
 * CPU measurements include the complete two generation calls, respectively,
 * including creation/destruction of the persistent session and its cache.
 * clock() may be unavailable or too coarse; CPU fields are zero in that case.
 * The caller must not train, destroy or use the model concurrently. */
UmiStatus UmiAiTrainingGenerationPerformanceCompare(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t generatedTokenCount, uint64_t seed,
    UmiAiTrainingGenerationPerformance *outComparison);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_GENERATION_PERFORMANCE_H */
