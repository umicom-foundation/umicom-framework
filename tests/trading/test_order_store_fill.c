/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_order_store_fill.c
 *
 * PURPOSE:
 *   Validate order store fill behaviour in the trading foundation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include <stdio.h>
#include "test_trading_common.h"
int main(void){
    UmiOrderStore store;umi_order_store_init(&store);UmiOrder o={0};o.request=test_order_request();o.status=UMI_ORDER_ACCEPTED;o.version=1;
    assert(umi_order_store_add(&store,&o)==UMI_STATUS_OK);UmiOrder *stored=NULL;assert(umi_order_store_find(&store,"ORDER-1",&stored)==UMI_STATUS_OK);assert(stored!=NULL);
    UmiExecutionReport e={0};(void)snprintf(e.execution_id.value,sizeof(e.execution_id.value),"%s","EXEC-1");e.client_order_id=o.request.client_order_id;e.fill_quantity=1;e.fill_price=25001;e.event_time_ms=10;
    assert(umi_order_apply_execution(stored,&e)==UMI_STATUS_OK);assert(stored->status==UMI_ORDER_FILLED);assert(stored->average_fill_price==25001);
    return 0;
}
