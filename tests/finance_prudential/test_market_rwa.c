/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_prudential/test_market_rwa.c
 *
 * PURPOSE:
 *   Verify convert a market-risk capital charge into risk-weighted assets.
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

#include "umicom/finance/prudential/market_rwa.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiPrudentialMarketRwa r; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_pru_market_rwa_calculate(&r,8.0,12.5)!=UMI_STATUS_OK)return 1; return r.rwa==100.0?0:2; }
