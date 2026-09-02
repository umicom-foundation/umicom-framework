/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_liquidity_headroom.c
 *
 * PURPOSE:
 *   Verify calculate liquidity headroom over a minimum required buffer.
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

#include "umicom/finance/prudential/liquidity_headroom.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialLiquidityHeadroom r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_liquidity_headroom_calculate(&r,120.0,100.0)!=UMI_STATUS_OK)return 1; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(r.headroom!=20.0||r.breached!=0)return 2; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_liquidity_headroom_calculate(&r,80.0,100.0)!=UMI_STATUS_OK||r.breached!=1)return 3; return 0; }
