/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/venue_catalogue.c
 *
 * PURPOSE:
 *   Provide a bounded deterministic catalogue of execution venues.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_catalogue.h"

#include <string.h>
/* Initialise an empty venue catalogue. */
void umi_trading_venue_catalogue_init(UmiTradingVenueCatalogue *catalogue){if(catalogue!=NULL)memset(catalogue,0,sizeof *catalogue);}
/* Locate a venue by its Framework identifier. */
const UmiTradingVenueDescriptor *umi_trading_venue_catalogue_find(const UmiTradingVenueCatalogue *catalogue,const UmiFinancialId *venue_id){if(catalogue==NULL||venue_id==NULL)return NULL;for(size_t i=0;i<catalogue->count;i++){if(umi_trading_core_id_equal(&catalogue->items[i].venue_id,venue_id))return &catalogue->items[i];}return NULL;}
/* Add one venue while rejecting duplicate venue identifiers. */
UmiStatus umi_trading_venue_catalogue_add(UmiTradingVenueCatalogue *catalogue,const UmiTradingVenueDescriptor *venue){if(catalogue==NULL||!umi_trading_venue_descriptor_valid(venue))return UMI_STATUS_INVALID_ARGUMENT;if(umi_trading_venue_catalogue_find(catalogue,&venue->venue_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(catalogue->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;catalogue->items[catalogue->count++]=*venue;catalogue->revision++;return UMI_STATUS_OK;}
