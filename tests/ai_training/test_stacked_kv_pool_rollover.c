/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ai_training/test_stacked_kv_pool_rollover.c
 *
 * PURPOSE:
 *   Require complete, correctly shifted projections at learned-position rollover.
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
    UmiAiTrainingStackedKvLease lease = {0};
    const size_t ids[4] = {40U, 41U, 42U, 43U};
    const size_t badShift[4] = {99U, 42U, 43U, 44U};
    const size_t goodShift[4] = {41U, 42U, 43U, 44U};
    float keys[64], values[64], shiftedKeys[64], shiftedValues[64];
    float readKey[8] = {0}, readValue[8] = {0};
    UmiAiTrainingStackedKvStoreInfo before = {0}, after = {0};
    PoolFill(keys, 64U, 1.0F);
    PoolFill(values, 64U, 3.0F);
    PoolFill(shiftedKeys, 64U, 7.0F);
    PoolFill(shiftedValues, 64U, 9.0F);
    VERIFY(UmiAiTrainingStackedKvPoolCreate(&cfg, &pool) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAcquire(pool, &lease) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolPrefill(pool, lease, ids, 4U,
        keys, 64U, values, 64U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, lease, &before) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolAppend(pool, lease, 44U,
        keys, 16U, values, 16U) != UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolReplaceRolled(pool, lease,
        badShift, 4U, shiftedKeys, 64U, shiftedValues, 64U) ==
        UMI_STATUS_INVALID_STATE);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, lease, &after) == UMI_STATUS_OK);
    VERIFY(after.fullWindowRebuilds == before.fullWindowRebuilds &&
        after.retainedTokens == 4U);
    VERIFY(UmiAiTrainingStackedKvPoolReplaceRolled(pool, lease,
        goodShift, 4U, shiftedKeys, 64U, shiftedValues, 64U) == UMI_STATUS_OK);
    VERIFY(UmiAiTrainingStackedKvPoolRead(pool, lease, 0U, 0U,
        readKey, 8U, readValue, 8U) == UMI_STATUS_OK);
    VERIFY(memcmp(readKey, shiftedKeys, 8U * sizeof(float)) == 0);
    VERIFY(memcmp(readValue, shiftedValues, 8U * sizeof(float)) == 0);
    VERIFY(UmiAiTrainingStackedKvPoolInspectLease(pool, lease, &after) == UMI_STATUS_OK);
    VERIFY(after.fullWindowRebuilds == 1U);
    VERIFY(UmiAiTrainingStackedKvPoolRelease(pool, lease) == UMI_STATUS_OK);
    UmiAiTrainingStackedKvPoolDestroy(pool);
    puts("KV pool: full-layer rollover and atomic rejection passed");
    return 0;
}
