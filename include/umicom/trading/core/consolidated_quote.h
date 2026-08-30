/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/consolidated_quote.h
 *
 * PURPOSE:
 *   Represent a venue quote using integer ticks and lots for deterministic consolidation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_CONSOLIDATED_QUOTE_H
#define UMICOM_TRADING_CORE_CONSOLIDATED_QUOTE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingConsolidatedQuote { UmiFinancialId instrument_id; UmiFinancialId venue_id; UmiTradingPriceTicks bid_ticks; UmiTradingQuantityLots bid_lots; UmiTradingPriceTicks ask_ticks; UmiTradingQuantityLots ask_lots; int64_t event_time_ms; } UmiTradingConsolidatedQuote;
/* Initialise and validate an integer-normalised quote. */
UmiStatus umi_trading_consolidated_quote_init(UmiTradingConsolidatedQuote *quote,const UmiFinancialId *instrument_id,const UmiFinancialId *venue_id,UmiTradingPriceTicks bid,UmiTradingQuantityLots bid_lots,UmiTradingPriceTicks ask,UmiTradingQuantityLots ask_lots,int64_t event_time_ms);
/* Validate price ordering and positive displayed size. */
bool umi_trading_consolidated_quote_valid(const UmiTradingConsolidatedQuote *quote);
#ifdef __cplusplus
}
#endif
#endif
