/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/jobs.c
 *
 * PURPOSE:
 *   Freeze reviewed AI requests and preserve uncertain outcomes without automatic retries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "workspace_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool AwToolAllowed(const UmiAiWorkspace *workspace, const char *id)
{
    for (size_t i = 0U; i < workspace->allowedToolCount; ++i)
        if (strcmp(workspace->allowedTools[i], id) == 0) return true;
    return false;
}
UmiStatus UmiAiWorkspaceAllowTool(UmiAiWorkspace *workspace, const char *toolId)
{
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(toolId, UMI_AI_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->runtime->tools.count > UMI_AI_MAX_TOOLS) return UMI_STATUS_INVALID_STATE;
    if (umi_ai_tool_registry_find(&workspace->runtime->tools, toolId) == NULL) return UMI_STATUS_NOT_FOUND;
    if (AwToolAllowed(workspace, toolId)) return UMI_STATUS_OK;
    if (workspace->allowedToolCount >= AW_MAX_ALLOWED_TOOLS) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)AwTextCopy(workspace->allowedTools[workspace->allowedToolCount++], UMI_AI_ID_CAPACITY, toolId, false);
    return UMI_STATUS_OK;
}
static UmiStatus AwJobBinding(UmiAiWorkspace *workspace, const UmiAiWorkspaceJob *job, bool preparing)
{
    if (workspace->runtime->providers.count > UMI_AI_MAX_PROVIDERS || workspace->runtime->tools.count > UMI_AI_MAX_TOOLS)
        return UMI_STATUS_INVALID_STATE;
    if (job->kind == UMI_AI_WORKSPACE_TOOL) {
        UmiAiTool *tool = umi_ai_tool_registry_find(&workspace->runtime->tools, job->providerId);
        if (tool == NULL) return UMI_STATUS_NOT_FOUND;
        if (!AwToolAllowed(workspace, job->providerId)) return UMI_STATUS_PERMISSION_DENIED;
        if (!AwTextValid(tool->permission, sizeof(tool->permission), false) || tool->invoke == NULL) return UMI_STATUS_INVALID_STATE;
        if (!preparing && strcmp(job->permission, tool->permission) != 0) return UMI_STATUS_BUSY;
        return umi_ai_policy_check_tool(&workspace->runtime->policy, 1);
    }
    UmiAiProvider *provider = umi_ai_provider_registry_find(&workspace->runtime->providers, job->providerId);
    if (provider == NULL) return UMI_STATUS_NOT_FOUND;
    if (umi_ai_provider_validate(provider) != UMI_STATUS_OK ||
        provider->kind < UMI_AI_PROVIDER_LOCAL || provider->kind > UMI_AI_PROVIDER_TEST) return UMI_STATUS_INVALID_STATE;
    if (!preparing && job->providerKind != provider->kind) return UMI_STATUS_BUSY;
    return umi_ai_policy_check_provider(&workspace->runtime->policy, provider->kind, 1);
}
UmiStatus UmiAiWorkspacePrepare(UmiAiWorkspace *workspace, const char *jobId,
    UmiAiWorkspaceJobKind kind, const char *providerId, const char *modelId,
    const char *collectionId, const char *prompt, const char *requestedBy, uint32_t maxOutputTokens)
{
    UmiStatus status = AwReady(workspace); AwState *next; size_t existing;
    const char *collection = collectionId != NULL ? collectionId : "";
    const char *model = modelId != NULL ? modelId : "";
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(jobId, UMI_AI_WORKSPACE_ID_CAPACITY) || !AwIdValid(providerId, UMI_AI_ID_CAPACITY) ||
        !AwTextValid(model, UMI_AI_ID_CAPACITY, kind == UMI_AI_WORKSPACE_TOOL) ||
        !AwTextValid(prompt, UMI_AI_WORKSPACE_PASSAGE_CAPACITY, false) ||
        !AwIdValid(requestedBy, UMI_AI_WORKSPACE_ID_CAPACITY) || kind < UMI_AI_WORKSPACE_DRAFT || kind > UMI_AI_WORKSPACE_TOOL ||
        (kind != UMI_AI_WORKSPACE_TOOL && (maxOutputTokens == 0U || maxOutputTokens > 4096U)) ||
        (kind == UMI_AI_WORKSPACE_TOOL && (maxOutputTokens != 0U || model[0] != '\0')) ||
        (kind == UMI_AI_WORKSPACE_GROUNDED_DRAFT ? !AwIdValid(collection, UMI_AI_WORKSPACE_ID_CAPACITY) : collection[0] != '\0'))
        return UMI_STATUS_INVALID_ARGUMENT;
    existing = AwJobIndex(workspace->state, jobId);
    if (existing != SIZE_MAX) {
        const UmiAiWorkspaceJob *job = &workspace->state->jobs[existing];
        return job->kind == kind && job->maxOutputTokens == maxOutputTokens &&
            strcmp(job->providerId, providerId) == 0 && strcmp(job->modelId, model) == 0 &&
            strcmp(job->collectionId, collection) == 0 && strcmp(job->prompt, prompt) == 0 &&
            strcmp(job->requestedBy, requestedBy) == 0 ? UMI_STATUS_OK : UMI_STATUS_ALREADY_EXISTS;
    }
    if (workspace->state->jobCount >= UMI_AI_WORKSPACE_MAX_JOBS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = malloc(sizeof(*next)); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state;
    UmiAiWorkspaceJob *job = &next->jobs[next->jobCount++]; memset(job, 0, sizeof(*job));
    (void)AwTextCopy(job->id, sizeof(job->id), jobId, false);
    (void)AwTextCopy(job->providerId, sizeof(job->providerId), providerId, false);
    (void)AwTextCopy(job->modelId, sizeof(job->modelId), model, true);
    (void)AwTextCopy(job->collectionId, sizeof(job->collectionId), collection, true);
    (void)AwTextCopy(job->prompt, sizeof(job->prompt), prompt, false);
    (void)AwTextCopy(job->requestedBy, sizeof(job->requestedBy), requestedBy, false);
    job->kind = kind; job->state = UMI_AI_WORKSPACE_REVIEW; job->maxOutputTokens = maxOutputTokens;
    job->corpusRevision = next->corpusRevision;
    status = AwJobBinding(workspace, job, true);
    if (status == UMI_STATUS_OK && kind == UMI_AI_WORKSPACE_TOOL) {
        UmiAiTool *tool = umi_ai_tool_registry_find(&workspace->runtime->tools, providerId);
        (void)AwTextCopy(job->permission, sizeof(job->permission), tool->permission, false);
    } else if (status == UMI_STATUS_OK) {
        UmiAiProvider *provider = umi_ai_provider_registry_find(&workspace->runtime->providers, providerId);
        job->providerKind = provider->kind;
    }
    if (status == UMI_STATUS_OK && kind == UMI_AI_WORKSPACE_GROUNDED_DRAFT) {
        status = UmiAiWorkspaceSearch(workspace, collection, prompt, NULL, NULL, NULL,
            job->evidence, UMI_AI_WORKSPACE_MAX_EVIDENCE, &job->evidenceCount);
        if (status == UMI_STATUS_OK && job->evidenceCount == 0U) status = UMI_STATUS_NOT_FOUND;
    }
    if (status != UMI_STATUS_OK) { free(next); return status; }
    return AwPublish(workspace, next, false);
}
UmiStatus UmiAiWorkspaceReview(UmiAiWorkspace *workspace, const char *jobId,
    const char *reviewedBy, bool approve)
{
    UmiStatus status = AwReady(workspace); AwState *next; size_t index;
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(jobId, UMI_AI_WORKSPACE_ID_CAPACITY) || !AwIdValid(reviewedBy, UMI_AI_WORKSPACE_ID_CAPACITY))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = AwJobIndex(workspace->state, jobId); if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiAiWorkspaceJob *job = &workspace->state->jobs[index];
    UmiAiWorkspaceJobState desired = approve ? UMI_AI_WORKSPACE_APPROVED : UMI_AI_WORKSPACE_DENIED;
    if (job->state == desired && strcmp(job->reviewedBy, reviewedBy) == 0) return UMI_STATUS_OK;
    if (job->state != UMI_AI_WORKSPACE_REVIEW) return UMI_STATUS_INVALID_STATE;
    if (approve) {
        if (job->kind == UMI_AI_WORKSPACE_GROUNDED_DRAFT && job->corpusRevision != workspace->state->corpusRevision)
            return UMI_STATUS_BUSY;
        status = AwJobBinding(workspace, job, false); if (status != UMI_STATUS_OK) return status;
    }
    next = malloc(sizeof(*next)); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state; next->jobs[index].state = desired;
    (void)AwTextCopy(next->jobs[index].reviewedBy, sizeof(next->jobs[index].reviewedBy), reviewedBy, false);
    return AwPublish(workspace, next, false);
}
UmiStatus UmiAiWorkspaceCancel(UmiAiWorkspace *workspace, const char *jobId)
{
    UmiStatus status = AwReady(workspace); AwState *next; size_t index;
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(jobId, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    index = AwJobIndex(workspace->state, jobId); if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiAiWorkspaceJobState state = workspace->state->jobs[index].state;
    if (state == UMI_AI_WORKSPACE_CANCELLED) return UMI_STATUS_OK;
    if (state != UMI_AI_WORKSPACE_REVIEW && state != UMI_AI_WORKSPACE_APPROVED) return UMI_STATUS_INVALID_STATE;
    next = malloc(sizeof(*next)); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state; next->jobs[index].state = UMI_AI_WORKSPACE_CANCELLED;
    next->jobs[index].status = UMI_STATUS_CANCELLED;
    return AwPublish(workspace, next, false);
}
UmiStatus UmiAiWorkspaceRun(UmiAiWorkspace *workspace, const char *jobId, const UmiAiWorkspaceCancellation *cancellation)
{
    UmiStatus status = AwReady(workspace), commitStatus; AwState *next; size_t index;
    UmiAiRequest *request = NULL; UmiAiResponse *response = NULL;
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(jobId, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    index = AwJobIndex(workspace->state, jobId); if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    const UmiAiWorkspaceJob *job = &workspace->state->jobs[index];
    if (job->state == UMI_AI_WORKSPACE_SUCCEEDED) return UMI_STATUS_OK;
    if (job->state != UMI_AI_WORKSPACE_APPROVED) return UMI_STATUS_INVALID_STATE;
    if (UmiAiWorkspaceCancellationIsRequested(cancellation)) {
        status = UmiAiWorkspaceCancel(workspace, jobId);
        return status == UMI_STATUS_OK ? UMI_STATUS_CANCELLED : status;
    }
    if (job->kind == UMI_AI_WORKSPACE_GROUNDED_DRAFT && job->corpusRevision != workspace->state->corpusRevision)
        return UMI_STATUS_BUSY;
    status = AwJobBinding(workspace, job, false); if (status != UMI_STATUS_OK) return status;
    request = calloc(1U, sizeof(*request)); response = calloc(1U, sizeof(*response));
    next = malloc(sizeof(*next));
    if (request == NULL || response == NULL || next == NULL) { free(request); free(response); free(next); return UMI_STATUS_OUT_OF_MEMORY; }
    if (job->kind != UMI_AI_WORKSPACE_TOOL) status = UmiAiWorkspaceBuildRequest(job, request);
    if (status != UMI_STATUS_OK) { free(request); free(response); free(next); return status; }
    *next = *workspace->state; next->jobs[index].state = UMI_AI_WORKSPACE_RUNNING;
    workspace->busy = true;
    status = AwPublish(workspace, next, false);
    if (status != UMI_STATUS_OK) { workspace->busy = false; free(request); free(response); return status; }
    job = &workspace->state->jobs[index];
    /* External work happens only after RUNNING is durable. The canonical AI
     * Runtime checks its current policy again and remains the sole dispatcher. */
    if (job->kind == UMI_AI_WORKSPACE_TOOL) {
        (void)AwTextCopy(response->request_id, sizeof(response->request_id), job->id, false);
        (void)AwTextCopy(response->provider_id, sizeof(response->provider_id), job->providerId, false);
        response->finish_reason = UMI_AI_FINISH_STOP;
        status = umi_ai_runtime_invoke_tool(workspace->runtime, job->providerId, 1, job->prompt,
            response->text, sizeof(response->text));
    } else status = umi_ai_runtime_generate(workspace->runtime, job->providerId, 1, request, response);
    if (status < UMI_STATUS_OK || status > UMI_STATUS_BUSY) status = UMI_STATUS_INTERNAL_ERROR;
    if (status == UMI_STATUS_OK) status = AwResponseValidate(job, response);
    bool cancelled = UmiAiWorkspaceCancellationIsRequested(cancellation);
    if (cancelled) status = UMI_STATUS_CANCELLED;
    next = malloc(sizeof(*next));
    if (next == NULL) {
        workspace->recoveryRequired = true; workspace->busy = false; free(request); free(response);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *next = *workspace->state; UmiAiWorkspaceJob *finished = &next->jobs[index];
    finished->status = status;
    finished->state = status == UMI_STATUS_OK ? UMI_AI_WORKSPACE_SUCCEEDED : UMI_AI_WORKSPACE_FAILED;
    if (cancelled || status == UMI_STATUS_CANCELLED)
        finished->state = job->kind == UMI_AI_WORKSPACE_TOOL ? UMI_AI_WORKSPACE_INTERRUPTED : UMI_AI_WORKSPACE_CANCELLED;
    if (status == UMI_STATUS_OK) finished->response = *response;
    commitStatus = AwPublish(workspace, next, false);
    if (commitStatus != UMI_STATUS_OK) workspace->recoveryRequired = true;
    workspace->busy = false; free(request); free(response);
    return commitStatus != UMI_STATUS_OK ? commitStatus : status;
}
