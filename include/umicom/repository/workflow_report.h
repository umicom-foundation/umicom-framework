/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/workflow_report.h
 *
 * PURPOSE:
 *   Report exactly what a repository workflow planned or completed, including
 *   the final Git output needed to diagnose a failure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_REPOSITORY_WORKFLOW_REPORT_H
#define UMICOM_REPOSITORY_WORKFLOW_REPORT_H

#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include "umicom/repository/workflow_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryWorkflowReport {
    UmiRepositoryWorkflowAction action;
    UmiStatus status;
    int last_exit_code;
    int planned;
    int cloned;
    int initialised;
    int remote_added;
    int submodule_added;
    int staged;
    int commit_created;
    int no_changes;
    int pushed;
    int fetched;
    int updated;
    int submodules_updated;
    int output_truncated;
    char output[UMI_PROCESS_OUTPUT_CAPACITY];
} UmiRepositoryWorkflowReport;

void umi_repository_workflow_report_init(
    UmiRepositoryWorkflowReport *report,
    UmiRepositoryWorkflowAction action);

#ifdef __cplusplus
}
#endif

#endif
