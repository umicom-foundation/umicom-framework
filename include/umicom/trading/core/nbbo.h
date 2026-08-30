/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/nbbo.h
 *
 * PURPOSE:
 *   Calculate a national-best-style bid and offer across bounded venue quotes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_NBBO_H
#define UMICOM_TRADING_CORE_NBBO_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/consolidated_quote.h"
typedef struct UmiTradingNbbo { UmiTradingPriceTicks best_bid_ticks; UmiTradingQuantityLots best_bid_lots; UmiFinancialId bid_venue_id; UmiTradingPriceTicks best_ask_ticks; UmiTradingQuantityLots best_ask_lots; UmiFinancialId ask_venue_id; bool valid; } UmiTradingNbbo;
/* Consolidate the highest bid and lowest ask across valid quotes. */
UmiStatus umi_trading_nbbo_compute(const UmiTradingConsolidatedQuote *quotes,size_t count,UmiTradingNbbo *out_nbbo);
#ifdef __cplusplus
}
#endif
#endif
