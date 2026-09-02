/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/venue_catalogue.c
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
#include "umicom/trading/core/venue_catalogue.h"

#include <string.h>
/* Initialise an empty venue catalogue. */
void umi_trading_venue_catalogue_init(UmiTradingVenueCatalogue *catalogue){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue!=NULL)memset(catalogue,0,sizeof *catalogue);}
/* Locate a venue by its Framework identifier. */
const UmiTradingVenueDescriptor *umi_trading_venue_catalogue_find(const UmiTradingVenueCatalogue *catalogue,const UmiFinancialId *venue_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||venue_id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<catalogue->count;i++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&catalogue->items[i].venue_id,venue_id))return &catalogue->items[i];}return NULL;}
/* Add one venue while rejecting duplicate venue identifiers. */
UmiStatus umi_trading_venue_catalogue_add(UmiTradingVenueCatalogue *catalogue,const UmiTradingVenueDescriptor *venue){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||!umi_trading_venue_descriptor_valid(venue))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_venue_catalogue_find(catalogue,&venue->venue_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;catalogue->items[catalogue->count++]=*venue;catalogue->revision++;return UMI_STATUS_OK;}
