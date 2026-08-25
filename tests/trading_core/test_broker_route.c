/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_broker_route.c
 *
 * PURPOSE:
 *   Exercise describe a candidate broker/venue route with cost and latency scores.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/broker_route.h"
int main(void) {
    UmiFinancialId rid,vid;
    umi_trading_core_id_assign(&rid,"r");
    umi_trading_core_id_assign(&vid,"v");
     UmiTradingBrokerRoute v;
     if(umi_trading_broker_route_init(&v,&rid,&vid,5U,10U,true)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_broker_route_valid(&v)) return 2;
     return 0;
}
