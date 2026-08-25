/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_order_identity.c
 *
 * PURPOSE:
 *   Exercise provide stable client, broker and venue identifiers for one order lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/trading/core/order_identity.h"
int main(void) {

    UmiTradingOrderIdentity id;
     if(umi_trading_order_identity_init(&id,"c1")!=UMI_STATUS_OK)return 1;
    if(umi_trading_order_identity_set_broker(&id,"b1")!=UMI_STATUS_OK)return 2;
    if(umi_trading_order_identity_set_venue(&id,"v1")!=UMI_STATUS_OK)return 3;
    return id.venue_order_id.value[0]=='v'?0:4;
}
