/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_workspace/import.c
 * PURPOSE: Prepare citation-aware text passages and publish them through one existing workspace transaction.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai_workspace/import.h"
#include "workspace_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiAiWorkspaceImport
{
    UmiAiWorkspace *owner;
    UmiAiWorkspaceImportSummary summary;
    UmiAiWorkspaceImportPassage passages[UMI_AI_WORKSPACE_MAX_SOURCES];
};
void UmiAiWorkspaceImportDestroy(UmiAiWorkspaceImport *import) { free(import); }
UmiStatus UmiAiWorkspaceImportInspect(const UmiAiWorkspaceImport *import, UmiAiWorkspaceImportSummary *out)
{
    if (import == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = import->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceImportPassageAt(const UmiAiWorkspaceImport *import, size_t index,
                                        UmiAiWorkspaceImportPassage *out)
{
    if (import == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= import->summary.passageCount)
        return UMI_STATUS_NOT_FOUND;
    *out = import->passages[index];
    return UMI_STATUS_OK;
}
/* Prefer a physical line ending, then a whole Unicode character. This keeps
 * ordinary paragraphs readable while still accepting a very long line. */
static size_t ImportBoundary(const char *text, size_t remaining)
{
    size_t count = UMI_AI_WORKSPACE_PASSAGE_CAPACITY - 1U;
    if (remaining <= count)
        return remaining;
    while (((unsigned char)text[count] & 0xc0U) == 0x80U)
        --count;
    for (size_t i = count; i != 0U; --i)
        if (text[i - 1U] == '\n')
            return i;
    return count;
}
UmiStatus UmiAiWorkspaceImportCreate(UmiAiWorkspace *workspace, const char *collectionId,
                                     const char *collectionTitle, const char *documentId, const char *title,
                                     const char *text, size_t byteCount, UmiAiWorkspaceImport **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK)
        return status;
    if (byteCount > UMI_AI_WORKSPACE_IMPORT_MAX_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (text == NULL || byteCount == 0U || !AwIdValid(collectionId, UMI_AI_WORKSPACE_ID_CAPACITY) ||
        !AwIdValid(documentId, 56U) ||
        !AwTextValid(collectionTitle, UMI_AI_WORKSPACE_TITLE_CAPACITY, false) ||
        !AwTextValid(title, UMI_AI_WORKSPACE_TITLE_CAPACITY, false) || memchr(text, '\0', byteCount) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (workspace->state->revision == UINT64_MAX || workspace->state->corpusRevision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t collection = AwCollectionIndex(workspace->state, collectionId);
    if (collection != SIZE_MAX &&
        strcmp(workspace->state->collections[collection].title, collectionTitle) != 0)
        return UMI_STATUS_ALREADY_EXISTS;
    if (collection == SIZE_MAX && workspace->state->collectionCount == UMI_AI_WORKSPACE_MAX_COLLECTIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *normalized = malloc(byteCount + 1U);
    if (normalized == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    size_t start = byteCount >= 3U && memcmp(text, "\xef\xbb\xbf", 3U) == 0 ? 3U : 0U, length = 0U;
    for (size_t i = start; i < byteCount; ++i)
    {
        char value = text[i];
        if (value == '\r')
        {
            value = '\n';
            if (i + 1U < byteCount && text[i + 1U] == '\n')
                ++i;
        }
        normalized[length++] = value;
    }
    normalized[length] = '\0';
    if (!AwTextValid(normalized, length + 1U, false))
    {
        free(normalized);
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiAiWorkspaceImport *import = calloc(1U, sizeof(*import));
    if (import == NULL)
    {
        free(normalized);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    import->owner = workspace;
    UmiAiWorkspaceImportSummary *summary = &import->summary;
    (void)AwTextCopy(summary->collectionId, sizeof(summary->collectionId), collectionId, false);
    (void)AwTextCopy(summary->collectionTitle, sizeof(summary->collectionTitle), collectionTitle, false);
    (void)AwTextCopy(summary->documentId, sizeof(summary->documentId), documentId, false);
    (void)AwTextCopy(summary->title, sizeof(summary->title), title, false);
    summary->inputBytes = byteCount;
    summary->textBytes = length;
    summary->workspaceRevision = workspace->state->revision;
    summary->corpusRevision = workspace->state->corpusRevision;
    summary->createsCollection = collection == SIZE_MAX;
    size_t offset = 0U;
    uint32_t line = 1U;
    while (offset < length)
    {
        if (summary->passageCount >= UMI_AI_WORKSPACE_MAX_SOURCES - workspace->state->sourceCount)
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        UmiAiWorkspaceImportPassage *passage = &import->passages[summary->passageCount];
        UmiAiWorkspaceSource *source = &passage->source;
        int written =
            snprintf(source->id, sizeof(source->id), "%s.part.%zu", documentId, summary->passageCount + 1U);
        if (written < 0 || (size_t)written >= sizeof(source->id))
        {
            status = UMI_STATUS_CAPACITY_EXCEEDED;
            break;
        }
        if (AwSourceIndex(workspace->state, source->id) != SIZE_MAX)
        {
            status = UMI_STATUS_ALREADY_EXISTS;
            break;
        }
        (void)AwTextCopy(source->collectionId, sizeof(source->collectionId), collectionId, false);
        (void)AwTextCopy(source->title, sizeof(source->title), title, false);
        passage->textOffset = offset;
        passage->byteCount = ImportBoundary(normalized + offset, length - offset);
        memcpy(source->text, normalized + offset, passage->byteCount);
        source->firstLine = line;
        source->lastLine = line;
        for (size_t i = 0U; i < passage->byteCount; ++i)
            if (source->text[i] == '\n')
            {
                ++line;
                if (i + 1U < passage->byteCount)
                    source->lastLine = line;
            }
        source->revision = summary->corpusRevision + 1U;
        ++summary->passageCount;
        offset += passage->byteCount;
    }
    free(normalized);
    if (status != UMI_STATUS_OK)
    {
        UmiAiWorkspaceImportDestroy(import);
        return status;
    }
    *out = import;
    return UMI_STATUS_OK;
}
UmiStatus UmiAiWorkspaceImportApply(UmiAiWorkspace *workspace, UmiAiWorkspaceImport *import, bool approved)
{
    if (workspace == NULL || import == NULL || workspace != import->owner)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (import->summary.saved)
        return UMI_STATUS_INVALID_STATE;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    UmiStatus status = AwReady(workspace);
    if (status != UMI_STATUS_OK)
        return status;
    if (workspace->state->revision != import->summary.workspaceRevision ||
        workspace->state->corpusRevision != import->summary.corpusRevision)
        return UMI_STATUS_BUSY;
    /* Stage all passages in a private state, then let AwPublish perform one
     * compare-and-commit. Failure cannot leave half an imported document. */
    AwState *next = malloc(sizeof(*next));
    if (next == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    *next = *workspace->state;
    if (import->summary.createsCollection)
    {
        UmiAiWorkspaceCollection *collection = &next->collections[next->collectionCount++];
        (void)AwTextCopy(collection->id, sizeof(collection->id), import->summary.collectionId, false);
        (void)AwTextCopy(collection->title, sizeof(collection->title), import->summary.collectionTitle,
                         false);
    }
    for (size_t i = 0U; i < import->summary.passageCount; ++i)
        next->sources[next->sourceCount++] = import->passages[i].source;
    ++next->corpusRevision;
    status = AwPublish(workspace, next, true);
    if (status == UMI_STATUS_OK)
        import->summary.saved = true;
    return status;
}
