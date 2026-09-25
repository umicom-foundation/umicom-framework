/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/open_tms/liquidity_ladder.c
 *
 * PURPOSE:
 *   Implement treasury liquidity ladder aggregation and worst-gap evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/open_tms/liquidity_ladder.h"
#include <string.h>

void umi_open_tms_liquidity_ladder_init(UmiOpenTmsLiquidityLadder *ladder)
{
    if (ladder == NULL) return;
    (void)memset(ladder, 0, sizeof(*ladder));
    ladder->revision = 1U;
}

UmiStatus umi_open_tms_liquidity_ladder_add(
    UmiOpenTmsLiquidityLadder *ladder,
    const UmiOpenTmsLiquidityBucket *bucket)
{
    if (ladder == NULL || bucket == NULL || bucket->id[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (ladder->count >= UMI_OPEN_TMS_LIQUIDITY_BUCKET_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (ladder->count > 0U &&
        bucket->endMilliseconds <= ladder->buckets[ladder->count - 1U].endMilliseconds)
        return UMI_STATUS_INVALID_STATE;
    ladder->buckets[ladder->count++] = *bucket;
    ladder->revision += 1U;
    return UMI_STATUS_OK;
}

double umi_open_tms_liquidity_ladder_gap(const UmiOpenTmsLiquidityLadder *ladder)
{
    size_t i;
    double total = 0.0;
    if (ladder == NULL) return 0.0;
    for (i = 0U; i < ladder->count; ++i)
        total += ladder->buckets[i].inflows +
                 ladder->buckets[i].buffer -
                 ladder->buckets[i].outflows;
    return total;
}

double umi_open_tms_liquidity_ladder_worst_gap(const UmiOpenTmsLiquidityLadder *ladder)
{
    size_t i;
    double cumulative = 0.0;
    double worst = 0.0;
    if (ladder == NULL) return 0.0;
    for (i = 0U; i < ladder->count; ++i) {
        cumulative += ladder->buckets[i].inflows +
                      ladder->buckets[i].buffer -
                      ladder->buckets[i].outflows;
        if (cumulative < worst) worst = cumulative;
    }
    return worst;
}
