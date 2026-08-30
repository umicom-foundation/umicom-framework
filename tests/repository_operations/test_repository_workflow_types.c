/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_operations/test_repository_workflow_types.c
 *
 * PURPOSE:
 *   Verify repository workflow actions retain stable names and safety
 *   classifications for CLI and application consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_types.h"

#include <stdlib.h>
#include <string.h>

int main(void)
{
    if (strcmp(umi_repository_workflow_action_text(
                   UMI_REPOSITORY_WORKFLOW_PUBLISH),
               "publish") != 0) {
        return EXIT_FAILURE;
    }
    if (!umi_repository_workflow_action_writes_index(
            UMI_REPOSITORY_WORKFLOW_PUBLISH)) {
        return EXIT_FAILURE;
    }
    if (umi_repository_workflow_action_writes_index(
            UMI_REPOSITORY_WORKFLOW_CLONE)) {
        return EXIT_FAILURE;
    }
    if (!umi_repository_workflow_action_uses_remote(
            UMI_REPOSITORY_WORKFLOW_PUSH)) {
        return EXIT_FAILURE;
    }
    if (strcmp(umi_repository_workflow_action_text(
                   UMI_REPOSITORY_WORKFLOW_UPDATE),
               "update") != 0 ||
        !umi_repository_workflow_action_writes_index(
            UMI_REPOSITORY_WORKFLOW_UPDATE) ||
        !umi_repository_workflow_action_uses_remote(
            UMI_REPOSITORY_WORKFLOW_UPDATE)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
