/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_budget.c
 *
 * PURPOSE:
 *   Check raw payload budget, slot capacity and independent pool ownership.
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
    UmiAiTrainingStackedKvPoolConfig cfg = PoolTestConfig();
    UmiAiTrainingStackedKvPool *pool = NULL, *other = NULL;
    UmiAiTrainingStackedKvLease a = {0}, b = {0}, c = {0}, noChange = {0};
    UmiAiTrainingStackedKvPoolStats stats = {0};
    noChange.generation = UINT64_C(9876);
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&cfg, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &noChange) ==
        UMI_STATUS_CAPACITY_EXCEEDED);
    VERIFY(noChange.generation == UINT64_C(9876));
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.activeSessions == 2U && stats.reservedRawKvBytes == 1024U);
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&cfg, &other) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(other, a) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.activeSessions == 0U && stats.peakActiveSessions == 2U &&
        stats.peakReservedRawKvBytes == 1024U &&
        stats.totalAcquisitions == 3U && stats.totalReleases == 3U);
    UmiAiTrainingStackedKvPoolDestroy(other);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    pool = NULL;
    cfg.rawKvBudgetBytes = 511U;
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&cfg, &pool) ==
        UMI_STATUS_CAPACITY_EXCEEDED && pool == NULL);
    puts("KV pool: allocation admission and foreign leases passed");
    return 0;
}
