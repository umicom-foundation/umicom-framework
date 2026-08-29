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

void umi_repository_workflow_report_init(
    UmiRepositoryWorkflowReport *report,
    UmiRepositoryWorkflowAction action)
{
    if (report == NULL) return;
    (void)memset(report, 0, sizeof(*report));
    report->action = action;
    report->status = UMI_STATUS_OK;
    report->last_exit_code = -1;
}
