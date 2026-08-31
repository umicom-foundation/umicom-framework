/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/workflow_report.c
 *
 * PURPOSE:
 *   Initialise repository workflow reports to a deterministic empty state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_report.h"

#include <string.h>

UmiStatus umi_repository_workflow_report_initialize(
    UmiRepositoryWorkflowReport *report,
    size_t caller_structure_size,
    UmiRepositoryWorkflowAction action)
{
    /* A size mismatch is rejected before the report allocation is touched. */
    if (report == NULL || caller_structure_size != sizeof(*report)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Initialise all status flags and the diagnostic buffer deterministically. */
    (void)memset(report, 0, sizeof(*report));
    report->structure_size = (uint32_t)sizeof(*report);
    report->api_version = UMI_REPOSITORY_WORKFLOW_REPORT_API_VERSION;
    report->action = action;
    report->status = UMI_STATUS_OK;
    report->last_exit_code = -1;
    return UMI_STATUS_OK;
}
