/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_slippage_analysis.c
 *
 * PURPOSE:
 *   Exercise aggregate execution slippage in integer ticks with side-aware sign.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/slippage_analysis.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingSlippageAnalysis a;
    umi_trading_slippage_analysis_init(&a);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_slippage_analysis_add(&a,UMI_SIDE_BUY,100,102)!=UMI_STATUS_OK)return 1;
    return umi_trading_slippage_analysis_average(&a)==2?0:2;
}
