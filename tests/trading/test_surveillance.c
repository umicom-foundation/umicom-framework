/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_surveillance.c
 *
 * PURPOSE:
 *   Validate surveillance behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "test_trading_common.h"
int main(void){
    UmiOrderRequest a=test_order_request(),b=test_order_request();b.side=UMI_SIDE_SELL;(void)snprintf(b.client_order_id.value,sizeof(b.client_order_id.value),"%s","ORDER-2");
    assert(umi_self_trade_risk(&a,&b));assert(umi_surveillance_large_order(&a,1));assert(umi_price_within_band(101,100,0.02));return 0;
}
