/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_best_execution.c
 *
 * PURPOSE:
 *   Exercise score execution outcomes using price, explicit cost and latency evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/best_execution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingBestExecutionScore s;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_best_execution_score(10,2,3U,&s)!=UMI_STATUS_OK)return 1;
    return s.total_score==5?0:2;
}
