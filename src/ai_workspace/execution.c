/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/execution.c
 *
 * PURPOSE:
 *   Own durable reviewed execution and distinguish provider outcomes from final storage publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "execution_internal.h"
#include <stdlib.h>
#include <string.h>

struct AwExecution {
    UmiAiWorkspace *workspace;
    size_t job_index;
    AwExecutionPhase phase;
    bool owns_busy;
    UmiStatus operation_status;
    UmiStatus publication_status;
    UmiAiWorkspaceJob job;
    UmiAiRequest request;
    UmiAiResponse response;
};

UmiStatus AwExecutionPrepare(UmiAiWorkspace *workspace, const char *job_id,
    const UmiAiWorkspaceCancellation *cancellation, AwExecution **out_execution)
{
    if (out_execution == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(job_id, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    size_t index = AwJobIndex(workspace->state, job_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiAiWorkspaceJob *job = &workspace->state->jobs[index];
    if (job->state == UMI_AI_WORKSPACE_SUCCEEDED) return UMI_STATUS_ALREADY_EXISTS;
    if (job->state != UMI_AI_WORKSPACE_APPROVED) return UMI_STATUS_INVALID_STATE;
    if (UmiAiWorkspaceCancellationIsRequested(cancellation)) {
        status = UmiAiWorkspaceCancel(workspace, job_id);
        return status == UMI_STATUS_OK ? UMI_STATUS_CANCELLED : status;
    }
    if (job->kind == UMI_AI_WORKSPACE_GROUNDED_DRAFT &&
        job->corpusRevision != workspace->state->corpusRevision) return UMI_STATUS_BUSY;
    status = AwJobBinding(workspace, job, false);
    if (status != UMI_STATUS_OK) return status;
    if (workspace->execution_owners == SIZE_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    AwExecution *execution = calloc(1U, sizeof(*execution));
    if (execution == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    execution->workspace = workspace;
    execution->job_index = index;
    execution->job = *job;
    execution->operation_status = UMI_STATUS_BUSY;
    execution->publication_status = UMI_STATUS_BUSY;
    if (job->kind != UMI_AI_WORKSPACE_TOOL)
        status = UmiAiWorkspaceBuildRequest(job, &execution->request);
    if (status != UMI_STATUS_OK) { free(execution); return status; }
    /* The count retains storage even after completion releases the busy lease.
     * A controller may still need the copied outcome before releasing itself. */
    ++workspace->execution_owners;
    workspace->busy = true;
    execution->owns_busy = true;
    *out_execution = execution;
    return UMI_STATUS_OK;
}

UmiStatus AwExecutionStart(AwExecution *execution)
{
    if (execution == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (execution->phase != AW_EXECUTION_PREPARED || !execution->owns_busy)
        return UMI_STATUS_INVALID_STATE;
    UmiAiWorkspace *workspace = execution->workspace;
    /* Policy may have changed between preparation and the dispatch boundary.
     * Workspace mutation remains blocked by the prepared execution's lease. */
    UmiStatus status = AwJobBinding(workspace, &execution->job, false);
    if (status != UMI_STATUS_OK) return status;
    AwState *next = malloc(sizeof(*next));
    if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state;
    next->jobs[execution->job_index].state = UMI_AI_WORKSPACE_RUNNING;
    status = AwPublish(workspace, next, false);
    if (status == UMI_STATUS_OK) execution->phase = AW_EXECUTION_RUNNING;
    return status;
}

UmiStatus AwExecutionComplete(AwExecution *execution, UmiStatus operation_status, bool cancelled)
{
    if (execution == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (execution->phase != AW_EXECUTION_RUNNING || !execution->owns_busy)
        return UMI_STATUS_INVALID_STATE;
    UmiAiWorkspace *workspace = execution->workspace;
    if (operation_status < UMI_STATUS_OK || operation_status > UMI_STATUS_BUSY)
        operation_status = UMI_STATUS_INTERNAL_ERROR;
    if (operation_status == UMI_STATUS_OK)
        operation_status = AwResponseValidate(&execution->job, &execution->response);
    if (cancelled) operation_status = UMI_STATUS_CANCELLED;
    execution->operation_status = operation_status;
    AwState *next = malloc(sizeof(*next));
    UmiStatus publication = UMI_STATUS_OUT_OF_MEMORY;
    if (next != NULL) {
        *next = *workspace->state;
        UmiAiWorkspaceJob *finished = &next->jobs[execution->job_index];
        finished->status = operation_status;
        finished->state = operation_status == UMI_STATUS_OK
            ? UMI_AI_WORKSPACE_SUCCEEDED : UMI_AI_WORKSPACE_FAILED;
        if (cancelled || operation_status == UMI_STATUS_CANCELLED)
            finished->state = execution->job.kind == UMI_AI_WORKSPACE_TOOL
                ? UMI_AI_WORKSPACE_INTERRUPTED : UMI_AI_WORKSPACE_CANCELLED;
        if (operation_status == UMI_STATUS_OK) finished->response = execution->response;
        publication = AwPublish(workspace, next, false);
    }
    /* A completed external operation must never be automatically repeated
     * because its final record could not be published. Reload performs the
     * existing uncertain-outcome recovery; it does not retry generation. */
    execution->publication_status = publication;
    if (publication != UMI_STATUS_OK) workspace->recoveryRequired = true;
    execution->phase = AW_EXECUTION_SETTLED;
    execution->owns_busy = false;
    workspace->busy = false;
    return publication != UMI_STATUS_OK ? publication : operation_status;
}

UmiStatus AwExecutionRelease(AwExecution **in_out_execution)
{
    if (in_out_execution == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    AwExecution *execution = *in_out_execution;
    if (execution == NULL) return UMI_STATUS_OK;
    if (execution->phase == AW_EXECUTION_RUNNING) return UMI_STATUS_BUSY;
    if (execution->workspace->execution_owners == 0U) return UMI_STATUS_INVALID_STATE;
    if (execution->owns_busy) execution->workspace->busy = false;
    --execution->workspace->execution_owners;
    free(execution);
    *in_out_execution = NULL;
    return UMI_STATUS_OK;
}
const UmiAiWorkspaceJob *AwExecutionJob(const AwExecution *execution)
{ return execution != NULL ? &execution->job : NULL; }
const UmiAiRequest *AwExecutionRequest(const AwExecution *execution)
{ return execution != NULL ? &execution->request : NULL; }
UmiAiResponse *AwExecutionResponse(AwExecution *execution)
{ return execution != NULL ? &execution->response : NULL; }
AwExecutionPhase AwExecutionGetPhase(const AwExecution *execution)
{ return execution != NULL ? execution->phase : AW_EXECUTION_PREPARED; }
UmiStatus AwExecutionOperationStatus(const AwExecution *execution)
{ return execution != NULL ? execution->operation_status : UMI_STATUS_INVALID_ARGUMENT; }
UmiStatus AwExecutionPublicationStatus(const AwExecution *execution)
{ return execution != NULL ? execution->publication_status : UMI_STATUS_INVALID_ARGUMENT; }
