/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/kv_cache.h
 *
 * PURPOSE:
 *   Reuse computed attention keys and values during native decoder inference.
 *   Preserve exact learned-position semantics when a context window moves.
 *
 * OWNERSHIP AND LIFETIME:
 *   Each cache owns its weight snapshot, key/value rows and token history.
 *   The borrowed decoder must remain alive and must not be used concurrently.
 *   A successful training step invalidates the cache; destroy and recreate
 *   the cache from the updated decoder before generating further logits.
 *   No source files, model parameters, checkpoints or external state are
 *   modified by cache operations. No GUI or scripting runtime is required.
 *
 * ARCHITECTURE:
 *   The canonical decoder owns model construction and training. This module
 *   provides an inference-only scalar CPU implementation of that decoder's
 *   pre-LN block with matching float32 materialisation points. Parity against
 *   canonical tape logits is a required regression test.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_KV_CACHE_H
#define UMICOM_AI_TRAINING_KV_CACHE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingKvCache UmiAiTrainingKvCache;

/* Snapshot an existing decoder's trained parameters. The decoder must outlive
 * the cache. The output pointer remains unchanged on failure. */
UmiStatus UmiAiTrainingKvCacheCreate(UmiAiTrainingDecoder *decoder,
                                    UmiAiTrainingKvCache **outCache);
void UmiAiTrainingKvCacheDestroy(UmiAiTrainingKvCache *cache);

/* Clear cached tokens and key/value rows, retaining the immutable weight
 * snapshot and starting a new sequence at absolute position zero. */
UmiStatus UmiAiTrainingKvCacheReset(UmiAiTrainingKvCache *cache);

/* Rebuild from an approved prefix of 1..contextLength token IDs. Output is a
 * SINGLE vocabulary-logit row for the last token, not all time positions.
 * Failed operations leave both the cache and caller-owned outputs unchanged.
 * `capacity` counts float values, not bytes. */
UmiStatus UmiAiTrainingKvCachePrefill(UmiAiTrainingKvCache *cache,
                                     const size_t *tokenIds, size_t tokenCount,
                                     float *outLogits, size_t capacity);

/* Compute ONE new attention query and reuse earlier keys/values while the
 * prefix is shorter than contextLength. Once full, drop the oldest token and
 * rebuild the window because learned absolute positions change for every
 * retained token. Reusing stale keys after that shift would be incorrect.
 * Errors are failure-atomic and do not consume tokens. */
UmiStatus UmiAiTrainingKvCacheAppend(UmiAiTrainingKvCache *cache,
                                    size_t tokenId,
                                    float *outLogits, size_t capacity);

/* Inspect the retained token count, total computed token rows, and number of
 * complete rolling-window rebuilds. Counters are educational work estimates,
 * NOT elapsed time or a claim of accelerated full-window generation.
 * Inspect rejects a stale cache after model training. */
UmiStatus UmiAiTrainingKvCacheInspect(const UmiAiTrainingKvCache *cache,
                                     size_t *outTokenCount,
                                     uint64_t *outComputedRows,
                                     uint64_t *outWindowRebuilds);

/* Compatibility: return a conservative, shape-independent UPPER BOUND for
 * the cache allocation, rather than the old fixed-size reservation. Existing
 * callers may use it for admission budgeting. It excludes allocator metadata,
 * stack staging and decoder-owned memory. Output is unchanged on failure. */
UmiStatus UmiAiTrainingKvCacheAllocationSize(size_t *outBytes);

/* Calculate exact model-shape-dependent cache heap bytes BEFORE allocation.
 * This includes the opaque control structure, parameter copies and the full
 * configured key/value/logit payload. Does not allocate or access weights.
 * Invalid configurations leave *outBytes unchanged. */
UmiStatus UmiAiTrainingKvCacheAllocationForConfig(
    const UmiAiTrainingDecoderConfig *config, size_t *outBytes);

/* Report the actual owned heap allocation for an existing cache, excluding
 * allocator metadata, borrowed decoder storage and temporary stack buffers.
 * No pointer into the cache is returned; *outBytes remains unchanged on error.
 * The decoder and cache must satisfy the documented lifetime contract. */
UmiStatus UmiAiTrainingKvCacheAllocatedBytes(
    const UmiAiTrainingKvCache *cache, size_t *outBytes);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_KV_CACHE_H */
