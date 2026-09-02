/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/core/listing_catalogue.c
 *
 * PURPOSE:
 *   Catalogue active venue listings without duplicating listing identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/listing_catalogue.h"

#include <string.h>
/*
 * Initialise trading listing catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_trading_listing_catalogue_init(UmiTradingListingCatalogue *catalogue){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue!=NULL)memset(catalogue,0,sizeof *catalogue);}
/*
 * Find trading listing catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTradingInstrumentListing *umi_trading_listing_catalogue_find(const UmiTradingListingCatalogue *catalogue,const UmiFinancialId *listing_id){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||listing_id==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(size_t i=0;i<catalogue->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_core_id_equal(&catalogue->items[i].listing_id,listing_id))return &catalogue->items[i];return NULL;}
/*
 * Add trading listing catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_trading_listing_catalogue_add(UmiTradingListingCatalogue *catalogue,const UmiTradingInstrumentListing *listing){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue==NULL||!umi_trading_instrument_listing_valid(listing))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_trading_listing_catalogue_find(catalogue,&listing->listing_id)!=NULL)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(catalogue->count>=UMI_TRADING_CORE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;catalogue->items[catalogue->count++]=*listing;return UMI_STATUS_OK;}
