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

UmiStatus umi_order_apply_execution(UmiOrder *order,
                                    const UmiExecutionReport *report)
{
    if (order == NULL ||
        !umi_execution_report_valid(report) ||
        !umi_financial_id_equal(&order->request.client_order_id,
                                &report->client_order_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const double old_filled = order->filled_quantity;
    const double total_filled = old_filled + report->fill_quantity;

    if (total_filled > order->request.quantity + 1.0e-9) {
        return UMI_STATUS_INVALID_STATE;
    }

    if (total_filled > 0.0) {
        order->average_fill_price =
            ((order->average_fill_price * old_filled) +
             (report->fill_price * report->fill_quantity)) /
            total_filled;
    }

    order->filled_quantity = total_filled;
    order->status =
        total_filled + 1.0e-9 >= order->request.quantity
            ? UMI_ORDER_FILLED
            : UMI_ORDER_PARTIALLY_FILLED;
    order->version++;

    return UMI_STATUS_OK;
}
