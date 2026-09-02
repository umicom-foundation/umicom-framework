/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/acceptance/report.c
 *
 * PURPOSE:
 *   Implement acceptance report totals and completion percentage over focused gate results.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/developer/acceptance/report.h"
#include <string.h>
/*
 * Provide the developer acceptance report build operation used by this module and its
 * client applications.
 */
UmiStatus umi_developer_acceptance_report_build(
    const UmiDeveloperAcceptanceGateResult *results,
    size_t result_count,
    UmiDeveloperAcceptanceReport *out_report)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if ((result_count > 0U && results == NULL) || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_report, 0, sizeof(*out_report));
    out_report->gate_count = result_count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < result_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (results[index].passed)
            out_report->passed_gate_count += 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else
            out_report->failed_gate_count += 1U;
        out_report->missing_requirement_count += results[index].missing_count;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result_count > 0U)
        out_report->completion_percent =
            (unsigned)((out_report->passed_gate_count * 100U) / result_count);
    return UMI_STATUS_OK;
}
