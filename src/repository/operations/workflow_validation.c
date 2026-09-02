/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/workflow_validation.c
 *
 * PURPOSE:
 *   Reject incomplete or option-like repository workflow values before a Git
 *   process is started.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_validation.h"

#include <stddef.h>

/*
 * Provide the repository workflow has text operation used by this module and its client
 * applications.
 */
static int umi_repository_workflow_has_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

/*
 * Provide the repository workflow safe identifier operation used by this module and its
 * client applications.
 */
static int umi_repository_workflow_safe_identifier(const char *value)
{
    return !umi_repository_workflow_has_text(value) || value[0] != '-';
}

/*
 * Check that repository workflow satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_repository_workflow_validate(
    const UmiRepositoryWorkflowRequest *request)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (request == NULL ||
        request->action <= UMI_REPOSITORY_WORKFLOW_UNKNOWN ||
        request->action > UMI_REPOSITORY_WORKFLOW_UPDATE ||
        !umi_repository_workflow_has_text(request->repository_root) ||
        !umi_repository_workflow_safe_identifier(request->branch) ||
        !umi_repository_workflow_safe_identifier(request->remote_name) ||
        (request->auto_commit_message != 0 &&
         request->auto_commit_message != 1)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Select the behaviour associated with the requested command or state value. */
    switch (request->action) {
        case UMI_REPOSITORY_WORKFLOW_CLONE:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!umi_repository_workflow_has_text(request->source_url) ||
                !umi_repository_workflow_has_text(request->destination)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_INITIALISE:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!umi_repository_workflow_has_text(request->branch) ||
                (umi_repository_workflow_has_text(request->remote_url) &&
                 request->remote_url[0] == '-')) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!umi_repository_workflow_has_text(request->source_url) ||
                !umi_repository_workflow_has_text(request->submodule_path)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_COMMIT:
            /* Exactly one message source avoids silently ignoring either a
             * reviewed manual message or the explicit automatic request. */
            if (umi_repository_workflow_has_text(request->commit_message) ==
                request->auto_commit_message) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_PUSH:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!umi_repository_workflow_has_text(request->branch) ||
                !umi_repository_workflow_has_text(request->remote_name)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_PUBLISH:
            /* Publish uses the same exclusive message-source rule as commit. */
            if (umi_repository_workflow_has_text(request->commit_message) ==
                    request->auto_commit_message ||
                !umi_repository_workflow_has_text(request->branch) ||
                !umi_repository_workflow_has_text(request->remote_name)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_UPDATE:
            /* Apply this branch only when its contract condition is satisfied. */
            if (!umi_repository_workflow_has_text(request->branch) ||
                !umi_repository_workflow_has_text(request->remote_name)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        default:
            break;
    }
    return UMI_STATUS_OK;
}
