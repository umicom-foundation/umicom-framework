/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_simulated_broker.c
 *
 * PURPOSE:
 *   Validate simulated broker behaviour in the trading foundation.
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
#include "test_trading_common.h"
int main(void){
    UmiSimulatedBrokerStorage storage={0};UmiBroker b={0};assert(umi_simulated_broker_create(&storage,&b)==UMI_STATUS_OK);assert(umi_broker_contract_valid(&b));
    assert(b.connect(b.instance,UMI_TRADING_SIMULATION)==UMI_STATUS_OK);UmiOrderRequest r=test_order_request();assert(b.submit_order(b.instance,&r)==UMI_STATUS_OK);assert(b.cancel_order(b.instance,&r.client_order_id)==UMI_STATUS_OK);b.destroy(b.instance);return 0;
}
