/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_execution_quality.c
 *
 * PURPOSE:
 *   Exercise measure execution price improvement or shortfall against a reference price.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_quality.h"
int main(void) {

    UmiTradingExecutionQuality q;
    if(umi_trading_execution_quality_compute(UMI_SIDE_BUY,100,98,&q)!=UMI_STATUS_OK)return 1;
    return q.signed_improvement_ticks==2?0:2;
}
