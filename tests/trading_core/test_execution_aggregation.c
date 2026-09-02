/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_execution_aggregation.c
 *
 * PURPOSE:
 *   Exercise aggregate fills with overflow-aware quantities and average prices.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_aggregation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingExecutionAggregation a;
     UmiTradingExecutionFill f;
     UmiFinancialId e,o;
     umi_trading_execution_aggregation_init(&a);
    umi_trading_core_id_assign(&e,"e");
    umi_trading_core_id_assign(&o,"o");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_execution_fill_init(&f,&e,&o,5,100,1)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_execution_aggregation_add(&a,&f)!=UMI_STATUS_OK)return 2;
    return a.total_lots==5&&a.average_price_ticks==100?0:3;
}
