/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_execution_capture.c
 *
 * PURPOSE:
 *   Validate execution capture behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "umicom/trading/trading.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){
    UmiExecutionReport e={0};(void)snprintf(e.execution_id.value,sizeof(e.execution_id.value),"%s","E-42");(void)snprintf(e.client_order_id.value,sizeof(e.client_order_id.value),"%s","O-1");e.fill_quantity=2;e.fill_price=100;e.event_time_ms=1;
    UmiExecutionStore s;umi_execution_store_init(&s);assert(umi_execution_store_add(&s,&e)==UMI_STATUS_OK);char ref[64];assert(umi_trade_capture_reference(&e,ref,sizeof(ref))==UMI_STATUS_OK);assert(strcmp(ref,"TRD-E-42")==0);return 0;
}
