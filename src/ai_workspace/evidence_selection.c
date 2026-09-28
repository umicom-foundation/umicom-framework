/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/evidence_selection.c
 * PURPOSE: Validate selected sources at the same Framework boundary as preparation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "evidence_internal.h"
#include <math.h>
#include <string.h>

static bool SourceStringsValid(const UmiAiWorkspaceSource *source)
{
    return source != NULL && AwIdValid(source->id, sizeof source->id) &&
        AwIdValid(source->collectionId, sizeof source->collectionId) &&
        AwTextValid(source->title, sizeof source->title, false) &&
        AwTextValid(source->text, sizeof source->text, false);
}
bool AwEvidenceSourceEqual(const UmiAiWorkspaceSource *a, const UmiAiWorkspaceSource *b)
{
    return SourceStringsValid(a) && SourceStringsValid(b) &&
        a->firstLine == b->firstLine && a->lastLine == b->lastLine &&
        a->revision == b->revision && strcmp(a->id, b->id) == 0 &&
        strcmp(a->collectionId, b->collectionId) == 0 &&
        strcmp(a->title, b->title) == 0 && strcmp(a->text, b->text) == 0;
}
UmiStatus AwEvidenceSelectionCheck(const UmiAiWorkspace *workspace,
    const char *collection, uint64_t corpusRevision,
    const UmiAiWorkspaceEvidence *evidence, size_t count)
{
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK) return status;
    if (!AwIdValid(collection, UMI_AI_WORKSPACE_ID_CAPACITY) || evidence == NULL ||
        count == 0U || count > UMI_AI_WORKSPACE_MAX_EVIDENCE) return UMI_STATUS_INVALID_ARGUMENT;
    if (AwCollectionIndex(workspace->state, collection) == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (corpusRevision != workspace->state->corpusRevision) return UMI_STATUS_BUSY;
    for (size_t i = 0U; i < count; ++i) {
        const UmiAiWorkspaceSource *source = &evidence[i].source;
        if (!SourceStringsValid(source) || !isfinite(evidence[i].score) || evidence[i].score < 0.0 ||
            source->firstLine == 0U || source->lastLine < source->firstLine || source->revision == 0U ||
            strcmp(source->collectionId, collection) != 0) return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t n = 0U; n < i; ++n)
            if (strcmp(evidence[n].source.id, source->id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
        size_t index = AwSourceIndex(workspace->state, source->id);
        if (index == SIZE_MAX || !AwEvidenceSourceEqual(source, &workspace->state->sources[index]))
            return UMI_STATUS_BUSY;
    }
    return UMI_STATUS_OK;
}
