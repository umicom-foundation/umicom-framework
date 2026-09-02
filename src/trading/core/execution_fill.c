/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/execution_fill.c
 *
 * PURPOSE:
 *   Represent one integer-normalised venue fill for deterministic aggregation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/execution_fill.h"
#include <string.h>
/* Initialise and validate represent one integer-normalised venue fill for deterministic aggregation.. */
UmiStatus umi_trading_execution_fill_init(UmiTradingExecutionFill *value,const UmiFinancialId * execution_id, const UmiFinancialId * client_order_id, UmiTradingQuantityLots quantity_lots, UmiTradingPriceTicks price_ticks, int64_t event_time_ms) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(execution_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->execution_id=*execution_id;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(client_order_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->client_order_id=*client_order_id;
    value->quantity_lots=quantity_lots;
    value->price_ticks=price_ticks;
    value->event_time_ms=event_time_ms;
    return umi_trading_execution_fill_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_execution_fill_valid(const UmiTradingExecutionFill *value) { return value!=NULL && (value->execution_id.value[0]!='\0' && value->client_order_id.value[0]!='\0' && value->quantity_lots>0 && value->price_ticks>0 && value->event_time_ms>=0); }
