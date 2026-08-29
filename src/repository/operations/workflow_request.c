/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/workflow_request.c
 *
 * PURPOSE:
 *   Initialise repository workflow requests with predictable, beginner-safe
 *   defaults.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/workflow_request.h"

#include <string.h>

void umi_repository_workflow_request_init(
    UmiRepositoryWorkflowRequest *request,
    UmiRepositoryWorkflowAction action,
    const char *repository_root)
{
    if (request == NULL) return;
    (void)memset(request, 0, sizeof(*request));
    request->action = action;
    request->repository_root =
        repository_root != NULL && repository_root[0] != '\0'
            ? repository_root
            : ".";
    request->branch = "main";
    request->remote_name = "origin";
    request->recursive = 1;
}
