/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_instrument_listing.c
 *
 * PURPOSE:
 *   Exercise bind a canonical instrument to a venue listing and trading increments.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/instrument_listing.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiInstrument i={0};
     UmiFinancialId v;
     UmiTradingInstrumentListing l;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_core_id_assign(&i.instrument_id,"inst")!=UMI_STATUS_OK||umi_trading_core_id_assign(&v,"venue")!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_instrument_listing_init(&l,"list",&i,&v,1,100)!=UMI_STATUS_OK)return 2;
    return umi_trading_instrument_listing_valid(&l)?0:3;
}
