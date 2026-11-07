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

/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include "test_trading_common.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The original single happy-path case is preserved for comparison. The expanded
 * scenario below calls the same public callbacks and observes refused mutations. */
#if 0
int main(void){
    UmiSimulatedBrokerStorage storage={0};UmiBroker b={0};assert(umi_simulated_broker_create(&storage,&b)==UMI_STATUS_OK);assert(umi_broker_contract_valid(&b));
    assert(b.connect(b.instance,UMI_TRADING_SIMULATION)==UMI_STATUS_OK);UmiOrderRequest r=test_order_request();assert(b.submit_order(b.instance,&r)==UMI_STATUS_OK);assert(b.cancel_order(b.instance,&r.client_order_id)==UMI_STATUS_OK);b.destroy(b.instance);return 0;
}

#endif

#include <math.h>
#include <stdint.h>
#define CHECK(condition) do { if (!(condition)) {     fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } } while (0)

int main(void)
{
    UmiSimulatedBrokerStorage storage = {0}, before = {0};
    UmiBroker broker = {0};
    UmiOrderRequest request = test_order_request();
    UmiFinancialId invalid = {0};
    CHECK(umi_simulated_broker_create(&storage, &broker) == UMI_STATUS_OK);
    CHECK(umi_broker_contract_valid(&broker));
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_INVALID_STATE);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_INVALID_STATE);
    memcpy(&before, &storage, sizeof before);
    CHECK(broker.connect(broker.instance, UMI_TRADING_LIVE) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(broker.connect(broker.instance, (UmiTradingEnvironment)99) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &storage, sizeof before) == 0);
    CHECK(broker.connect(broker.instance, UMI_TRADING_SIMULATION) == UMI_STATUS_OK);
    memcpy(&before, &storage, sizeof before);
    request.quantity = NAN;
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_INVALID_ARGUMENT);
    request = test_order_request();
    memset(request.account_id.value, 'x', sizeof request.account_id.value);
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_INVALID_ARGUMENT);
    request = test_order_request();
    request.environment = UMI_TRADING_PAPER;
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_PERMISSION_DENIED);
    request.environment = UMI_TRADING_LIVE;
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&before, &storage, sizeof before) == 0);
    request = test_order_request();
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_OK);
    CHECK(storage.orders.count == 1U && storage.orders.orders[0].version == 1U);
    memcpy(&before, &storage, sizeof before);
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(broker.connect(broker.instance, UMI_TRADING_PAPER) == UMI_STATUS_INVALID_STATE);
    CHECK(broker.connect(broker.instance, UMI_TRADING_SIMULATION) == UMI_STATUS_OK);
    memset(invalid.value, 'x', sizeof invalid.value);
    CHECK(broker.cancel_order(broker.instance, &invalid) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &storage, sizeof before) == 0);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_OK);
    CHECK(storage.orders.orders[0].status == UMI_ORDER_CANCELLED);
    CHECK(storage.orders.orders[0].version == 2U);
    memcpy(&before, &storage, sizeof before);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_OK);
    CHECK(memcmp(&before, &storage, sizeof before) == 0);

    const UmiOrderStatus refused[] = {UMI_ORDER_NEW, UMI_ORDER_VALIDATED,
        UMI_ORDER_FILLED, UMI_ORDER_REJECTED, (UmiOrderStatus)99};
    for (size_t i = 0U; i < sizeof refused / sizeof refused[0]; ++i) {
        storage.orders.orders[0].status = refused[i];
        memcpy(&before, &storage, sizeof before);
        CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&before, &storage, sizeof before) == 0);
    }
    storage.orders.orders[0].status = UMI_ORDER_PARTIALLY_FILLED;
    storage.orders.orders[0].filled_quantity = 0.25;
    storage.orders.orders[0].version = UINT64_MAX;
    memcpy(&before, &storage, sizeof before);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&before, &storage, sizeof before) == 0);
    storage.orders.orders[0].version = UINT64_MAX - 1U;
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_OK);
    CHECK(storage.orders.orders[0].version == UINT64_MAX);
    CHECK(storage.orders.orders[0].filled_quantity == 0.25);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_OK);
    CHECK(storage.orders.orders[0].version == UINT64_MAX);
    storage.connected = 0;
    memcpy(&before, &storage, sizeof before);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&before, &storage, sizeof before) == 0);
    CHECK(broker.connect(broker.instance, UMI_TRADING_SIMULATION) == UMI_STATUS_OK);
    storage.orders.count = SIZE_MAX;
    memcpy(&before, &storage, sizeof before);
    CHECK(broker.connect(broker.instance, UMI_TRADING_SIMULATION) == UMI_STATUS_INVALID_STATE);
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_INVALID_STATE);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&before, &storage, sizeof before) == 0);
    broker.destroy(broker.instance);
    CHECK(!storage.connected && storage.orders.count == 0U);
    CHECK(broker.connect(broker.instance, UMI_TRADING_PAPER) == UMI_STATUS_OK);
    request.environment = UMI_TRADING_PAPER;
    CHECK(broker.submit_order(broker.instance, &request) == UMI_STATUS_OK);
    CHECK(broker.cancel_order(broker.instance, &request.client_order_id) == UMI_STATUS_OK);
    broker.destroy(broker.instance);
    puts("Simulated broker lifecycle boundaries checked.");
    return 0;
}
