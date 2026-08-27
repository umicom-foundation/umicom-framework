/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/acceptance/report.c
 *
 * PURPOSE:
 *   Implement acceptance report totals and completion percentage over focused gate results.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/acceptance/report.h"
#include <string.h>
UmiStatus umi_developer_acceptance_report_build(
    const UmiDeveloperAcceptanceGateResult *results,
    size_t result_count,
    UmiDeveloperAcceptanceReport *out_report)
{
    size_t index;
    if ((result_count > 0U && results == NULL) || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->gate_count = result_count;
    for (index = 0U; index < result_count; ++index) {
        if (results[index].passed)
            out_report->passed_gate_count += 1U;
        else
            out_report->failed_gate_count += 1U;
        out_report->missing_requirement_count += results[index].missing_count;
    }
    if (result_count > 0U)
        out_report->completion_percent =
            (unsigned)((out_report->passed_gate_count * 100U) / result_count);
    return UMI_STATUS_OK;
}
