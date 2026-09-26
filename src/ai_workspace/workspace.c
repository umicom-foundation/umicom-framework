/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/workspace.c
 *
 * PURPOSE:
 *   Own source collections, immutable job views and controlled AI workspace mutations.
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

UmiStatus AwReady(const UmiAiWorkspace *workspace)
{
    if (workspace == NULL || workspace->state == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->busy) return UMI_STATUS_BUSY;
    if (workspace->runtime->providers.count > UMI_AI_MAX_PROVIDERS ||
        workspace->runtime->tools.count > UMI_AI_MAX_TOOLS) return UMI_STATUS_INVALID_STATE;
    return workspace->recoveryRequired ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
}
size_t AwCollectionIndex(const AwState *state, const char *id)
{
    if (id == NULL) return SIZE_MAX;
    for (size_t i = 0U; i < state->collectionCount; ++i)
        if (strcmp(state->collections[i].id, id) == 0) return i;
    return SIZE_MAX;
}
size_t AwSourceIndex(const AwState *state, const char *id)
{
    if (id == NULL) return SIZE_MAX;
    for (size_t i = 0U; i < state->sourceCount; ++i)
        if (strcmp(state->sources[i].id, id) == 0) return i;
    return SIZE_MAX;
}
size_t AwJobIndex(const AwState *state, const char *id)
{
    if (id == NULL) return SIZE_MAX;
    for (size_t i = 0U; i < state->jobCount; ++i)
        if (strcmp(state->jobs[i].id, id) == 0) return i;
    return SIZE_MAX;
}
static bool AwSourceValid(const UmiAiWorkspaceSource *source, uint64_t maximumRevision)
{
    uint64_t last;
    if (!AwIdValid(source->id, sizeof(source->id)) ||
        !AwIdValid(source->collectionId, sizeof(source->collectionId)) ||
        !AwTextValid(source->title, sizeof(source->title), false) ||
        !AwTextValid(source->text, sizeof(source->text), false) ||
        source->firstLine == 0U || source->revision == 0U || source->revision > maximumRevision) return false;
    last = source->firstLine;
    for (size_t n = 0U; source->text[n] != '\0'; ++n)
        if (source->text[n] == '\n' && source->text[n + 1U] != '\0') ++last;
    return last <= UINT32_MAX && source->lastLine == (uint32_t)last;
}
UmiStatus AwStateValidate(const AwState *state)
{
    if (state == NULL || state->collectionCount > UMI_AI_WORKSPACE_MAX_COLLECTIONS ||
        state->sourceCount > UMI_AI_WORKSPACE_MAX_SOURCES || state->jobCount > UMI_AI_WORKSPACE_MAX_JOBS ||
        state->corpusRevision > state->revision) return UMI_STATUS_PARSE_ERROR;
    for (size_t i = 0U; i < state->collectionCount; ++i) {
        const UmiAiWorkspaceCollection *collection = &state->collections[i];
        if (!AwIdValid(collection->id, sizeof(collection->id)) ||
            !AwTextValid(collection->title, sizeof(collection->title), false)) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(collection->id, state->collections[j].id) == 0) return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; i < state->sourceCount; ++i) {
        const UmiAiWorkspaceSource *source = &state->sources[i];
        if (!AwSourceValid(source, state->corpusRevision) ||
            AwCollectionIndex(state, source->collectionId) == SIZE_MAX) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(source->id, state->sources[j].id) == 0) return UMI_STATUS_PARSE_ERROR;
    }
    for (size_t i = 0U; i < state->jobCount; ++i) {
        const UmiAiWorkspaceJob *job = &state->jobs[i];
        if (!AwIdValid(job->id, sizeof(job->id)) || !AwIdValid(job->providerId, sizeof(job->providerId)) ||
            !AwTextValid(job->modelId, sizeof(job->modelId), job->kind == UMI_AI_WORKSPACE_TOOL) ||
            !AwTextValid(job->prompt, sizeof(job->prompt), false) ||
            !AwIdValid(job->requestedBy, sizeof(job->requestedBy)) ||
            !AwTextValid(job->reviewedBy, sizeof(job->reviewedBy), true) ||
            (job->reviewedBy[0] != '\0' && !AwIdValid(job->reviewedBy, sizeof(job->reviewedBy))) ||
            !AwTextValid(job->permission, sizeof(job->permission), true) ||
            job->kind < UMI_AI_WORKSPACE_DRAFT || job->kind > UMI_AI_WORKSPACE_TOOL ||
            job->state < UMI_AI_WORKSPACE_REVIEW || job->state > UMI_AI_WORKSPACE_INTERRUPTED ||
            job->evidenceCount > UMI_AI_WORKSPACE_MAX_EVIDENCE || job->corpusRevision > state->corpusRevision ||
            job->status < UMI_STATUS_OK || job->status > UMI_STATUS_BUSY ||
            !AwTextValid(job->response.text, sizeof(job->response.text), true)) return UMI_STATUS_PARSE_ERROR;
        if (job->kind != UMI_AI_WORKSPACE_TOOL && (job->maxOutputTokens == 0U || job->maxOutputTokens > 4096U ||
            job->providerKind < UMI_AI_PROVIDER_LOCAL || job->providerKind > UMI_AI_PROVIDER_TEST)) return UMI_STATUS_PARSE_ERROR;
        /* Persisted records must obey the same kind/state contract as fresh
         * commands. An altered record cannot smuggle output into a pending
         * job or make a failed operation look like a successful one. */
        if (job->kind == UMI_AI_WORKSPACE_TOOL) {
            if (job->modelId[0] != '\0' || job->providerKind != 0 || job->maxOutputTokens != 0U ||
                !AwTextValid(job->permission, sizeof(job->permission), false)) return UMI_STATUS_PARSE_ERROR;
        } else if (job->permission[0] != '\0') return UMI_STATUS_PARSE_ERROR;
        if (job->state == UMI_AI_WORKSPACE_REVIEW && job->reviewedBy[0] != '\0') return UMI_STATUS_PARSE_ERROR;
        if ((job->state == UMI_AI_WORKSPACE_REVIEW || job->state == UMI_AI_WORKSPACE_APPROVED ||
            job->state == UMI_AI_WORKSPACE_RUNNING || job->state == UMI_AI_WORKSPACE_DENIED) &&
            job->status != UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
        if ((job->state == UMI_AI_WORKSPACE_FAILED || job->state == UMI_AI_WORKSPACE_INTERRUPTED) &&
            job->status == UMI_STATUS_OK) return UMI_STATUS_PARSE_ERROR;
        if (job->state == UMI_AI_WORKSPACE_CANCELLED && job->status != UMI_STATUS_CANCELLED) return UMI_STATUS_PARSE_ERROR;
        if (job->state != UMI_AI_WORKSPACE_SUCCEEDED && (job->response.text[0] != '\0' ||
            job->response.request_id[0] != '\0' || job->response.provider_id[0] != '\0' ||
            job->response.model_id[0] != '\0' || job->response.finish_reason != UMI_AI_FINISH_NONE ||
            job->response.usage.input_tokens != 0U || job->response.usage.output_tokens != 0U ||
            job->response.usage.total_tokens != 0U)) return UMI_STATUS_PARSE_ERROR;
        if (job->kind == UMI_AI_WORKSPACE_GROUNDED_DRAFT) {
            if (job->evidenceCount == 0U || !AwIdValid(job->collectionId, sizeof(job->collectionId))) return UMI_STATUS_PARSE_ERROR;
        } else if (job->evidenceCount != 0U || job->collectionId[0] != '\0') return UMI_STATUS_PARSE_ERROR;
        if ((job->state == UMI_AI_WORKSPACE_APPROVED || job->state == UMI_AI_WORKSPACE_RUNNING ||
             job->state == UMI_AI_WORKSPACE_SUCCEEDED || job->state == UMI_AI_WORKSPACE_FAILED ||
             job->state == UMI_AI_WORKSPACE_DENIED || job->state == UMI_AI_WORKSPACE_INTERRUPTED) &&
             job->reviewedBy[0] == '\0') return UMI_STATUS_PARSE_ERROR;
        for (size_t e = 0U; e < job->evidenceCount; ++e) {
            if (!AwSourceValid(&job->evidence[e].source, job->corpusRevision) ||
                strcmp(job->evidence[e].source.collectionId, job->collectionId) != 0) return UMI_STATUS_PARSE_ERROR;
            for (size_t n = 0U; n < e; ++n)
                if (strcmp(job->evidence[n].source.id, job->evidence[e].source.id) == 0) return UMI_STATUS_PARSE_ERROR;
        }
        if (job->state == UMI_AI_WORKSPACE_SUCCEEDED &&
            (job->status != UMI_STATUS_OK || AwResponseValidate(job, &job->response) != UMI_STATUS_OK)) return UMI_STATUS_PARSE_ERROR;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(job->id, state->jobs[j].id) == 0) return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
UmiStatus AwPublish(UmiAiWorkspace *workspace, AwState *next, bool corpusChanged)
{
    UmiStatus status = AwSave(workspace, next);
    if (status == UMI_STATUS_OK) {
        AwState *previous = workspace->state; workspace->state = next; free(previous);
        if (corpusChanged) memset(workspace->embeddings, 0, sizeof(workspace->embeddings));
    } else free(next);
    return status;
}
UmiStatus UmiAiWorkspaceCreate(UmiDataServer *data, UmiAiRuntime *runtime,
    const char *bookId, UmiAiWorkspace **outWorkspace)
{
    UmiAiWorkspace *workspace; UmiStatus status;
    if (outWorkspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outWorkspace = NULL;
    if (data == NULL || runtime == NULL || !AwIdValid(bookId, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    workspace = calloc(1U, sizeof(*workspace)); if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    workspace->state = calloc(1U, sizeof(*workspace->state));
    if (workspace->state == NULL) { free(workspace); return UMI_STATUS_OUT_OF_MEMORY; }
    workspace->data = data; workspace->runtime = runtime;
    (void)snprintf(workspace->prefix, sizeof(workspace->prefix), "ai.workspace/%s/", bookId);
    status = AwLoad(workspace, workspace->state);
    if (status != UMI_STATUS_OK) { UmiAiWorkspaceDestroy(workspace); return status; }
    *outWorkspace = workspace; return UMI_STATUS_OK;
}
void UmiAiWorkspaceDestroy(UmiAiWorkspace *workspace)
{
    if (workspace == NULL) return;
    /* The owner must join pending work first. Borrowed registries and storage
     * are deliberately not destroyed here. */
    free(workspace->state); free(workspace);
}
UmiStatus UmiAiWorkspaceReload(UmiAiWorkspace *workspace)
{
    AwState *next; UmiStatus status;
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->busy) return UMI_STATUS_BUSY;
    next = calloc(1U, sizeof(*next)); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = AwLoad(workspace, next);
    if (status == UMI_STATUS_OK) {
        free(workspace->state); workspace->state = next; workspace->recoveryRequired = false;
        memset(workspace->embeddings, 0, sizeof(workspace->embeddings));
    } else free(next);
    return status;
}
UmiStatus UmiAiWorkspaceSnapshotRead(const UmiAiWorkspace *workspace, UmiAiWorkspaceSnapshot *outSnapshot)
{
    if (workspace == NULL || outSnapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->busy) return UMI_STATUS_BUSY;
    memset(outSnapshot, 0, sizeof(*outSnapshot));
    outSnapshot->revision = workspace->state->revision; outSnapshot->corpusRevision = workspace->state->corpusRevision;
    outSnapshot->collectionCount = workspace->state->collectionCount; outSnapshot->sourceCount = workspace->state->sourceCount;
    outSnapshot->jobCount = workspace->state->jobCount; outSnapshot->recoveryRequired = workspace->recoveryRequired;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceCollectionAt(const UmiAiWorkspace *workspace, size_t index, UmiAiWorkspaceCollection *outCollection)
{
    if (workspace == NULL || outCollection == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->busy) return UMI_STATUS_BUSY;
    if (index >= workspace->state->collectionCount) return UMI_STATUS_NOT_FOUND;
    *outCollection = workspace->state->collections[index]; return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceSourceAt(const UmiAiWorkspace *workspace, size_t index, UmiAiWorkspaceSource *outSource)
{
    if (workspace == NULL || outSource == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->busy) return UMI_STATUS_BUSY;
    if (index >= workspace->state->sourceCount) return UMI_STATUS_NOT_FOUND;
    *outSource = workspace->state->sources[index]; return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceJobAt(const UmiAiWorkspace *workspace, size_t index, UmiAiWorkspaceJob *outJob)
{
    if (workspace == NULL || outJob == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->busy) return UMI_STATUS_BUSY;
    if (index >= workspace->state->jobCount) return UMI_STATUS_NOT_FOUND;
    *outJob = workspace->state->jobs[index]; return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceJobFind(const UmiAiWorkspace *workspace, const char *id, UmiAiWorkspaceJob *outJob)
{
    if (workspace == NULL || outJob == NULL || !AwIdValid(id, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->busy) return UMI_STATUS_BUSY;
    return UmiAiWorkspaceJobAt(workspace, AwJobIndex(workspace->state, id), outJob);
}
UmiStatus UmiAiWorkspacePutCollection(UmiAiWorkspace *workspace, const char *id, const char *title)
{
    AwState *next; size_t index; UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(id, UMI_AI_WORKSPACE_ID_CAPACITY) || !AwTextValid(title, UMI_AI_WORKSPACE_TITLE_CAPACITY, false))
        return UMI_STATUS_INVALID_ARGUMENT;
    index = AwCollectionIndex(workspace->state, id);
    if (index != SIZE_MAX && strcmp(title, workspace->state->collections[index].title) == 0) return UMI_STATUS_OK;
    if (index == SIZE_MAX && workspace->state->collectionCount >= UMI_AI_WORKSPACE_MAX_COLLECTIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = malloc(sizeof(*next)); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state;
    if (index == SIZE_MAX) index = next->collectionCount++;
    (void)AwTextCopy(next->collections[index].id, sizeof(next->collections[index].id), id, false);
    (void)AwTextCopy(next->collections[index].title, sizeof(next->collections[index].title), title, false);
    return AwPublish(workspace, next, false);
}
UmiStatus UmiAiWorkspacePutSource(UmiAiWorkspace *workspace, const char *id,
    const char *collectionId, const char *title, const char *text, uint32_t firstLine)
{
    AwState *next; size_t index; uint64_t last; UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(id, UMI_AI_WORKSPACE_ID_CAPACITY) || !AwIdValid(collectionId, UMI_AI_WORKSPACE_ID_CAPACITY) ||
        !AwTextValid(title, UMI_AI_WORKSPACE_TITLE_CAPACITY, false) ||
        !AwTextValid(text, UMI_AI_WORKSPACE_PASSAGE_CAPACITY, false) || firstLine == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    if (AwCollectionIndex(workspace->state, collectionId) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    last = firstLine;
    for (size_t n = 0U; text[n] != '\0'; ++n) if (text[n] == '\n' && text[n + 1U] != '\0') ++last;
    if (last > UINT32_MAX || workspace->state->corpusRevision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    index = AwSourceIndex(workspace->state, id);
    if (index != SIZE_MAX) {
        const UmiAiWorkspaceSource *source = &workspace->state->sources[index];
        if (source->firstLine == firstLine && strcmp(source->collectionId, collectionId) == 0 &&
            strcmp(source->title, title) == 0 && strcmp(source->text, text) == 0) return UMI_STATUS_OK;
    } else if (workspace->state->sourceCount >= UMI_AI_WORKSPACE_MAX_SOURCES) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = malloc(sizeof(*next)); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state; if (index == SIZE_MAX) index = next->sourceCount++;
    UmiAiWorkspaceSource *source = &next->sources[index]; memset(source, 0, sizeof(*source));
    (void)AwTextCopy(source->id, sizeof(source->id), id, false);
    (void)AwTextCopy(source->collectionId, sizeof(source->collectionId), collectionId, false);
    (void)AwTextCopy(source->title, sizeof(source->title), title, false);
    (void)AwTextCopy(source->text, sizeof(source->text), text, false);
    source->firstLine = firstLine; source->lastLine = (uint32_t)last; source->revision = ++next->corpusRevision;
    return AwPublish(workspace, next, true);
}
UmiStatus UmiAiWorkspaceRemoveSource(UmiAiWorkspace *workspace, const char *id)
{
    AwState *next; size_t index; UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(id, UMI_AI_WORKSPACE_ID_CAPACITY)) return UMI_STATUS_INVALID_ARGUMENT;
    index = AwSourceIndex(workspace->state, id); if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (workspace->state->corpusRevision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = malloc(sizeof(*next)); if (next == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state;
    memmove(&next->sources[index], &next->sources[index + 1U], (next->sourceCount - index - 1U) * sizeof(next->sources[0]));
    memset(&next->sources[--next->sourceCount], 0, sizeof(next->sources[0])); ++next->corpusRevision;
    return AwPublish(workspace, next, true);
}
const char *UmiAiWorkspaceJobStateText(UmiAiWorkspaceJobState state)
{
    switch (state) {
    case UMI_AI_WORKSPACE_REVIEW: return "Review required";
    case UMI_AI_WORKSPACE_APPROVED: return "Approved, not run";
    case UMI_AI_WORKSPACE_RUNNING: return "Running";
    case UMI_AI_WORKSPACE_SUCCEEDED: return "Completed";
    case UMI_AI_WORKSPACE_FAILED: return "Failed";
    case UMI_AI_WORKSPACE_DENIED: return "Denied";
    case UMI_AI_WORKSPACE_CANCELLED: return "Cancelled";
    case UMI_AI_WORKSPACE_INTERRUPTED: return "Interrupted: outcome uncertain";
    default: return "Invalid state";
    }
}
