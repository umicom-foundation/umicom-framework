/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/source_change.c
 * PURPOSE: Keep source maintenance and frozen job evidence in the canonical workspace transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/source_change.h"
#include "workspace_internal.h"
#include <stdlib.h>
#include <string.h>

struct UmiAiWorkspaceSourceChange
{
    UmiAiWorkspace *owner;
    UmiAiWorkspaceSourceChangeSummary summary;
    UmiAiWorkspaceCollection collection;
    UmiAiWorkspaceSource before[UMI_AI_WORKSPACE_MAX_SOURCES];
    UmiAiWorkspaceSource after[UMI_AI_WORKSPACE_MAX_SOURCES];
};
void UmiAiWorkspaceSourceChangeDestroy(UmiAiWorkspaceSourceChange *change) { free(change); }
UmiStatus UmiAiWorkspaceSourceChangeInspect(const UmiAiWorkspaceSourceChange *change,
                                            UmiAiWorkspaceSourceChangeSummary *out)
{
    if (change == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = change->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceSourceChangeBeforeAt(const UmiAiWorkspaceSourceChange *change, size_t index,
                                             UmiAiWorkspaceSource *out)
{
    if (change == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= change->summary.beforeCount)
        return UMI_STATUS_NOT_FOUND;
    *out = change->before[index];
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceSourceChangeAfterAt(const UmiAiWorkspaceSourceChange *change, size_t index,
                                            UmiAiWorkspaceSource *out)
{
    if (change == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= change->summary.afterCount)
        return UMI_STATUS_NOT_FOUND;
    *out = change->after[index];
    return UMI_STATUS_OK;
}
static bool SourceSelected(const UmiAiWorkspaceSourceChange *change, const char *id)
{
    for (size_t i = 0U; i < change->summary.beforeCount; ++i)
        if (strcmp(change->before[i].id, id) == 0)
            return true;
    return false;
}
/* Compact a private candidate only. No caller can observe half the removal,
 * and unrelated passages retain their order, contents and source revisions. */
static void SourceRemoveSelected(AwState *state, const UmiAiWorkspaceSourceChange *change)
{
    size_t retained = 0U, previous = state->sourceCount;
    for (size_t i = 0U; i < previous; ++i)
        if (!SourceSelected(change, state->sources[i].id))
            state->sources[retained++] = state->sources[i];
    memset(&state->sources[retained], 0, (previous - retained) * sizeof(state->sources[0]));
    state->sourceCount = retained;
}
static UmiStatus SourceCapture(UmiAiWorkspace *workspace, const char *const *ids, size_t count,
                               UmiAiWorkspaceSourceChange **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK)
        return status;
    if (ids == NULL || count == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (count > UMI_AI_WORKSPACE_MAX_SOURCES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    for (size_t i = 0U; i < count; ++i)
    {
        if (!AwIdValid(ids[i], UMI_AI_WORKSPACE_ID_CAPACITY))
            return UMI_STATUS_INVALID_ARGUMENT;
        if (AwSourceIndex(workspace->state, ids[i]) == SIZE_MAX)
            return UMI_STATUS_NOT_FOUND;
        for (size_t j = 0U; j < i; ++j)
            if (strcmp(ids[j], ids[i]) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
    }
    UmiAiWorkspaceSourceChange *change = calloc(1U, sizeof(*change));
    if (change == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    change->owner = workspace;
    change->summary.beforeCount = count;
    change->summary.workspaceRevision = workspace->state->revision;
    change->summary.corpusRevision = workspace->state->corpusRevision;
    for (size_t i = 0U; i < count; ++i)
        change->before[i] = workspace->state->sources[AwSourceIndex(workspace->state, ids[i])];
    *out = change;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceSourceRemovalCreate(UmiAiWorkspace *workspace, const char *const *ids, size_t count,
                                            UmiAiWorkspaceSourceChange **out)
{
    UmiStatus status = SourceCapture(workspace, ids, count, out);
    if (status != UMI_STATUS_OK)
        return status;
    if (workspace->state->revision == UINT64_MAX || workspace->state->corpusRevision == UINT64_MAX)
    {
        UmiAiWorkspaceSourceChangeDestroy(*out);
        *out = NULL;
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (*out)->summary.kind = UMI_AI_WORKSPACE_SOURCE_REMOVE;
    (*out)->summary.hasChanges = true;
    return UMI_STATUS_OK;
}
static bool SourceSameContent(const UmiAiWorkspaceSource *a, const UmiAiWorkspaceSource *b)
{
    return strcmp(a->id, b->id) == 0 && strcmp(a->collectionId, b->collectionId) == 0 &&
           strcmp(a->title, b->title) == 0 && strcmp(a->text, b->text) == 0 && a->firstLine == b->firstLine &&
           a->lastLine == b->lastLine;
}
UmiStatus UmiAiWorkspaceSourceReplacementCreate(UmiAiWorkspace *workspace, const char *const *ids,
                                                size_t count, const char *collectionId,
                                                const char *collectionTitle, const char *documentId,
                                                const char *title, const char *text, size_t bytes,
                                                UmiAiWorkspaceSourceChange **out)
{
    UmiStatus status = SourceCapture(workspace, ids, count, out);
    if (status != UMI_STATUS_OK)
        return status;
    UmiAiWorkspaceSourceChange *change = *out;
    UmiAiWorkspaceImport *import = NULL;
    /* Reuse the import parser against a private view with only the explicitly
     * selected passages removed. This shares Unicode, line and capacity rules
     * without adding a second persistence path or guessing document ownership. */
    UmiAiWorkspace *view = malloc(sizeof(*view));
    AwState *candidate = malloc(sizeof(*candidate));
    if (view == NULL || candidate == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    else
    {
        *view = *workspace;
        *candidate = *workspace->state;
        view->state = candidate;
        SourceRemoveSelected(candidate, change);
        status = UmiAiWorkspaceImportCreate(view, collectionId, collectionTitle, documentId, title, text,
                                            bytes, &import);
    }
    if (status == UMI_STATUS_OK)
    {
        UmiAiWorkspaceImportSummary summary;
        (void)UmiAiWorkspaceImportInspect(import, &summary);
        change->summary.kind = UMI_AI_WORKSPACE_SOURCE_REPLACE;
        change->summary.createsCollection = summary.createsCollection;
        change->summary.afterCount = summary.passageCount;
        (void)AwTextCopy(change->collection.id, sizeof(change->collection.id), collectionId, false);
        (void)AwTextCopy(change->collection.title, sizeof(change->collection.title), collectionTitle, false);
        bool same = !summary.createsCollection && count == summary.passageCount;
        for (size_t i = 0U; i < summary.passageCount; ++i)
        {
            UmiAiWorkspaceImportPassage passage;
            (void)UmiAiWorkspaceImportPassageAt(import, i, &passage);
            change->after[i] = passage.source;
            bool found = false;
            for (size_t j = 0U; j < count; ++j)
                if (SourceSameContent(&change->before[j], &passage.source))
                    found = true;
            if (!found)
                same = false;
        }
        change->summary.hasChanges = !same;
        /* An unchanged preview shows the revisions that will actually remain
         * stored, not the candidate revision assigned by the import parser. */
        if (same)
            for (size_t i = 0U; i < count; ++i)
                for (size_t j = 0U; j < count; ++j)
                    if (strcmp(change->after[i].id, change->before[j].id) == 0)
                        change->after[i].revision = change->before[j].revision;
    }
    UmiAiWorkspaceImportDestroy(import);
    free(candidate);
    free(view);
    if (status != UMI_STATUS_OK)
    {
        UmiAiWorkspaceSourceChangeDestroy(change);
        *out = NULL;
    }
    return status;
}
UmiStatus UmiAiWorkspaceSourceChangeCheck(UmiAiWorkspace *workspace, const UmiAiWorkspaceSourceChange *change)
{
    if (workspace == NULL || change == NULL || change->owner != workspace)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (change->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK)
        return status;
    if (workspace->state->revision != change->summary.workspaceRevision ||
        workspace->state->corpusRevision != change->summary.corpusRevision)
        return UMI_STATUS_BUSY;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceSourceChangeApply(UmiAiWorkspace *workspace, UmiAiWorkspaceSourceChange *change,
                                          bool approved)
{
    UmiStatus status = UmiAiWorkspaceSourceChangeCheck(workspace, change);
    if (status != UMI_STATUS_OK)
        return status;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    AwState *next = calloc(1U, sizeof(*next));
    if (next == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    if (!change->summary.hasChanges)
    {
        /* Even an unchanged review must not report success against an unseen
         * disk writer. Read a separate snapshot without rebinding job approvals. */
        status = AwLoad(workspace, next);
        if (status == UMI_STATUS_OK && (next->revision != change->summary.workspaceRevision ||
                                        next->corpusRevision != change->summary.corpusRevision))
            status = UMI_STATUS_BUSY;
        free(next);
    }
    else
    {
        *next = *workspace->state;
        SourceRemoveSelected(next, change);
        if (change->summary.createsCollection)
            next->collections[next->collectionCount++] = change->collection;
        for (size_t i = 0U; i < change->summary.afterCount; ++i)
            next->sources[next->sourceCount++] = change->after[i];
        ++next->corpusRevision;
        status = AwPublish(workspace, next, true);
    }
    if (status == UMI_STATUS_OK)
        change->summary.applied = true;
    return status;
}
