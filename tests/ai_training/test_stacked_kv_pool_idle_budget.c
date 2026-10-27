/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_idle_budget.c
 *
 * PURPOSE:
 *   Account for active and idle physical bytes under a bounded KV budget.
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
    UmiAiTrainingStackedKvLease a = {0}, b = {0}, c = {0}, reject = {0};
    UmiAiTrainingStackedKvPoolStats stats = {0};
    reject.generation = UINT64_C(771);
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 1U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.activeSessions == 1U && stats.idleStores == 1U &&
        stats.reservedRawKvBytes == 512U && stats.idleRawKvBytes == 512U &&
        stats.physicalRawKvBytes == config.rawKvBudgetBytes);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &c) == UMI_STATUS_OK);
    VERIFY(c.slotIndex == a.slotIndex);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &reject) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
    VERIFY(reject.generation == UINT64_C(771));
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.physicalRawKvBytes == 1024U && stats.activeSessions == 2U &&
        stats.newStoreAllocations == 2U && stats.idleStoreReuses == 1U);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.idleStores == 1U && stats.physicalRawKvBytes == 512U &&
        stats.reservedRawKvBytes == 0U);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("Idle budget: active+idle physical reservation never exceeds limit");
    return 0;
}
