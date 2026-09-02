/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_venue_descriptor.c
 *
 * PURPOSE:
 *   Exercise define exchange and execution-venue identity and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/venue_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingVenueDescriptor v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_venue_descriptor_init(&v,"venue-lse","XLON","London",true,false,10U)!=UMI_STATUS_OK)return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_trading_venue_descriptor_valid(&v)||!v.supports_auctions)return 2;
    return 0;
}
