/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_idle_reuse.c
 *
 * PURPOSE:
 *   Qualify reset-before-reuse, generation changes and allocation counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "stacked_kv_pool_fixture.h"

int main(void)
{
    const UmiAiTrainingStackedKvPoolConfig config = PoolTestConfig();
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiAiTrainingStackedKvLease first = {0}, second = {0};
    UmiAiTrainingStackedKvPoolStats stats = {0};
    UmiAiTrainingStackedKvStoreInfo info = {0};
    size_t ids[2] = {42U, 43U};
    float keys[32] = {0};
    float values[32] = {0};
    PoolFill(keys, 32U, 2.0F);
    PoolFill(values, 32U, 4.0F);

    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 1U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &first) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolPrefill(pool, first, ids, 2U,
        keys, 32U, values, 32U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, first) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.idleStores == 1U && stats.activeSessions == 0U);
    VERIFY(stats.reservedRawKvBytes == 0U && stats.idleRawKvBytes == 512U);
    VERIFY(stats.physicalRawKvBytes == 512U && stats.maximumIdleStores == 1U);
    VERIFY(stats.newStoreAllocations == 1U && stats.idleStoreReuses == 0U);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, first, &info) ==
        UMI_STATUS_INVALID_STATE);

    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &second) == UMI_STATUS_OK);
    VERIFY(second.slotIndex == first.slotIndex &&
        second.poolIdentity == first.poolIdentity &&
        second.generation != first.generation);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, second, &info) ==
        UMI_STATUS_OK);
    VERIFY(info.retainedTokens == 0U && info.initialPromptTokens == 0U &&
        info.singleRowAppends == 0U && info.acceptedTokenLayerRows == 0U);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.idleStores == 0U && stats.activeSessions == 1U &&
        stats.idleStoreReuses == 1U && stats.newStoreAllocations == 1U &&
        stats.physicalRawKvBytes == 512U &&
        stats.reservedRawKvBytes == 512U);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, first) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, second) == UMI_STATUS_OK);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("Idle reuse: zeroed canonical cache, new lease generation, no allocation");
    return 0;
}
