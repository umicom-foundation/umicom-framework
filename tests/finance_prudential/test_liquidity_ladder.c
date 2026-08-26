/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_liquidity_ladder.c
 *
 * PURPOSE:
 *   Verify build a bounded ordered liquidity ladder and cumulative balance.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; regulated applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_ladder.h"

int main(void) { UmiPrudentialLiquidityLadder l={0}; UmiPrudentialLiquidityBucket b; if(umi_pru_liquidity_bucket_calculate(&b,1,100.0,20.0,30.0)!=UMI_STATUS_OK)return 1; if(umi_pru_liquidity_ladder_add(&l,&b)!=UMI_STATUS_OK)return 2; return umi_pru_liquidity_ladder_closing(&l)==90.0?0:3; }
