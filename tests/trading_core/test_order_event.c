/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_event.c
 *
 * PURPOSE:
 *   Exercise capture sequence-ordered evidence for an order lifecycle transition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_event.h"
int main(void) {
    UmiFinancialId id;
     if(umi_trading_core_id_assign(&id,"o")!=UMI_STATUS_OK)return 9;
     UmiTradingOrderEvent v;
     if(umi_trading_order_event_init(&v,&id,1U,1000,UMI_TRADING_CORE_ORDER_OPEN)!=UMI_STATUS_OK) return 1;
     if(!umi_trading_order_event_valid(&v)) return 2;
     return 0;
}
