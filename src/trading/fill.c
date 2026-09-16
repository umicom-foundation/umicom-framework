/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/fill.c
 *
 * PURPOSE:
 *   Apply an execution report to an order's fill state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of fill. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/fill.h"
#include "umicom/trading/execution_report.h"
#include "umicom/finance/identifier.h"
#include <math.h>

/*
 * Provide the order apply execution operation used by this module and its client
 * applications.
 */
UmiStatus umi_order_apply_execution(UmiOrder *order,
                                    const UmiExecutionReport *report)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (order == NULL ||
        !umi_execution_report_valid(report) ||
        !umi_financial_id_equal(&order->request.client_order_id,
                                &report->client_order_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if ((order->status != UMI_ORDER_ACCEPTED &&
         order->status != UMI_ORDER_PARTIALLY_FILLED) ||
        !isfinite(order->request.quantity) || order->request.quantity <= 0.0 ||
        !isfinite(order->filled_quantity) || order->filled_quantity < 0.0 ||
        !isfinite(order->average_fill_price) || order->average_fill_price < 0.0 ||
        order->version == UINT64_MAX)
        return UMI_STATUS_INVALID_STATE;
    UmiOrder candidate = *order;

    const double old_filled = order->filled_quantity;
    const double total_filled = old_filled + report->fill_quantity;

    /* Apply this branch only when its contract condition is satisfied. */
    if (!isfinite(total_filled) || total_filled > order->request.quantity + 1.0e-9) {
        return UMI_STATUS_INVALID_STATE;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (total_filled > 0.0) {
        const double old_price = order->average_fill_price;
        const double price = report->fill_price;
        candidate.average_fill_price = old_filled == 0.0 ? price :
            (old_price <= price
                ? old_price + (price - old_price) * (report->fill_quantity / total_filled)
                : price + (old_price - price) * (old_filled / total_filled));
    }
    if (!isfinite(candidate.average_fill_price)) return UMI_STATUS_CAPACITY_EXCEEDED;
    candidate.filled_quantity = total_filled;
    candidate.status = total_filled + 1.0e-9 >= order->request.quantity
        ? UMI_ORDER_FILLED : UMI_ORDER_PARTIALLY_FILLED;
    candidate.version++;
    *order = candidate;

    return UMI_STATUS_OK;
}
