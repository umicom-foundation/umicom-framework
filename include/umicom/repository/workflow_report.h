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

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"
#include "umicom/platform/process.h"
#include "umicom/repository/workflow_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_WORKFLOW_REPORT_API_VERSION 1U

typedef struct UmiRepositoryWorkflowReport {
    uint32_t structure_size;
    uint32_t api_version;
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

UmiStatus umi_repository_workflow_report_initialize(
    UmiRepositoryWorkflowReport *report,
    size_t caller_structure_size,
    UmiRepositoryWorkflowAction action);

/* Pass the caller's size so the Framework never writes through a stale ABI. */
#define umi_repository_workflow_report_init(report, action)                  \
    ((void)umi_repository_workflow_report_initialize(                       \
        (report), sizeof(*(report)), (action)))

#ifdef __cplusplus
}
#endif

#endif
