/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/trade_capture.c
 *
 * PURPOSE:
 *   Create an execution-derived trade identifier string for downstream TMS integration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of trade capture. Product-specific UI and vendor details stay outside this file.
 */

#include <stdio.h>
#include "umicom/trading/trade_capture.h"
#include "umicom/trading/execution_report.h"

UmiStatus umi_trade_capture_reference(const UmiExecutionReport *report,
                                      char *buffer,
                                      size_t capacity)
{
    if (!umi_execution_report_valid(report) ||
        buffer == NULL ||
        capacity == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const int written =
        snprintf(buffer, capacity, "TRD-%s", report->execution_id.value);
    if (written < 0 || (size_t)written >= capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    return UMI_STATUS_OK;
}
