/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_execution_reconciler.c
 *
 * PURPOSE:
 *   Verify duplicate, correction and late-execution handling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/broker_connectivity/execution_reconciler.h"

static UmiExecutionReport report(const char *id, double price, int64_t time)
{
    UmiExecutionReport value = {0};
    (void)strcpy(value.execution_id.value, id);
    (void)strcpy(value.client_order_id.value, "order-1");
    value.fill_quantity = 1.0;
    value.fill_price = price;
    value.event_time_ms = time;
    return value;
}

int main(void)
{
    UmiBrokerExecutionReconciler reconciler;
    UmiExecutionReport one = report("exec-1", 100.0, 1000);
    UmiExecutionReport corrected = report("exec-1", 100.25, 1000);
    UmiExecutionReport late = report("exec-2", 99.0, 900);

    umi_broker_execution_reconciler_init(&reconciler);
    assert(umi_broker_execution_reconciler_accept(&reconciler, &one) ==
           UMI_STATUS_OK);
    assert(umi_broker_execution_reconciler_accept(&reconciler, &one) ==
           UMI_STATUS_OK);
    assert(reconciler.duplicates == 1U);
    assert(umi_broker_execution_reconciler_accept(&reconciler, &corrected) ==
           UMI_STATUS_OK);
    assert(reconciler.corrections == 1U);
    assert(umi_broker_execution_reconciler_accept(&reconciler, &late) ==
           UMI_STATUS_OK);
    assert(reconciler.lateReports == 1U);
    return 0;
}
