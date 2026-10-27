/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/training/stacked_kv_pool.c
 *
 * PURPOSE:
 *   Own checkpoint-bound per-session K/V stores under an admission budget,
 *   generation-stamped leases and explicit release/reset semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_pool.h"
#include "umicom/ai/training/inference_memory_plan.h"

#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct KvPoolSlot {
    UmiAiTrainingStackedKvStore *store;
    uint64_t generation;
} KvPoolSlot;

struct UmiAiTrainingStackedKvPool {
    UmiAiTrainingStackedKvPoolConfig config;
    UmiAiTrainingStackedKvPoolStats stats;
    uint64_t identity;
    KvPoolSlot *slots;
};

/* An instance identity prevents an accidentally reused lease from a
 * different pool matching the same slot and generation. This monotonic ID
 * grants no security authority and never replaces checkpoint identity. */
static _Atomic uint64_t s_nextPoolIdentity = UINT64_C(1);

static int PoolIdentityAllocate(uint64_t *outId)
{
    uint64_t observed = atomic_load_explicit(
        &s_nextPoolIdentity, memory_order_relaxed);
    while (observed != UINT64_MAX) {
        if (atomic_compare_exchange_weak_explicit(&s_nextPoolIdentity,
                &observed, observed + UINT64_C(1),
                memory_order_relaxed, memory_order_relaxed)) {
            *outId = observed;
            return 1;
        }
    }
    return 0;
}

static KvPoolSlot *PoolLookup(UmiAiTrainingStackedKvPool *pool,
                              UmiAiTrainingStackedKvLease lease)
{
    if (pool == NULL || lease.poolIdentity != pool->identity ||
        lease.generation == 0U ||
        lease.slotIndex >= pool->config.maximumSessions) return NULL;
    KvPoolSlot *slot = &pool->slots[lease.slotIndex];
    return slot->store != NULL && slot->generation == lease.generation ?
           slot : NULL;
}

static const KvPoolSlot *PoolLookupConst(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease)
{
    if (pool == NULL || lease.poolIdentity != pool->identity ||
        lease.generation == 0U ||
        lease.slotIndex >= pool->config.maximumSessions) return NULL;
    const KvPoolSlot *slot = &pool->slots[lease.slotIndex];
    return slot->store != NULL && slot->generation == lease.generation ?
           slot : NULL;
}

UmiStatus UmiAiTrainingStackedKvPoolCreate(
    const UmiAiTrainingStackedKvPoolConfig *config,
    UmiAiTrainingStackedKvPool **outPool)
{
    UmiAiTrainingInferenceMemoryRequest req = {0};
    UmiAiTrainingInferenceMemoryReport report = {0};
    UmiAiTrainingStackedKvStore *probe = NULL;
    UmiAiTrainingStackedKvStoreInfo info = {0};
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiStatus status;
    uint64_t identity = 0U;
    if (config == NULL || outPool == NULL || *outPool != NULL ||
        config->maximumSessions == 0U ||
        config->maximumSessions > SIZE_MAX / sizeof(KvPoolSlot))
        return UMI_STATUS_INVALID_ARGUMENT;
    const UmiAiTrainingStackedDecoderConfig *model = &config->store.decoder;
    if (model->attentionHeads == 0U || model->channels == 0U ||
        model->channels % model->attentionHeads != 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    req.layers = (size_t)model->transformerLayers;
    req.contextTokens = (size_t)model->contextLength;
    req.queryHeads = (size_t)model->attentionHeads;
    req.keyValueHeads = (size_t)model->attentionHeads;
    req.headChannels = (size_t)(model->channels / model->attentionHeads);
    req.bytesPerScalar = sizeof(float);
    req.sessionCount = 1U;
    req.budgetBytes = config->rawKvBudgetBytes;
    status = UmiAiTrainingInferenceMemoryPlan(&req, &report);
    if (status != UMI_STATUS_OK) return status;
    if (config->rawKvBudgetBytes != 0U && !report.fitsBudget)
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* The real Framework K/V store validates bounded model geometry,
     * checkpoint content identity and its scaling-planner agreement. */
    status = UmiAiTrainingStackedKvStoreCreate(&config->store, &probe);
    if (status != UMI_STATUS_OK) return status;
    status = UmiAiTrainingStackedKvStoreInspect(probe, &info);
    UmiAiTrainingStackedKvStoreDestroy(probe);
    if (status != UMI_STATUS_OK) return status;
    if (info.reservedKvBytes != report.bytesPerSession)
        return UMI_STATUS_INVALID_STATE;

    pool = calloc(1U, sizeof(*pool));
    if (pool == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    pool->slots = calloc(config->maximumSessions, sizeof(*pool->slots));
    if (pool->slots == NULL) {
        free(pool);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    if (!PoolIdentityAllocate(&identity)) {
        free(pool->slots);
        free(pool);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    pool->config = *config;
    pool->identity = identity;
    pool->stats.maximumSessions = config->maximumSessions;
    pool->stats.rawKvBytesPerSession = info.reservedKvBytes;
    pool->stats.rawKvBudgetBytes = config->rawKvBudgetBytes;
    *outPool = pool;
    return UMI_STATUS_OK;
}

void UmiAiTrainingStackedKvPoolDestroy(UmiAiTrainingStackedKvPool *pool)
{
    if (pool == NULL) return;
    for (size_t i = 0U; i < pool->config.maximumSessions; ++i)
        UmiAiTrainingStackedKvStoreDestroy(pool->slots[i].store);
    free(pool->slots);
    /* The pool includes only model metadata and an untrusted SHA-256 digest;
     * it does not own model weights, token inputs or a training tape. */
    free(pool);
}

UmiStatus UmiAiTrainingStackedKvPoolAcquire(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease *outLease)
{
    if (pool == NULL || outLease == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const size_t bytes = pool->stats.rawKvBytesPerSession;
    if (pool->stats.activeSessions == pool->config.maximumSessions ||
        pool->stats.totalAcquisitions == UINT64_MAX ||
        bytes > SIZE_MAX - pool->stats.reservedRawKvBytes ||
        (pool->config.rawKvBudgetBytes != 0U &&
         bytes > pool->config.rawKvBudgetBytes - pool->stats.reservedRawKvBytes))
        return UMI_STATUS_CAPACITY_EXCEEDED;

    for (size_t i = 0U; i < pool->config.maximumSessions; ++i) {
        KvPoolSlot *slot = &pool->slots[i];
        if (slot->store == NULL && slot->generation != UINT64_MAX) {
            UmiAiTrainingStackedKvStore *created = NULL;
            UmiAiTrainingStackedKvStoreInfo info = {0};
            UmiStatus status = UmiAiTrainingStackedKvStoreCreate(
                &pool->config.store, &created);
            if (status != UMI_STATUS_OK) return status;
            status = UmiAiTrainingStackedKvStoreInspect(created, &info);
            if (status != UMI_STATUS_OK || info.reservedKvBytes != bytes) {
                UmiAiTrainingStackedKvStoreDestroy(created);
                return UMI_STATUS_INVALID_STATE;
            }
            slot->store = created;
            ++slot->generation;
            ++pool->stats.activeSessions;
            ++pool->stats.totalAcquisitions;
            pool->stats.reservedRawKvBytes += bytes;
            if (pool->stats.activeSessions > pool->stats.peakActiveSessions)
                pool->stats.peakActiveSessions = pool->stats.activeSessions;
            if (pool->stats.reservedRawKvBytes >
                pool->stats.peakReservedRawKvBytes)
                pool->stats.peakReservedRawKvBytes =
                    pool->stats.reservedRawKvBytes;
            *outLease = (UmiAiTrainingStackedKvLease){
                pool->identity, slot->generation, i};
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_CAPACITY_EXCEEDED;
}

UmiStatus UmiAiTrainingStackedKvPoolRelease(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease)
{
    KvPoolSlot *slot = PoolLookup(pool, lease);
    if (slot == NULL) return UMI_STATUS_INVALID_STATE;
    if (pool->stats.totalReleases == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiAiTrainingStackedKvStoreDestroy(slot->store);
    slot->store = NULL;
    --pool->stats.activeSessions;
    ++pool->stats.totalReleases;
    pool->stats.reservedRawKvBytes -= pool->stats.rawKvBytesPerSession;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvPoolInspect(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvPoolStats *outStats)
{
    if (pool == NULL || outStats == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outStats = pool->stats;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvPoolInspectLease(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    UmiAiTrainingStackedKvStoreInfo *outInfo)
{
    if (outInfo == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const KvPoolSlot *slot = PoolLookupConst(pool, lease);
    return slot == NULL ? UMI_STATUS_INVALID_STATE :
           UmiAiTrainingStackedKvStoreInspect(slot->store, outInfo);
}

UmiStatus UmiAiTrainingStackedKvPoolBorrow(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    UmiAiTrainingStackedKvStore **outBorrowedStore)
{
    if (outBorrowedStore == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    KvPoolSlot *slot = PoolLookup(pool, lease);
    if (slot == NULL) return UMI_STATUS_INVALID_STATE;
    *outBorrowedStore = slot->store;
    return UMI_STATUS_OK;
}

UmiStatus UmiAiTrainingStackedKvPoolCheckIdentity(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    const uint8_t checkpointSha256[UMI_AI_STACKED_KV_IDENTITY_BYTES])
{
    if (checkpointSha256 == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    const KvPoolSlot *slot = PoolLookupConst(pool, lease);
    if (slot == NULL) return UMI_STATUS_INVALID_STATE;
    if (memcmp(pool->config.store.checkpointSha256, checkpointSha256,
               UMI_AI_STACKED_KV_IDENTITY_BYTES) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UmiAiTrainingStackedKvStoreCheckIdentity(
        slot->store, checkpointSha256);
}

UmiStatus UmiAiTrainingStackedKvPoolReset(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease)
{
    KvPoolSlot *slot = PoolLookup(pool, lease);
    return slot == NULL ? UMI_STATUS_INVALID_STATE :
           UmiAiTrainingStackedKvStoreReset(slot->store);
}

UmiStatus UmiAiTrainingStackedKvPoolPrefill(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    const size_t *tokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount)
{
    KvPoolSlot *slot = PoolLookup(pool, lease);
    return slot == NULL ? UMI_STATUS_INVALID_STATE :
           UmiAiTrainingStackedKvStorePrefill(slot->store, tokenIds,
               tokenCount, keys, keyCount, values, valueCount);
}

UmiStatus UmiAiTrainingStackedKvPoolAppend(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    size_t tokenId, const float *keys, size_t keyCount,
    const float *values, size_t valueCount)
{
    KvPoolSlot *slot = PoolLookup(pool, lease);
    return slot == NULL ? UMI_STATUS_INVALID_STATE :
           UmiAiTrainingStackedKvStoreAppend(slot->store, tokenId,
               keys, keyCount, values, valueCount);
}

UmiStatus UmiAiTrainingStackedKvPoolReplaceRolled(
    UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    const size_t *tokenIds, size_t tokenCount,
    const float *keys, size_t keyCount,
    const float *values, size_t valueCount)
{
    KvPoolSlot *slot = PoolLookup(pool, lease);
    return slot == NULL ? UMI_STATUS_INVALID_STATE :
           UmiAiTrainingStackedKvStoreReplaceRolled(slot->store, tokenIds,
               tokenCount, keys, keyCount, values, valueCount);
}

UmiStatus UmiAiTrainingStackedKvPoolRead(
    const UmiAiTrainingStackedKvPool *pool,
    UmiAiTrainingStackedKvLease lease,
    size_t layerIndex, size_t tokenIndex,
    float *outKey, size_t keyCapacity,
    float *outValue, size_t valueCapacity)
{
    const KvPoolSlot *slot = PoolLookupConst(pool, lease);
    return slot == NULL ? UMI_STATUS_INVALID_STATE :
           UmiAiTrainingStackedKvStoreRead(slot->store, layerIndex, tokenIndex,
               outKey, keyCapacity, outValue, valueCapacity);
}
