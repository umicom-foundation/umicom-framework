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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/prudential/liquidity_bucket.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialLiquidityBucket b; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_liquidity_bucket_calculate(&b,1,100.0,20.0,30.0)!=UMI_STATUS_OK)return 1; return b.closing_liquidity==90.0?0:2; }
