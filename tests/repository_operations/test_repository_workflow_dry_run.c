/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_workflow_dry_run.c
 *
 * PURPOSE:
 *   Verify dry-run planning requires no installed Git process and makes no
 *   filesystem or remote changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_executor.h"

#include <stdlib.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryWorkflowRequest request;
    UmiRepositoryWorkflowReport report;

    /* An undersized output record is rejected before any workflow is planned. */
    if (umi_repository_workflow_execute_sized(
            NULL,
            NULL,
            NULL,
            &report,
            sizeof(report) - 1U) != UMI_STATUS_INVALID_ARGUMENT) {
        return EXIT_FAILURE;
    }

    /* A valid request can then use the normal caller-size-aware API. */
    umi_repository_workflow_request_init(
        &request, UMI_REPOSITORY_WORKFLOW_PUBLISH, ".");
    request.commit_message = "feat(repository): add safe workflow";
    request.dry_run = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repository_workflow_execute(
            NULL, NULL, &request, &report) != UMI_STATUS_OK) {
        return EXIT_FAILURE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (!report.planned || report.commit_created || report.pushed ||
        strstr(report.output, "stage, validate") == NULL ||
        strstr(report.output, "origin/main") == NULL ||
        strstr(report.output, request.commit_message) == NULL) {
        return EXIT_FAILURE;
    }
    /* Dry-run describes automatic generation without requiring Git or staging
     * files, while the real workflow generates only after validation. */
    request.commit_message = NULL;
    request.auto_commit_message = 1;
    if (umi_repository_workflow_execute(
            NULL, NULL, &request, &report) != UMI_STATUS_OK ||
        strstr(report.output, "generated locally from staged paths") == NULL) {
        return EXIT_FAILURE;
    }
    umi_repository_workflow_request_init(
        &request, UMI_REPOSITORY_WORKFLOW_UPDATE, ".");
    request.dry_run = 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_repository_workflow_execute(
            NULL, NULL, &request, &report) != UMI_STATUS_OK ||
        !report.planned || strstr(report.output, "fast-forward") == NULL ||
        strstr(report.output, "origin/main") == NULL) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
