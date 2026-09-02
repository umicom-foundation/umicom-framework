/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_broker_route.c
 *
 * PURPOSE:
 *   Exercise describe a candidate broker/venue route with cost and latency scores.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/broker_route.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiFinancialId rid,vid;
    umi_trading_core_id_assign(&rid,"r");
    umi_trading_core_id_assign(&vid,"v");
     UmiTradingBrokerRoute v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_broker_route_init(&v,&rid,&vid,5U,10U,true)!=UMI_STATUS_OK) return 1;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(!umi_trading_broker_route_valid(&v)) return 2;
     return 0;
}
