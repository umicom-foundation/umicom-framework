/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/listing_catalogue.c
 *
 * PURPOSE:
 *   Catalogue active venue listings without duplicating listing identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/listing_catalogue.h"

#include <string.h>
void umi_trading_listing_catalogue_init(UmiTradingListingCatalogue *catalogue){if(catalogue!=NULL)memset(catalogue,0,sizeof *catalogue);}
const UmiTradingInstrumentListing *umi_trading_listing_catalogue_find(const UmiTradingListingCatalogue *catalogue,const UmiFinancialId *listing_id){if(catalogue==NULL||listing_id==NULL)return NULL;for(size_t i=0;i<catalogue->count;i++)if(umi_trading_core_id_equal(&catalogue->items[i].listing_id,listing_id))return &catalogue->items[i];return NULL;}
UmiStatus umi_trading_listing_catalogue_add(UmiTradingListingCatalogue *catalogue,const UmiTradingInstrumentListing *listing){if(catalogue==NULL||!umi_trading_instrument_listing_valid(listing))return UMI_STATUS_INVALID_ARGUMENT;if(umi_trading_listing_catalogue_find(catalogue,&listing->listing_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;if(catalogue->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;catalogue->items[catalogue->count++]=*listing;return UMI_STATUS_OK;}
