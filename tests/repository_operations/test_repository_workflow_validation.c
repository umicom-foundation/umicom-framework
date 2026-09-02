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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRepositoryWorkflowRequest request;

    umi_repository_workflow_request_init(
        &request, UMI_REPOSITORY_WORKFLOW_PUBLISH, ".");
    /* Apply this operation only while the related capability or state is available. */
    if (umi_repository_workflow_validate(&request) !=
        UMI_STATUS_INVALID_ARGUMENT) {
        return EXIT_FAILURE;
    }
    request.commit_message = "feat(repository): add safe workflow";
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_repository_workflow_validate(&request) != UMI_STATUS_OK) {
        return EXIT_FAILURE;
    }
    /* Automatic and manual messages are individually valid but deliberately
     * cannot be requested together. */
    request.commit_message = NULL;
    request.auto_commit_message = 1;
    if (umi_repository_workflow_validate(&request) != UMI_STATUS_OK) {
        return EXIT_FAILURE;
    }
    request.commit_message = "feat(repository): conflicting message";
    if (umi_repository_workflow_validate(&request) !=
        UMI_STATUS_INVALID_ARGUMENT) {
        return EXIT_FAILURE;
    }
    request.auto_commit_message = 0;
    request.remote_name = "--force";
    /* Apply this operation only while the related capability or state is available. */
    if (umi_repository_workflow_validate(&request) !=
        UMI_STATUS_INVALID_ARGUMENT) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
