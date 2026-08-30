/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/consolidated_trade.c
 *
 * PURPOSE:
 *   Represent a venue trade print with integer-normalised price and quantity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/consolidated_trade.h"
#include <string.h>
/* Initialise and validate represent a venue trade print with integer-normalised price and quantity.. */
UmiStatus umi_trading_consolidated_trade_init(UmiTradingConsolidatedTrade *value,const UmiFinancialId * instrument_id, const UmiFinancialId * venue_id, UmiTradingPriceTicks price_ticks, UmiTradingQuantityLots quantity_lots, int64_t event_time_ms) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    if(instrument_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->instrument_id=*instrument_id;
    if(venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    value->venue_id=*venue_id;
    value->price_ticks=price_ticks;
    value->quantity_lots=quantity_lots;
    value->event_time_ms=event_time_ms;
    return umi_trading_consolidated_trade_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_consolidated_trade_valid(const UmiTradingConsolidatedTrade *value) { return value!=NULL && (value->instrument_id.value[0]!='\0' && value->venue_id.value[0]!='\0' && value->price_ticks>0 && value->quantity_lots>0 && value->event_time_ms>=0); }
