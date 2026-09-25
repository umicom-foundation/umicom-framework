/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/open_tms/test_liquidity_ladder.c
 *
 * PURPOSE:
 *   Verify ordered liquidity buckets and cumulative gap evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/open_tms/liquidity_ladder.h"

int main(void)
{
    UmiOpenTmsLiquidityLadder ladder;
    UmiOpenTmsLiquidityBucket one = {"0-1d", 1000U, 100.0, 200.0, 20.0};
    UmiOpenTmsLiquidityBucket two = {"1-7d", 2000U, 300.0, 100.0, 0.0};
    umi_open_tms_liquidity_ladder_init(&ladder);
    assert(umi_open_tms_liquidity_ladder_add(&ladder, &one) == UMI_STATUS_OK);
    assert(umi_open_tms_liquidity_ladder_add(&ladder, &two) == UMI_STATUS_OK);
    assert(umi_open_tms_liquidity_ladder_gap(&ladder) == 120.0);
    assert(umi_open_tms_liquidity_ladder_worst_gap(&ladder) == -80.0);
    return 0;
}
