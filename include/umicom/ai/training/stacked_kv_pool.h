/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/training/stacked_kv_pool.h
 *
 * PURPOSE:
 *   Bound the lifetime and raw K/V reservations of multiple independent
 *   stacked-decoder sessions while delegating tensors to the existing K/V store.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_AI_TRAINING_STACKED_KV_POOL_H
#define UMICOM_AI_TRAINING_STACKED_KV_POOL_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ai/training/stacked_kv_store.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiTrainingStackedKvPool UmiAiTrainingStackedKvPool;

/* Never interpret these fields individually. A lease is validated against
 * pool ownership, the slot index, its generation and its active state. */
typedef struct UmiAiTrainingStackedKvLease {
    uint64_t poolIdentity;
    uint64_t generation;
    size_t slotIndex;
} UmiAiTrainingStackedKvLease;

typedef struct UmiAiTrainingStackedKvPoolConfig {
    UmiAiTrainingStackedKvStoreConfig store;
    size_t maximumSessions;
    /* 0 = no raw K/V budget ceiling (maximumSessions still applies).
     * This excludes pool metadata, allocator overhead and model/tape memory. */
    size_t rawKvBudgetBytes;
} UmiAiTrainingStackedKvPoolConfig;

typedef struct UmiAiTrainingStackedKvPoolStats {
    size_t maximumSessions;
    size_t activeSessions;
    size_t peakActiveSessions;
    size_t rawKvBytesPerSession;
    size_t reservedRawKvBytes;
    size_t peakReservedRawKvBytes;
    size_t rawKvBudgetBytes;
    uint64_t totalAcquisitions;
    uint64_t totalReleases;
} UmiAiTrainingStackedKvPoolStats;

/* Validate the bounded stacked-model shape, real storage implementation and
 * FP32 planner before creating any pool. Initialise *outPool to NULL. On an
 * error the output pointer is unchanged. No shared model is retained. */
UmiStatus UmiAiTrainingStackedKvPoolCreate(
    const UmiAiTrainingStackedKvPoolConfig *config,
    UmiAiTrainingStackedKvPool **outPool);

/* Destroy all active stores; borrowed pointers and leases are invalidated.
 * Callers must stop using all borrowed pointers before Destroy or Release. */
void UmiAiTrainingStackedKvPoolDestroy(UmiAiTrainingStackedKvPool *pool);

/* Reserve one complete FP32 K/V store; do not count merely live token rows.
 * The actual store creation precedes all accounting/publication. The output
 * lease is untouched if capacity, budget, allocation or generation fails. */
UmiStatus UmiAiTrainingStackedKvPoolAcquire(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease *outLease);

/* Release the owned store, invalidate the lease and free its reservation.
 * A stale, already-released or foreign-pool lease cannot release another. */
UmiStatus UmiAiTrainingStackedKvPoolRelease(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease);

UmiStatus UmiAiTrainingStackedKvPoolInspect(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvPoolStats *outStats);
UmiStatus UmiAiTrainingStackedKvPoolInspectLease(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    UmiAiTrainingStackedKvStoreInfo *outInfo);

/* Borrow only for a synchronous operation while the lease remains active.
 * This borrowed pointer must NOT be retained across Release, pool destruction
 * or use on another thread. Pool and all its stores are caller-serialised.
 * The caller must independently verify the live decoder checkpoint before
 * passing a borrowed store to StackedKvStepPropose; a digest alone is not a
 * publisher signature or model-integrity approval. */
UmiStatus UmiAiTrainingStackedKvPoolBorrow(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    UmiAiTrainingStackedKvStore **outBorrowedStore);

/* Compare a caller-supplied checkpoint digest against pool and owned store.
 * This does not inspect a live decoder; use StackedModelIdentityVerify there. */
UmiStatus UmiAiTrainingStackedKvPoolCheckIdentity(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    const uint8_t checkpointSha256[UMI_AI_STACKED_KV_IDENTITY_BYTES]);

/* The following operations delegate to the canonical store's proven atomic
 * prefill/append/full-rollover/reset/read contracts. They do not compute
 * neural projections, shift old rows, or alter the raw reservation budget. */
UmiStatus UmiAiTrainingStackedKvPoolReset(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease);
UmiStatus UmiAiTrainingStackedKvPoolPrefill(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    const size_t *tokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount);
UmiStatus UmiAiTrainingStackedKvPoolAppend(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    size_t tokenId, const float *keys, size_t keyCount,
    const float *values, size_t valueCount);
UmiStatus UmiAiTrainingStackedKvPoolReplaceRolled(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    const size_t *tokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount);
UmiStatus UmiAiTrainingStackedKvPoolRead(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    size_t layerIndex, size_t tokenIndex,
    float *outKey, size_t keyCapacity,
    float *outValue, size_t valueCapacity);

#ifdef __cplusplus
}
#endif
#endif /* UMICOM_AI_TRAINING_STACKED_KV_POOL_H */
