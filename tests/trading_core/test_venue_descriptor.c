/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_descriptor.c
 *
 * PURPOSE:
 *   Exercise define exchange and execution-venue identity and capabilities.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_descriptor.h"
int main(void) {

    UmiTradingVenueDescriptor v;
    if(umi_trading_venue_descriptor_init(&v,"venue-lse","XLON","London",true,false,10U)!=UMI_STATUS_OK)return 1;
    if(!umi_trading_venue_descriptor_valid(&v)||!v.supports_auctions)return 2;
    return 0;
}
