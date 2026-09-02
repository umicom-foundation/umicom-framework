/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_smart_order_router.c
 *
 * PURPOSE:
 *   Exercise choose broker routes using cost, latency and deterministic preference weights.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/smart_order_router.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingBrokerRouteTable t;
    UmiTradingBrokerRoute r;
    UmiFinancialId rid,v;
    UmiTradingSmartOrderRouterPolicy p={2U,1U};
    umi_trading_broker_route_table_init(&t);
    umi_trading_core_id_assign(&rid,"r");
    umi_trading_core_id_assign(&v,"v");
    umi_trading_broker_route_init(&r,&rid,&v,2U,3U,true);
    umi_trading_broker_route_table_add(&t,&r);
    return umi_trading_smart_order_router_select(&t,&p)!=NULL?0:1;
}
