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
int main(void) {

    UmiTradingBestExecutionScore s;
    if(umi_trading_best_execution_score(10,2,3U,&s)!=UMI_STATUS_OK)return 1;
    return s.total_score==5?0:2;
}
