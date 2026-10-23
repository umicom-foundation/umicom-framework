/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/generation_cache.h
 *
 * PURPOSE:
 *   Compose the existing decoder's qualified key/value inference cache with
 *   native top-p nucleus sampling, without introducing another model or RNG.
 *
 * ENGINEERING CONTRACT:
 *   The canonical decoder, cache, byte tokenizer and sampler remain the sole
 *   owners of their respective semantics. The generation cache is private to
 *   each call; the caller owns model and buffers and serialises decoder use.
 *   The old generation APIs, weights and checkpoint formats are untouched.
 *
 *   The first cache prefill uses the exact, full, BOS-left-padded context of
 *   the historical generator. Once the context rolls, the cache rebuilds its
 *   keys/values because absolute-position embeddings have changed. This is a
 *   correctness-first integration, NOT a promised speedup: full-window
 *   regeneration can cost as much as canonical inference.
 *
 *   Rejected operations leave all caller-owned output arrays and counters
 *   unchanged. Cache statistics count computed token rows, not elapsed time.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_GENERATION_CACHE_H
#define UMICOM_AI_TRAINING_GENERATION_CACHE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/generation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingGenerationCacheStats {
    uint64_t computedRows;
    uint64_t windowRebuilds;
    size_t retainedTokens;
} UmiAiTrainingGenerationCacheStats;

/* Return exactly newTokenCount IDs. The seed, sampling policy and input
 * conventions are those of UmiAiTrainingDecoderGenerateNucleus. TopP=1
 * retains the original top-K selection policy. A cache is built from the
 * decoder's current parameters for each call, never retained after return.
 * All output buffers, counts and optional statistics are failure-atomic.
 * The current implementation does not accept overlapping output objects. */
UmiStatus UmiAiTrainingDecoderGenerateCachedNucleus(
    UmiAiTrainingDecoder *model,
    const size_t *promptTokenIds, size_t promptTokenCount,
    const UmiAiTrainingNucleusOptions *policy,
    size_t newTokenCount, uint64_t seed,
    size_t *outTokenIds, size_t capacity, size_t *outCount,
    UmiAiTrainingGenerationCacheStats *outStats);

/* Reversible byte-token adapter. The model must use the Framework's 260-ID
 * byte vocabulary. The returned bytes may contain NUL or invalid UTF-8.
 * There is no implicit terminator and the call never executes source text. */
UmiStatus UmiAiTrainingDecoderGenerateCachedBytesNucleus(
    UmiAiTrainingDecoder *model,
    const uint8_t *promptBytes, size_t promptLength,
    const UmiAiTrainingNucleusOptions *policy,
    size_t newByteCount, uint64_t seed,
    uint8_t *outBytes, size_t capacity, size_t *outCount,
    UmiAiTrainingGenerationCacheStats *outStats);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_GENERATION_CACHE_H */
