/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_lifecycle.c
 *
 * PURPOSE:
 *   Verify actual K/V prefill, append, read, reset and lease invalidation.
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
    const UmiAiTrainingStackedKvPoolConfig cfg = PoolTestConfig();
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiAiTrainingStackedKvLease a = {0}, b = {0};
    UmiAiTrainingStackedKvPoolStats stats = {0};
    UmiAiTrainingStackedKvStoreInfo info = {0};
    const size_t ids[2] = {41U, 42U};
    float keys[32], values[32], appendKeys[16], appendValues[16];
    float readKey[8] = {0}, readValue[8] = {0};
    UmiAiTrainingStackedKvStore *borrowed = NULL;
    PoolFill(keys, 32U, 0.5F);
    PoolFill(values, 32U, 2.5F);
    PoolFill(appendKeys, 16U, 4.5F);
    PoolFill(appendValues, 16U, 6.5F);
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&cfg, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolCheckIdentity(pool, a,
        cfg.store.checkpointSha256) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolBorrow(pool, a, &borrowed) == UMI_STATUS_OK);
    VERIFY(borrowed != NULL);
    VERIFY(UmiAiTrainingStackedKvPoolPrefill(pool, a, ids, 2U,
        keys, 32U, values, 32U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAppend(pool, a, 43U,
        appendKeys, 16U, appendValues, 16U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRead(pool, a, 1U, 2U,
        readKey, 8U, readValue, 8U) == UMI_STATUS_OK);
    VERIFY(memcmp(readKey, appendKeys + 8U, 8U * sizeof(float)) == 0);
    VERIFY(memcmp(readValue, appendValues + 8U, 8U * sizeof(float)) == 0);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, a, &info) == UMI_STATUS_OK);
    VERIFY(info.retainedTokens == 3U && info.reservedKvBytes == 512U);
    VERIFY(UmiAiTrainingStackedKvPoolReset(pool, a) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, a, &info) == UMI_STATUS_OK);
    VERIFY(info.retainedTokens == 0U && info.reservedKvBytes == 512U);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, a) == UMI_STATUS_OK);
    borrowed = NULL;
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, a) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &b) == UMI_STATUS_OK);
    VERIFY(a.slotIndex == b.slotIndex && a.generation != b.generation);
    VERIFY(UmiAiTrainingStackedKvPoolReset(pool, a) == UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.activeSessions == 1U && stats.totalAcquisitions == 2U &&
        stats.totalReleases == 1U && stats.reservedRawKvBytes == 512U);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, b) == UMI_STATUS_OK);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("KV pool: lifecycle and stale-lease handling passed");
    return 0;
}
