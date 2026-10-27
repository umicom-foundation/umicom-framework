/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_idle_trim.c
 *
 * PURPOSE:
 *   Prove that lowering idle retention frees idle stores but preserves active leases.
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
    UmiAiTrainingStackedKvLease a = {0}, b = {0}, c = {0};
    UmiAiTrainingStackedKvPoolStats stats = {0};

    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 2U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, b) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.idleStores == 2U && stats.idleRawKvBytes == 1024U &&
        stats.physicalRawKvBytes == 1024U && stats.activeSessions == 0U);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 1U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.idleStores == 1U && stats.idleRawKvBytes == 512U &&
        stats.physicalRawKvBytes == 512U && stats.peakPhysicalRawKvBytes == 1024U);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 0U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.activeSessions == 1U && stats.physicalRawKvBytes == 512U &&
        stats.idleStores == 0U && stats.maximumIdleStores == 0U);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, c) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.activeSessions == 0U && stats.idleStores == 0U &&
        stats.physicalRawKvBytes == 0U);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("Idle trim: deterministic idle eviction, active sessions preserved");
    return 0;
}
