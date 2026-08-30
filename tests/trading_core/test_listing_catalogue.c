/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_listing_catalogue.c
 *
 * PURPOSE:
 *   Exercise catalogue active venue listings without duplicating listing identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/listing_catalogue.h"
int main(void) {

    UmiTradingListingCatalogue c;
     UmiTradingInstrumentListing l;
     UmiInstrument i={0};
     UmiFinancialId v;
    umi_trading_listing_catalogue_init(&c);
     umi_trading_core_id_assign(&i.instrument_id,"i");
     umi_trading_core_id_assign(&v,"v");
    if(umi_trading_instrument_listing_init(&l,"l",&i,&v,1,1)!=UMI_STATUS_OK)return 1;
    if(umi_trading_listing_catalogue_add(&c,&l)!=UMI_STATUS_OK)return 2;
    return umi_trading_listing_catalogue_find(&c,&l.listing_id)!=NULL?0:3;
}
