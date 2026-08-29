/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/workflow_types.c
 *
 * PURPOSE:
 *   Implement stable text and safety classification for repository workflow
 *   actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_types.h"

const char *umi_repository_workflow_action_text(
    UmiRepositoryWorkflowAction action)
{
    switch (action) {
        case UMI_REPOSITORY_WORKFLOW_CLONE: return "clone";
        case UMI_REPOSITORY_WORKFLOW_INITIALISE: return "init";
        case UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD: return "submodule-add";
        case UMI_REPOSITORY_WORKFLOW_STAGE: return "stage";
        case UMI_REPOSITORY_WORKFLOW_COMMIT: return "commit";
        case UMI_REPOSITORY_WORKFLOW_PUSH: return "push";
        case UMI_REPOSITORY_WORKFLOW_PUBLISH: return "publish";
        default: return "unknown";
    }
}

int umi_repository_workflow_action_writes_index(
    UmiRepositoryWorkflowAction action)
{
    return action == UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD ||
           action == UMI_REPOSITORY_WORKFLOW_STAGE ||
           action == UMI_REPOSITORY_WORKFLOW_COMMIT ||
           action == UMI_REPOSITORY_WORKFLOW_PUBLISH;
}

int umi_repository_workflow_action_uses_remote(
    UmiRepositoryWorkflowAction action)
{
    return action == UMI_REPOSITORY_WORKFLOW_CLONE ||
           action == UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD ||
           action == UMI_REPOSITORY_WORKFLOW_PUSH ||
           action == UMI_REPOSITORY_WORKFLOW_PUBLISH;
}
