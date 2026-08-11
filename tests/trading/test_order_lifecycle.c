/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_order_lifecycle.c
 *
 * PURPOSE:
 *   Validate order lifecycle behaviour in the trading foundation.
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
    UmiOrderRequest r=test_order_request();assert(umi_order_request_validate(&r)==UMI_STATUS_OK);
    assert(umi_order_transition_allowed(UMI_ORDER_NEW,UMI_ORDER_VALIDATED));
    assert(umi_order_transition_allowed(UMI_ORDER_ACCEPTED,UMI_ORDER_PARTIALLY_FILLED));
    assert(!umi_order_transition_allowed(UMI_ORDER_FILLED,UMI_ORDER_ACCEPTED));
    assert(umi_order_validate_for_market(&r,UMI_MARKET_OPEN)==UMI_STATUS_OK);
    return 0;
}
