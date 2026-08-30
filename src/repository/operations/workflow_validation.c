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

static int umi_repository_workflow_has_text(const char *value)
{
    return value != NULL && value[0] != '\0';
}

static int umi_repository_workflow_safe_identifier(const char *value)
{
    return !umi_repository_workflow_has_text(value) || value[0] != '-';
}

UmiStatus umi_repository_workflow_validate(
    const UmiRepositoryWorkflowRequest *request)
{
    if (request == NULL ||
        request->action <= UMI_REPOSITORY_WORKFLOW_UNKNOWN ||
        request->action > UMI_REPOSITORY_WORKFLOW_UPDATE ||
        !umi_repository_workflow_has_text(request->repository_root) ||
        !umi_repository_workflow_safe_identifier(request->branch) ||
        !umi_repository_workflow_safe_identifier(request->remote_name)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    switch (request->action) {
        case UMI_REPOSITORY_WORKFLOW_CLONE:
            if (!umi_repository_workflow_has_text(request->source_url) ||
                !umi_repository_workflow_has_text(request->destination)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_INITIALISE:
            if (!umi_repository_workflow_has_text(request->branch) ||
                (umi_repository_workflow_has_text(request->remote_url) &&
                 request->remote_url[0] == '-')) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_SUBMODULE_ADD:
            if (!umi_repository_workflow_has_text(request->source_url) ||
                !umi_repository_workflow_has_text(request->submodule_path)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_COMMIT:
            if (!umi_repository_workflow_has_text(request->commit_message)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_PUSH:
            if (!umi_repository_workflow_has_text(request->branch) ||
                !umi_repository_workflow_has_text(request->remote_name)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_PUBLISH:
            if (!umi_repository_workflow_has_text(request->commit_message) ||
                !umi_repository_workflow_has_text(request->branch) ||
                !umi_repository_workflow_has_text(request->remote_name)) {
                return UMI_STATUS_INVALID_ARGUMENT;
            }
            break;
        case UMI_REPOSITORY_WORKFLOW_UPDATE:
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
