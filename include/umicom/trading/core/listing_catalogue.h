/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/listing_catalogue.h
 *
 * PURPOSE:
 *   Catalogue active venue listings without duplicating listing identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_LISTING_CATALOGUE_H
#define UMICOM_TRADING_CORE_LISTING_CATALOGUE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/instrument_listing.h"
typedef struct UmiTradingListingCatalogue { UmiTradingInstrumentListing items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; } UmiTradingListingCatalogue;
/* Initialise an empty listing catalogue. */
void umi_trading_listing_catalogue_init(UmiTradingListingCatalogue *catalogue);
/* Add a unique listing. */
UmiStatus umi_trading_listing_catalogue_add(UmiTradingListingCatalogue *catalogue,const UmiTradingInstrumentListing *listing);
/* Find a listing by identifier. */
const UmiTradingInstrumentListing *umi_trading_listing_catalogue_find(const UmiTradingListingCatalogue *catalogue,const UmiFinancialId *listing_id);
#ifdef __cplusplus
}
#endif
#endif
