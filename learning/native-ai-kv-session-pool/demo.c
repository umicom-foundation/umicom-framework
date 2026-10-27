/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: learning/native-ai-kv-session-pool/demo.c
 *
 * PURPOSE:
 *   Demonstrate bounded, checkpoint-bound K/V session acquisition and release.
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
    UmiAiTrainingStackedKvLease lease = {0};
    UmiAiTrainingStackedKvPoolStats stats = {0};
    config.store.decoder = (UmiAiTrainingStackedDecoderConfig){
        260U, 4U, 8U, 12U, 2U, 2U, UINT64_C(42)};
    config.store.checkpointSha256[0] = 1U;
    config.maximumSessions = 3U;
    config.rawKvBudgetBytes = 1024U;
    if (UmiAiTrainingStackedKvPoolCreate(&config, &pool) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvPoolAcquire(pool, &lease) != UMI_STATUS_OK ||
        UmiAiTrainingStackedKvPoolInspect(pool, &stats) != UMI_STATUS_OK) {
        UmiAiTrainingStackedKvPoolDestroy(pool);
        return 1;
    }
    printf("K/V sessions: %zu of %zu; reserved=%zu, budget=%zu bytes\n",
        stats.activeSessions, stats.maximumSessions,
        stats.reservedRawKvBytes, stats.rawKvBudgetBytes);
    printf("Lease pool=%" PRIu64 ", generation=%" PRIu64 "\n",
        lease.poolIdentity, lease.generation);
    if (UmiAiTrainingStackedKvPoolRelease(pool, lease) != UMI_STATUS_OK) {
        UmiAiTrainingStackedKvPoolDestroy(pool);
        return 1;
    }
    UmiAiTrainingStackedKvPoolDestroy(pool);
    return 0;
}
