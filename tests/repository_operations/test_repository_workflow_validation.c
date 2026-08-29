/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_workflow_validation.c
 *
 * PURPOSE:
 *   Verify incomplete and option-like workflow values are rejected before Git
 *   can change a repository.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_validation.h"

#include <stdlib.h>

int main(void)
{
    UmiRepositoryWorkflowRequest request;

    umi_repository_workflow_request_init(
        &request, UMI_REPOSITORY_WORKFLOW_PUBLISH, ".");
    if (umi_repository_workflow_validate(&request) !=
        UMI_STATUS_INVALID_ARGUMENT) {
        return EXIT_FAILURE;
    }
    request.commit_message = "feat(repository): add safe workflow";
    if (umi_repository_workflow_validate(&request) != UMI_STATUS_OK) {
        return EXIT_FAILURE;
    }
    request.remote_name = "--force";
    if (umi_repository_workflow_validate(&request) !=
        UMI_STATUS_INVALID_ARGUMENT) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
