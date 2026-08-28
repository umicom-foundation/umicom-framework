/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/order_book_level.c
 *
 * PURPOSE:
 *   Represent aggregated price-time liquidity at one book level.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_book_level.h"
#include <string.h>
/* Initialise and validate represent aggregated price-time liquidity at one book level.. */
UmiStatus umi_trading_order_book_level_init(UmiTradingOrderBookLevel *value,UmiTradingPriceTicks price_ticks, UmiTradingQuantityLots quantity_lots, uint32_t order_count) {
    if(value==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value,0,sizeof *value);
    value->price_ticks=price_ticks;
    value->quantity_lots=quantity_lots;
    value->order_count=order_count;
    return umi_trading_order_book_level_valid(value)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
}
/* Validate the invariant set for this trading record. */
bool umi_trading_order_book_level_valid(const UmiTradingOrderBookLevel *value) { return value!=NULL && (value->price_ticks>0 && value->quantity_lots>=0 && (value->quantity_lots>0 || value->order_count==0U)); }
