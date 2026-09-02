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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingListingCatalogue c;
     UmiTradingInstrumentListing l;
     UmiInstrument i={0};
     UmiFinancialId v;
    umi_trading_listing_catalogue_init(&c);
     umi_trading_core_id_assign(&i.instrument_id,"i");
     umi_trading_core_id_assign(&v,"v");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_instrument_listing_init(&l,"l",&i,&v,1,1)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_listing_catalogue_add(&c,&l)!=UMI_STATUS_OK)return 2;
    return umi_trading_listing_catalogue_find(&c,&l.listing_id)!=NULL?0:3;
}
