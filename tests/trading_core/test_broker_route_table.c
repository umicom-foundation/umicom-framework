/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_broker_route_table.c
 *
 * PURPOSE:
 *   Exercise maintain candidate broker routes and select stable lowest-cost enabled routes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/broker_route_table.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingBrokerRouteTable t;
    UmiTradingBrokerRoute a,b;
    UmiFinancialId ra,rb,v;
    umi_trading_broker_route_table_init(&t);
    umi_trading_core_id_assign(&ra,"a");
    umi_trading_core_id_assign(&rb,"b");
    umi_trading_core_id_assign(&v,"v");
    umi_trading_broker_route_init(&a,&ra,&v,5U,10U,true);
    umi_trading_broker_route_init(&b,&rb,&v,3U,20U,true);
    umi_trading_broker_route_table_add(&t,&a);
    umi_trading_broker_route_table_add(&t,&b);
    return umi_trading_core_id_equal(&umi_trading_broker_route_table_best(&t)->route_id,&rb)?0:1;
}
