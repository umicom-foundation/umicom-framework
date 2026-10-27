/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_idle_stale.c
 *
 * PURPOSE:
 *   Reject stale/foreign leases and malformed idle-retention policies atomically.
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
    UmiAiTrainingStackedKvPoolConfig config = PoolTestConfig();
    UmiAiTrainingStackedKvPool *pool = NULL, *other = NULL;
    UmiAiTrainingStackedKvLease old = {0}, current = {0};
    UmiAiTrainingStackedKvStoreInfo info, expectedInfo;
    UmiAiTrainingStackedKvPoolStats before = {0}, after = {0};
    memset(&info, 0x5a, sizeof(info));
    expectedInfo = info;
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&config, &other) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 1U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &old) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, old) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, old, &info) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(memcmp(&info, &expectedInfo, sizeof(info)) == 0);
    VERIFY(UmiAiTrainingStackedKvPoolBorrow(pool, old, NULL) ==
        UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, old) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &current) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(other, current) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &before) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolSetIdleRetention(pool,
        config.maximumSessions + 1U) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &after) == UMI_STATUS_OK);
    VERIFY(memcmp(&before, &after, sizeof(before)) == 0);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, old, &info) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(memcmp(&info, &expectedInfo, sizeof(info)) == 0);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, current) == UMI_STATUS_OK);
    UmiAiTrainingStackedKvPoolDestroy(other);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("Idle lease: foreign/stale rejection and failure-atomic retention policy");
    return 0;
}
