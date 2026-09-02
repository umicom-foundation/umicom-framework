/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_liquidity_bucket.c
 *
 * PURPOSE:
 *   Exercise liquidity bucket validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_bucket.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryLiquidityBucket v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_liquidity_bucket_init(&v, "0-7d", 0, 7, 400, 600) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_treasury_liquidity_bucket_gap_minor(&v) != -200) return 2;
    return 0;
}
