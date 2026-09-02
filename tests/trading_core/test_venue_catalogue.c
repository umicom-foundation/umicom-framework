/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_catalogue.c
 *
 * PURPOSE:
 *   Exercise provide a bounded deterministic catalogue of execution venues.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingVenueCatalogue c;
     UmiTradingVenueDescriptor v;
    umi_trading_venue_catalogue_init(&c);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_venue_descriptor_init(&v,"v1","XNAS","Nasdaq",true,true,1U)!=UMI_STATUS_OK)return 1;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(umi_trading_venue_catalogue_add(&c,&v)!=UMI_STATUS_OK||c.count!=1U)return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_trading_venue_catalogue_find(&c,&v.venue_id)==NULL)return 3;
    return 0;
}
