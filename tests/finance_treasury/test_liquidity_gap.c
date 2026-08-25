/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_liquidity_gap.c
 *
 * PURPOSE:
 *   Exercise liquidity gap validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/liquidity_gap.h"
int main(void) {
    UmiTreasuryLiquidityGap v;
    if (umi_treasury_liquidity_gap_init(&v, "gap", 30, 1000, 1300) != UMI_STATUS_OK) return 1;
    if(umi_treasury_liquidity_gap_net_minor(&v)!=-300)return 2;
    return 0;
}
