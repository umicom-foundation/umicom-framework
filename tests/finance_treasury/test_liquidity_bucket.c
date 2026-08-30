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
int main(void) {
    UmiTreasuryLiquidityBucket v;
    if (umi_treasury_liquidity_bucket_init(&v, "0-7d", 0, 7, 400, 600) != UMI_STATUS_OK) return 1;
    if (umi_treasury_liquidity_bucket_gap_minor(&v) != -200) return 2;
    return 0;
}
