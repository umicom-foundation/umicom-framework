/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_event_log.c
 *
 * PURPOSE:
 *   Exercise maintain a bounded monotonic lifecycle event log per trading service.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_event_log.h"
int main(void) {

    UmiTradingOrderEventLog l;
     UmiTradingOrderEvent e;
     UmiFinancialId id;
     umi_trading_order_event_log_init(&l);
     umi_trading_core_id_assign(&id,"o");
    if(umi_trading_order_event_init(&e,&id,1U,10,UMI_TRADING_CORE_ORDER_OPEN)!=UMI_STATUS_OK)return 1;
    if(umi_trading_order_event_log_append(&l,&e)!=UMI_STATUS_OK)return 2;
    return umi_trading_order_event_log_latest(&l)->sequence==1U?0:3;
}
