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
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>
#include "umicom/broker_connectivity/paper_runtime.h"
#include "../trading/test_trading_common.h"

int main(void)
{
    UmiPaperBrokerRuntime runtime;
    UmiOrderRequest request = {0};

    assert(umi_paper_broker_runtime_init(&runtime) == UMI_STATUS_OK);
    assert(umi_paper_broker_runtime_connect(&runtime, 1000U) == UMI_STATUS_OK);

    /* The earlier fixture omitted account and instrument identity. It is kept
     * for review because it explains why the shared validator refused submission.
     * Reuse the trading fixture so this integration test follows the same public
     * order contract as direct simulated-broker callers. */
#if 0
    (void)strcpy(request.client_order_id.value, "paper-1");
    request.side = UMI_SIDE_BUY;
    request.type = UMI_ORDER_LIMIT;
    request.tif = UMI_TIF_DAY;
    request.quantity = 1.0;
    request.limit_price = 100.0;
    request.environment = UMI_TRADING_PAPER;

#endif
    request = test_order_request();
    (void)strcpy(request.client_order_id.value, "paper-1");
    request.limit_price = 100.0;
    request.environment = UMI_TRADING_PAPER;
    assert(umi_order_request_validate(&request) == UMI_STATUS_OK);

    /* Refusal must leave both retained order stores and audit sequence intact.
     * An incomplete fixture must never be made valid by bypassing the adapter. */
    UmiOrderRequest invalid = request;
    invalid.account_id.value[0] = '\0';
    const size_t auditCount = runtime.audit.count;
    const uint64_t auditSequence = runtime.nextAuditSequence;
    assert(umi_paper_broker_runtime_submit(&runtime, &invalid, 1050U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    invalid = request;
    invalid.instrument = (UmiInstrument){0};
    assert(umi_paper_broker_runtime_submit(&runtime, &invalid, 1050U) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(runtime.orders.count == 0U && runtime.storage.orders.count == 0U);
    assert(runtime.audit.count == auditCount);
    assert(runtime.nextAuditSequence == auditSequence);

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
