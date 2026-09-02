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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_ladder.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialLiquidityLadder l={0}; UmiPrudentialLiquidityBucket b; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_liquidity_bucket_calculate(&b,1,100.0,20.0,30.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_liquidity_ladder_add(&l,&b)!=UMI_STATUS_OK)return 2; return umi_pru_liquidity_ladder_closing(&l)==90.0?0:3; }
