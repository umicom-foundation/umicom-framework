/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/trade_capture.c
 *
 * PURPOSE:
 *   Create an execution-derived trade identifier string for downstream TMS integration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of trade capture. Product-specific UI and vendor details stay outside this file.
 */

#include <stdio.h>
#include "umicom/trading/trade_capture.h"
#include "umicom/trading/execution_report.h"

/*
 * Provide the trade capture reference operation used by this module and its client
 * applications.
 */
UmiStatus umi_trade_capture_reference(const UmiExecutionReport *report,
                                      char *buffer,
                                      size_t capacity)
{
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_execution_report_valid(report) ||
        buffer == NULL ||
        capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const int written =
        snprintf(buffer, capacity, "TRD-%s", report->execution_id.value);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}
