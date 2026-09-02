/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_market_status.c
 *
 * PURPOSE:
 *   Exercise capture exchange phase, sequence and operational availability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/market_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTradingMarketStatus v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_market_status_init(&v,UMI_TRADING_CORE_PHASE_CONTINUOUS,1U,true)!=UMI_STATUS_OK) return 1;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(!umi_trading_market_status_valid(&v)) return 2;
     return 0;
}
