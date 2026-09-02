/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/trading/core/venue_catalogue.h
 *
 * PURPOSE:
 *   Provide a bounded deterministic catalogue of execution venues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TRADING_CORE_VENUE_CATALOGUE_H
#define UMICOM_TRADING_CORE_VENUE_CATALOGUE_H
#include "umicom/trading/core/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/trading/core/venue_descriptor.h"
/**
 * Represent the trading venue catalogue data shared with callers of this public contract.
 */
typedef struct UmiTradingVenueCatalogue { UmiTradingVenueDescriptor items[UMI_TRADING_CORE_MAX_ITEMS]; size_t count; uint64_t revision; } UmiTradingVenueCatalogue;
/* Initialise an empty venue catalogue. */
void umi_trading_venue_catalogue_init(UmiTradingVenueCatalogue *catalogue);
/* Add one venue while rejecting duplicate venue identifiers. */
UmiStatus umi_trading_venue_catalogue_add(UmiTradingVenueCatalogue *catalogue,const UmiTradingVenueDescriptor *venue);
/* Locate a venue by its Framework identifier. */
const UmiTradingVenueDescriptor *umi_trading_venue_catalogue_find(const UmiTradingVenueCatalogue *catalogue,const UmiFinancialId *venue_id);
#ifdef __cplusplus
}
#endif
#endif
