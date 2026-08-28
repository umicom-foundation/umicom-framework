/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/consolidated_quote.c
 *
 * PURPOSE:
 *   Represent a venue quote using integer ticks and lots for deterministic consolidation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/consolidated_quote.h"

#include <string.h>
UmiStatus umi_trading_consolidated_quote_init(UmiTradingConsolidatedQuote *quote,const UmiFinancialId *instrument_id,const UmiFinancialId *venue_id,UmiTradingPriceTicks bid,UmiTradingQuantityLots bid_lots,UmiTradingPriceTicks ask,UmiTradingQuantityLots ask_lots,int64_t event_time_ms){if(quote==NULL||instrument_id==NULL||venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(quote,0,sizeof *quote);quote->instrument_id=*instrument_id;quote->venue_id=*venue_id;quote->bid_ticks=bid;quote->bid_lots=bid_lots;quote->ask_ticks=ask;quote->ask_lots=ask_lots;quote->event_time_ms=event_time_ms;return umi_trading_consolidated_quote_valid(quote)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
bool umi_trading_consolidated_quote_valid(const UmiTradingConsolidatedQuote *quote){return quote!=NULL&&quote->instrument_id.value[0]!='\0'&&quote->venue_id.value[0]!='\0'&&quote->bid_ticks>0&&quote->ask_ticks>=quote->bid_ticks&&quote->bid_lots>0&&quote->ask_lots>0&&quote->event_time_ms>=0;}
