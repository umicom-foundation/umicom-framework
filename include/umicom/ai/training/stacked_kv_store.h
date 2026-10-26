/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_kv_store.h
 * PURPOSE:
 *   Own bounded, immutable-until-published K/V projection rows for each layer
 *   of an experimental stacked decoder. Enforce full replacement on rollover
 *   of learned absolute token positions, never silently recycle stale rows.
 *
 * ARCHITECTURAL BOUNDARY:
 *   This is a real, shape-aware native C23 CACHE STORAGE OWNER. The established
 *   Framework stacked decoder remains the sole authority for token embedding,
 *   projection mathematics, attention, logits, checkpoints and sampling.
 *   A producer supplies independently computed, finite K/V projection rows.
 *   This service CANNOT certify that those supplied rows are mathematically
 *   correct, and it does NOT accelerate or replace StackedDecoderLogits.
 *   Model identity must be verified with the canonical Framework checkpoint
 *   identity service before the caller submits/uses cached projections.
 *
 *   During fixed-position prefix growth rows may be appended. Once context
 *   is full, replacement of ALL layers and ALL rows is mandatory on each
 *   rollover, because both learned positions and upper-layer input histories
 *   can change. Retaining shifted rows in this model is prohibited.
 *
 * OWNERSHIP: Caller owns token/K/V inputs. An opaque store owns its copy.
 *   Operations are synchronous and not thread-safe. Invalid inputs and
 *   insufficient outputs change neither store state nor caller buffers.
 * AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_KV_STORE_H
#define UMICOM_AI_TRAINING_STACKED_KV_STORE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_AI_STACKED_KV_IDENTITY_BYTES 32U

typedef struct UmiAiTrainingStackedKvStore UmiAiTrainingStackedKvStore;

typedef struct UmiAiTrainingStackedKvStoreConfig {
    UmiAiTrainingStackedDecoderConfig decoder;
    /* Exact 32-byte SHA-256 of the approved canonical stacked checkpoint.
     * Caller must obtain and independently verify this through Framework;
     * a plain digest is not a publisher signature or training approval. */
    uint8_t checkpointSha256[UMI_AI_STACKED_KV_IDENTITY_BYTES];
} UmiAiTrainingStackedKvStoreConfig;

typedef struct UmiAiTrainingStackedKvStoreInfo {
    size_t transformerLayers;
    size_t contextCapacity;
    size_t channels;
    size_t retainedTokens;
    size_t initialPromptTokens;
    uint64_t singleRowAppends;
    uint64_t fullWindowRebuilds;
    uint64_t acceptedTokenLayerRows;
    size_t reservedKvBytes;  /* Raw key + value storage only. */
    uint8_t checkpointSha256[UMI_AI_STACKED_KV_IDENTITY_BYTES];
} UmiAiTrainingStackedKvStoreInfo;

/* Allocate one shape-sized cache. Check the complete decoder configuration:
 * accepted shapes mirror the bounded stacked decoder's current limits.
 * Reject an all-zero placeholder digest. Initialise *outStore=NULL. The
 * decoder is NOT borrowed or retained, and no network/disk I/O occurs. */
UmiStatus UmiAiTrainingStackedKvStoreCreate(
    const UmiAiTrainingStackedKvStoreConfig *config,
    UmiAiTrainingStackedKvStore **outStore);

void UmiAiTrainingStackedKvStoreDestroy(UmiAiTrainingStackedKvStore *store);

/* Prefill an EMPTY store using 1..contextCapacity ordered tokens.
 * K/V input layout: [layer][provided token][channel] in row-major FP32.
 * keyCount and valueCount must equal layers * tokenCount * channels.
 * Models with learned absolute positions assign slots 0..tokenCount-1.
 * This is a complete atomic operation. */
UmiStatus UmiAiTrainingStackedKvStorePrefill(
    UmiAiTrainingStackedKvStore *store,
    const size_t *tokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount);

/* Append a single token only if the prefix is nonempty AND not yet full.
 * K/V inputs are [layer][channel], exactly layers * channels FP32 values.
 * The caller must have calculated rows for their existing absolute position;
 * appending never moves or changes an earlier cache row. */
UmiStatus UmiAiTrainingStackedKvStoreAppend(
    UmiAiTrainingStackedKvStore *store, size_t tokenId,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount);

/* On a FULL cache, accept ONLY an explicitly shifted context whose token IDs
 * equal oldIds[1..capacity-1] followed by one new ID. Supply all-new K/V rows
 * for ALL layers and ALL shifted positions: [layer][context][channel].
 * This is the entire rebuild transaction; no old projections survive it.
 * An invalid shape, shift, nonfinite value or overflow preserves everything. */
UmiStatus UmiAiTrainingStackedKvStoreReplaceRolled(
    UmiAiTrainingStackedKvStore *store,
    const size_t *shiftedTokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount);

/* Copy one layer/position's stored K and V, never expose internal pointers.
 * Output capacities are FLOAT counts and must each be >= channels.
 * The output arrays must not overlap. Rejected calls do not write either. */
UmiStatus UmiAiTrainingStackedKvStoreRead(
    const UmiAiTrainingStackedKvStore *store,
    size_t layerIndex, size_t tokenIndex,
    float *outKey, size_t keyCapacity,
    float *outValue, size_t valueCapacity);

/* Return exact model-label agreement, NOT an authentication verdict.
 * To detect a changed live model, first capture/verify its canonical identity
 * through UmiAiTrainingStackedModelIdentityVerify. */
UmiStatus UmiAiTrainingStackedKvStoreCheckIdentity(
    const UmiAiTrainingStackedKvStore *store,
    const uint8_t checkpointSha256[UMI_AI_STACKED_KV_IDENTITY_BYTES]);

UmiStatus UmiAiTrainingStackedKvStoreInspect(
    const UmiAiTrainingStackedKvStore *store,
    UmiAiTrainingStackedKvStoreInfo *outInfo);

/* Begin another independent prompt without changing model identity; old rows
 * are erased, and per-prompt row counters start again at zero. */
UmiStatus UmiAiTrainingStackedKvStoreReset(
    UmiAiTrainingStackedKvStore *store);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_KV_STORE_H */
