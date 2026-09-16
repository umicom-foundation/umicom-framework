/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/execution_report.c
 *
 * PURPOSE:
 *   Validate fill/execution reports.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of execution report. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/execution_report.h"
#include "umicom/finance/identifier.h"
#include <math.h>
/* Check that execution report satisfies its contract before another service relies on it. */
int umi_execution_report_valid(const UmiExecutionReport *r)
{
    return r != NULL && umi_financial_id_valid(&r->execution_id) &&
        umi_financial_id_valid(&r->client_order_id) &&
        isfinite(r->fill_quantity) && r->fill_quantity > 0.0 &&
        isfinite(r->fill_price) && r->fill_price > 0.0 && r->event_time_ms >= 0;
}

/* Exact economic identity is required for a harmless replay. In particular,
 * floating-point fields are not compared with a price tolerance here. */
int UmiExecutionReportEqual(const UmiExecutionReport *left,
                            const UmiExecutionReport *right)
{
    return umi_execution_report_valid(left) && umi_execution_report_valid(right) &&
        umi_financial_id_equal(&left->execution_id, &right->execution_id) &&
        umi_financial_id_equal(&left->client_order_id, &right->client_order_id) &&
        left->fill_quantity == right->fill_quantity &&
        left->fill_price == right->fill_price &&
        left->event_time_ms == right->event_time_ms;
}
