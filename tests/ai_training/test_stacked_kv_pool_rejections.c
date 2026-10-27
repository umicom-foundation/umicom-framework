/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_rejections.c
 *
 * PURPOSE:
 *   Verify checkpoint, invalid-pointer, wrong-shape and rejected-output contracts.
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
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiAiTrainingStackedKvLease lease = {0};
    UmiAiTrainingStackedKvStoreInfo info, original;
    UmiAiTrainingStackedKvPoolStats stats, originalStats;
    const size_t ids[2] = {12U, 13U};
    uint8_t badDigest[UMI_AI_STACKED_KV_IDENTITY_BYTES] = {0};
    float values[32] = {0};
    float outKeys[8] = {0}, outValues[8] = {0};
    memset(&info, 0x5a, sizeof(info));
    original = info;
    memset(&stats, 0x5a, sizeof(stats));
    originalStats = stats;
    cfg.store.decoder.attentionHeads = 3U;
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&cfg, &pool) ==
        UMI_STATUS_INVALID_ARGUMENT && pool == NULL);
    cfg = PoolTestConfig();
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&cfg, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &lease) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolCheckIdentity(pool, lease, badDigest) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolPrefill(pool, lease,
        ids, 2U, values, 31U, values, 32U) != UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRead(pool, lease,
        0U, 0U, outKeys, 8U, outValues, 8U) != UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, lease, &info) == UMI_STATUS_OK);
    VERIFY(info.retainedTokens == 0U);
    UmiAiTrainingStackedKvLease invalid = lease;
    ++invalid.generation;
    info = original;
    stats = originalStats;
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, invalid, &info) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(memcmp(&info, &original, sizeof(info)) == 0);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, invalid) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolInspect(pool, &stats) == UMI_STATUS_OK);
    VERIFY(stats.activeSessions == 1U && stats.totalReleases == 0U);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, lease) == UMI_STATUS_OK);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("KV pool: malformed operations preserve owner and output state");
    return 0;
}
