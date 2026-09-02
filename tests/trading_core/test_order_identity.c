/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_identity.c
 *
 * PURPOSE:
 *   Exercise provide stable client, broker and venue identifiers for one order lifecycle.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_identity.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {

    UmiTradingOrderIdentity id;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_order_identity_init(&id,"c1")!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_order_identity_set_broker(&id,"b1")!=UMI_STATUS_OK)return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_trading_order_identity_set_venue(&id,"v1")!=UMI_STATUS_OK)return 3;
    return id.venue_order_id.value[0]=='v'?0:4;
}
