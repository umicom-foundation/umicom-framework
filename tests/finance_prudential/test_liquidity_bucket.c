/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_liquidity_bucket.c
 *
 * PURPOSE:
 *   Verify aggregate inflows, outflows and closing liquidity for one horizon bucket.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_bucket.h"

int main(void) { UmiPrudentialLiquidityBucket b; if(umi_pru_liquidity_bucket_calculate(&b,1,100.0,20.0,30.0)!=UMI_STATUS_OK)return 1; return b.closing_liquidity==90.0?0:2; }
