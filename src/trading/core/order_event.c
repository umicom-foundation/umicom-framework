/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_event.c
 *
 * PURPOSE:
 *   Capture sequence-ordered evidence for an order lifecycle transition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_event.h"
#include <string.h>
/* Initialise and validate capture sequence-ordered evidence for an order lifecycle transition.. */
UmiStatus umi_trading_order_event_init(UmiTradingOrderEvent *value,const UmiFinancialId * client_order_id, uint64_t sequence, int64_t event_time_ms, UmiTradingCoreOrderState state) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    if(client_order_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->client_order_id=*client_order_id;
    value->sequence=sequence;
    value->event_time_ms=event_time_ms;
    value->state=state;
    return umi_trading_order_event_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_order_event_valid(const UmiTradingOrderEvent *value) { return value!=NULL && (value->client_order_id.value[0]!='\0' && value->sequence>0U && value->event_time_ms>=0 && value->state>=UMI_TRADING_CORE_ORDER_PENDING_NEW && value->state<=UMI_TRADING_CORE_ORDER_EXPIRED); }
