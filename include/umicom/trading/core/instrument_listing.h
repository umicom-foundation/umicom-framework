/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/instrument_listing.h
 *
 * PURPOSE:
 *   Bind a canonical instrument to a venue listing and trading increments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_INSTRUMENT_LISTING_H
#define UMICOM_TRADING_CORE_INSTRUMENT_LISTING_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiTradingInstrumentListing { UmiFinancialId listing_id; UmiInstrument instrument; UmiFinancialId venue_id; UmiTradingPriceTicks tick_size; UmiTradingQuantityLots lot_size; bool active; } UmiTradingInstrumentListing;
/* Initialise a venue-specific instrument listing. */
UmiStatus umi_trading_instrument_listing_init(UmiTradingInstrumentListing *listing,const char *listing_id,const UmiInstrument *instrument,const UmiFinancialId *venue_id,UmiTradingPriceTicks tick_size,UmiTradingQuantityLots lot_size);
/* Validate listing identity and positive trading increments. */
bool umi_trading_instrument_listing_valid(const UmiTradingInstrumentListing *listing);
#ifdef __cplusplus
}
#endif
#endif
