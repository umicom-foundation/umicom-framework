/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_catalogue.c
 *
 * PURPOSE:
 *   Exercise provide a bounded deterministic catalogue of execution venues.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_catalogue.h"
int main(void) {

    UmiTradingVenueCatalogue c;
     UmiTradingVenueDescriptor v;
    umi_trading_venue_catalogue_init(&c);
    if(umi_trading_venue_descriptor_init(&v,"v1","XNAS","Nasdaq",true,true,1U)!=UMI_STATUS_OK)return 1;
    if(umi_trading_venue_catalogue_add(&c,&v)!=UMI_STATUS_OK||c.count!=1U)return 2;
    if(umi_trading_venue_catalogue_find(&c,&v.venue_id)==NULL)return 3;
    return 0;
}
