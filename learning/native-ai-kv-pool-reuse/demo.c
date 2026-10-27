/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-kv-pool-reuse/demo.c
 *
 * PURPOSE:
 *   Demonstrate opt-in store reuse without transferring checkpoint or KV data.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/training/stacked_kv_pool.h"
#include <inttypes.h>
#include <stdio.h>

int main(void)
{
    UmiAiTrainingStackedKvPoolConfig config = {0};
    UmiAiTrainingStackedKvPool *pool = NULL;
    UmiAiTrainingStackedKvLease first = {0}, next = {0};
    UmiAiTrainingStackedKvPoolStats stats = {0};
    config.store.decoder = (UmiAiTrainingStackedDecoderConfig){
        260U, 4U, 8U, 12U, 2U, 2U, UINT64_C(2026)};
    config.store.checkpointSha256[0] = UINT8_C(17);
    config.maximumSessions = 3U;
    config.rawKvBudgetBytes = 1024U;
    if (UmiAiTrainingStackedKvPoolCreate(&config, &pool) != UMI_STATUS_OK)
        return 1;
    if (UmiAiTrainingStackedKvPoolSetIdleRetention(pool, 1U) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvPoolAcquire(pool, &first) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvPoolRelease(pool, first) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvPoolAcquire(pool, &next) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvPoolInspect(pool, &stats) != UMI_STATUS_OK) {
        UmiAiTrainingStackedKvPoolDestroy(pool);
        return 1;
    }
    printf("active=%zu idle=%zu physical=%zuB allocations=%" PRIu64
           " reuses=%" PRIu64 "\n", stats.activeSessions,
           stats.idleStores, stats.physicalRawKvBytes,
           stats.newStoreAllocations, stats.idleStoreReuses);
    if (first.generation == next.generation ||
        stats.newStoreAllocations != 1U || stats.idleStoreReuses != 1U) {
        UmiAiTrainingStackedKvPoolDestroy(pool);
        return 1;
    }
    UmiAiTrainingStackedKvPoolDestroy(pool);
    return 0;
}
