/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_oms.c
 *
 * PURPOSE:
 *   Validate oms behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include "test_trading_common.h"
int main(void){
    UmiOms o;UmiRiskLimit l={5,1000000,10,5000};umi_oms_init(&o,l);UmiOrderRequest r=test_order_request();UmiRiskDecision d={0};
    assert(umi_oms_submit(&o,&r,0,0,100,&d)==UMI_STATUS_OK);assert(d.allowed);assert(o.orders.count==1U);
    umi_kill_switch_engage(&o.kill_switch,"operator");(void)snprintf(r.client_order_id.value,sizeof(r.client_order_id.value),"%s","ORDER-2");assert(umi_oms_submit(&o,&r,0,0,200,&d)==UMI_STATUS_PERMISSION_DENIED);return 0;
}
