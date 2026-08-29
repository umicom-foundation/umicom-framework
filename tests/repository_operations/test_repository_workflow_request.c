/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_workflow_request.c
 *
 * PURPOSE:
 *   Verify repository workflow requests start with understandable defaults.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_request.h"

#include <stdlib.h>
#include <string.h>

int main(void)
{
    UmiRepositoryWorkflowRequest request;

    umi_repository_workflow_request_init(
        &request, UMI_REPOSITORY_WORKFLOW_PUBLISH, NULL);
    if (request.action != UMI_REPOSITORY_WORKFLOW_PUBLISH ||
        strcmp(request.repository_root, ".") != 0 ||
        strcmp(request.branch, "main") != 0 ||
        strcmp(request.remote_name, "origin") != 0 ||
        !request.recursive || request.dry_run) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
