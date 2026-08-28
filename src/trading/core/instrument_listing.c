/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/instrument_listing.c
 *
 * PURPOSE:
 *   Bind a canonical instrument to a venue listing and trading increments.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/instrument_listing.h"

#include <string.h>
/* Initialise a venue-specific instrument listing. */
UmiStatus umi_trading_instrument_listing_init(UmiTradingInstrumentListing *listing,const char *listing_id,const UmiInstrument *instrument,const UmiFinancialId *venue_id,UmiTradingPriceTicks tick_size,UmiTradingQuantityLots lot_size){if(listing==NULL||instrument==NULL||venue_id==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(listing,0,sizeof *listing);if(umi_trading_core_id_assign(&listing->listing_id,listing_id)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;listing->instrument=*instrument;listing->venue_id=*venue_id;listing->tick_size=tick_size;listing->lot_size=lot_size;listing->active=true;return umi_trading_instrument_listing_valid(listing)?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;}
/* Validate listing identity and positive trading increments. */
bool umi_trading_instrument_listing_valid(const UmiTradingInstrumentListing *listing){return listing!=NULL&&listing->listing_id.value[0]!='\0'&&listing->venue_id.value[0]!='\0'&&listing->instrument.instrument_id.value[0]!='\0'&&listing->tick_size>0&&listing->lot_size>0;}
