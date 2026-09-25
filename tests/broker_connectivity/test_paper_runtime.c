/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_paper_runtime.c
 *
 * PURPOSE:
 *   Verify provider-neutral paper broker connect, submit, cancel and audit flow.
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
#include "umicom/broker_connectivity/paper_runtime.h"

int main(void)
{
    UmiPaperBrokerRuntime runtime;
    UmiOrderRequest request = {0};

    assert(umi_paper_broker_runtime_init(&runtime) == UMI_STATUS_OK);
    assert(umi_paper_broker_runtime_connect(&runtime, 1000U) == UMI_STATUS_OK);

    (void)strcpy(request.client_order_id.value, "paper-1");
    request.side = UMI_SIDE_BUY;
    request.type = UMI_ORDER_LIMIT;
    request.tif = UMI_TIF_DAY;
    request.quantity = 1.0;
    request.limit_price = 100.0;
    request.environment = UMI_TRADING_PAPER;

    assert(umi_paper_broker_runtime_submit(&runtime, &request, 1100U) ==
           UMI_STATUS_OK);
    assert(runtime.orders.count == 1U);
    assert(umi_paper_broker_runtime_cancel(
               &runtime, &request.client_order_id, 1200U) == UMI_STATUS_OK);
    assert(runtime.audit.count >= 3U);

    request.environment = UMI_TRADING_LIVE;
    (void)strcpy(request.client_order_id.value, "live-1");
    assert(umi_paper_broker_runtime_submit(&runtime, &request, 1300U) ==
           UMI_STATUS_PERMISSION_DENIED);

    umi_paper_broker_runtime_destroy(&runtime);
    return 0;
}
