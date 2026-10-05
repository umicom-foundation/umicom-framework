/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/replacement_set.c
 * PURPOSE: Compose literal search and the shared source transaction into a complete-set review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/replacement_set.h"
#include "search_options_internal.h"
#include "umicom/document/text_encoding.h"
#include "umicom/editor/search_engine.h"
#include <stdlib.h>
#include <string.h>

struct UmiDocumentReplacementSet
{
    UmiDocumentCoordinator *coordinator;
    UmiDocumentSourceBatch *batch;
    UmiDocumentReplacementSetSummary summary;
    size_t matches[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    unsigned char reviewed[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
};
static UmiStatus ReplacementSetInput(const char *text, int allow_empty, size_t *out_bytes)
{
    if (text == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t bytes = 0U;
    while (bytes <= UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES && text[bytes] != '\0')
        ++bytes;
    if (bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if ((!allow_empty && bytes == 0U) ||
        !umi_document_utf8_validate((const unsigned char *)text, bytes, NULL))
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_bytes = bytes;
    return UMI_STATUS_OK;
}
void UmiDocumentReplacementSetDestroy(UmiDocumentReplacementSet *set)
{
    if (set == NULL)
        return;
    UmiDocumentSourceBatchDestroy(set->batch);
    free(set);
}
/* All captured drafts now share a copied explicit search policy; the original entry point continues to choose smart case.
 * The former implementation is retained for engineering review. */
#if 0
UmiStatus UmiDocumentReplacementSetCreate(UmiDocumentCoordinator *coordinator, const char *needle,
                                          const char *replacement, UmiDocumentReplacementSet **out_set)
{
    if (out_set == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_set = NULL;
    if (coordinator == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t needle_bytes = 0U, replacement_bytes = 0U;
    UmiStatus status = ReplacementSetInput(needle, 0, &needle_bytes);
    if (status == UMI_STATUS_OK)
        status = ReplacementSetInput(replacement, 1, &replacement_bytes);
    if (status != UMI_STATUS_OK)
        return status;
    size_t count = umi_document_coordinator_count(coordinator);
    if (count == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (count > UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentId ids[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDocumentWorkingCopySnapshot snapshot;
        status = umi_document_coordinator_at(coordinator, i, &snapshot);
        if (status != UMI_STATUS_OK)
            return status;
        ids[i] = snapshot.document_id;
    }
    UmiDocumentReplacementSet *set = calloc(1U, sizeof(*set));
    if (set == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    set->coordinator = coordinator;
    status = UmiDocumentSourceBatchCreate(coordinator, ids, count, &set->batch);
    UmiEditorSearchOptions options = {UMI_EDITOR_SEARCH_CASE_SMART, 0, 0, 0};
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        const char *source = NULL;
        size_t source_bytes = 0U, proposed_bytes = 0U;
        UmiDocumentSourceRequestSummary document;
        UmiDocumentSourceBatchSummary batch;
        status = UmiDocumentSourceBatchRead(set->batch, i, &source, &source_bytes);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceBatchAt(set->batch, i, &document);
        if (status == UMI_STATUS_OK)
            status =
                UmiEditorSearchReplaceAllSize(source, source_bytes, needle, needle_bytes, replacement_bytes,
                                              &options, &proposed_bytes, &set->matches[i]);
        if (status == UMI_STATUS_OK && (proposed_bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES ||
                                        set->matches[i] > SIZE_MAX - set->summary.match_count))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status != UMI_STATUS_OK)
            break;
        char *proposed = malloc(proposed_bytes + 1U);
        if (proposed == NULL)
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        status = UmiEditorSearchReplaceAll(source, source_bytes, needle, needle_bytes, replacement,
                                           replacement_bytes, &options, proposed, proposed_bytes + 1U, NULL);
        /* Keep the caret when it remains representable, moving back only when
         * a shorter result ends inside a UTF-8 character or CRLF boundary. */
        size_t cursor = document.cursor_offset < proposed_bytes ? document.cursor_offset : proposed_bytes;
        if (status == UMI_STATUS_OK)
        {
            while (cursor > 0U && cursor < proposed_bytes &&
                   ((unsigned char)proposed[cursor] & 0xc0U) == 0x80U)
                --cursor;
            if (cursor > 0U && cursor < proposed_bytes && proposed[cursor - 1U] == '\r' &&
                proposed[cursor] == '\n')
                --cursor;
            status = UmiDocumentSourceBatchInspect(set->batch, &batch);
        }
        if (status == UMI_STATUS_OK)
            status =
                UmiDocumentSourceBatchStage(set->batch, i, batch.revision, proposed, proposed_bytes, cursor);
        free(proposed);
        if (status == UMI_STATUS_OK)
            set->summary.match_count += set->matches[i];
    }
    UmiDocumentSourceBatchSummary batch;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchInspect(set->batch, &batch);
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentReplacementSetDestroy(set);
        return status;
    }
    set->summary.document_count = count;
    set->summary.changed_count = batch.changed_count;
    set->summary.revision = batch.revision;
    *out_set = set;
    return UMI_STATUS_OK;
}
#endif
UmiStatus UmiDocumentReplacementSetCreateWithOptions(UmiDocumentCoordinator *coordinator, const char *needle,
                                          const char *replacement, const UmiEditorSearchOptions *searchOptions, UmiDocumentReplacementSet **out_set)
{
    if (out_set == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_set = NULL;
    UmiEditorSearchOptions options;
    UmiStatus policy = DocumentSearchOptions(searchOptions, &options);
    if (policy != UMI_STATUS_OK) return policy;
    if (coordinator == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    size_t needle_bytes = 0U, replacement_bytes = 0U;
    UmiStatus status = ReplacementSetInput(needle, 0, &needle_bytes);
    if (status == UMI_STATUS_OK)
        status = ReplacementSetInput(replacement, 1, &replacement_bytes);
    if (status != UMI_STATUS_OK)
        return status;
    size_t count = umi_document_coordinator_count(coordinator);
    if (count == 0U)
        return UMI_STATUS_NOT_FOUND;
    if (count > UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiDocumentId ids[UMI_DOCUMENT_SOURCE_BATCH_MAXIMUM];
    for (size_t i = 0U; i < count; ++i)
    {
        UmiDocumentWorkingCopySnapshot snapshot;
        status = umi_document_coordinator_at(coordinator, i, &snapshot);
        if (status != UMI_STATUS_OK)
            return status;
        ids[i] = snapshot.document_id;
    }
    UmiDocumentReplacementSet *set = calloc(1U, sizeof(*set));
    if (set == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    set->coordinator = coordinator;
    status = UmiDocumentSourceBatchCreate(coordinator, ids, count, &set->batch);
    for (size_t i = 0U; status == UMI_STATUS_OK && i < count; ++i)
    {
        const char *source = NULL;
        size_t source_bytes = 0U, proposed_bytes = 0U;
        UmiDocumentSourceRequestSummary document;
        UmiDocumentSourceBatchSummary batch;
        status = UmiDocumentSourceBatchRead(set->batch, i, &source, &source_bytes);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentSourceBatchAt(set->batch, i, &document);
        if (status == UMI_STATUS_OK)
            status =
                UmiEditorSearchReplaceAllSize(source, source_bytes, needle, needle_bytes, replacement_bytes,
                                              &options, &proposed_bytes, &set->matches[i]);
        if (status == UMI_STATUS_OK && (proposed_bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES ||
                                        set->matches[i] > SIZE_MAX - set->summary.match_count))
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status != UMI_STATUS_OK)
            break;
        char *proposed = malloc(proposed_bytes + 1U);
        if (proposed == NULL)
        {
            status = UMI_STATUS_OUT_OF_MEMORY;
            break;
        }
        status = UmiEditorSearchReplaceAll(source, source_bytes, needle, needle_bytes, replacement,
                                           replacement_bytes, &options, proposed, proposed_bytes + 1U, NULL);
        /* Keep the caret when it remains representable, moving back only when
         * a shorter result ends inside a UTF-8 character or CRLF boundary. */
        size_t cursor = document.cursor_offset < proposed_bytes ? document.cursor_offset : proposed_bytes;
        if (status == UMI_STATUS_OK)
        {
            while (cursor > 0U && cursor < proposed_bytes &&
                   ((unsigned char)proposed[cursor] & 0xc0U) == 0x80U)
                --cursor;
            if (cursor > 0U && cursor < proposed_bytes && proposed[cursor - 1U] == '\r' &&
                proposed[cursor] == '\n')
                --cursor;
            status = UmiDocumentSourceBatchInspect(set->batch, &batch);
        }
        if (status == UMI_STATUS_OK)
            status =
                UmiDocumentSourceBatchStage(set->batch, i, batch.revision, proposed, proposed_bytes, cursor);
        free(proposed);
        if (status == UMI_STATUS_OK)
            set->summary.match_count += set->matches[i];
    }
    UmiDocumentSourceBatchSummary batch;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchInspect(set->batch, &batch);
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentReplacementSetDestroy(set);
        return status;
    }
    set->summary.document_count = count;
    set->summary.changed_count = batch.changed_count;
    set->summary.revision = batch.revision;
    *out_set = set;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentReplacementSetCreate(UmiDocumentCoordinator *coordinator, const char *needle,
    const char *replacement, UmiDocumentReplacementSet **out_set)
{
    return UmiDocumentReplacementSetCreateWithOptions(coordinator, needle, replacement, NULL, out_set);
}

UmiStatus UmiDocumentReplacementSetInspect(const UmiDocumentReplacementSet *set,
                                           UmiDocumentReplacementSetSummary *out_summary)
{
    if (set == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_summary = set->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentReplacementSetAt(const UmiDocumentReplacementSet *set, size_t index,
                                      UmiDocumentSourceRequestSummary *out_document, size_t *out_matches,
                                      int *out_reviewed)
{
    if (set == NULL || out_document == NULL || out_matches == NULL || out_reviewed == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = UmiDocumentSourceBatchAt(set->batch, index, out_document);
    if (status == UMI_STATUS_OK)
    {
        *out_matches = set->matches[index];
        *out_reviewed = set->reviewed[index] != 0U;
    }
    return status;
}
UmiStatus UmiDocumentReplacementSetTexts(const UmiDocumentReplacementSet *set, size_t index,
                                         const char **out_previous, size_t *out_previous_bytes,
                                         const char **out_proposed, size_t *out_proposed_bytes)
{
    if (out_previous != NULL)
        *out_previous = NULL;
    if (out_previous_bytes != NULL)
        *out_previous_bytes = 0U;
    if (out_proposed != NULL)
        *out_proposed = NULL;
    if (out_proposed_bytes != NULL)
        *out_proposed_bytes = 0U;
    if (set == NULL || out_previous == NULL || out_previous_bytes == NULL || out_proposed == NULL ||
        out_proposed_bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    const char *previous = NULL, *proposed = NULL;
    size_t previous_bytes = 0U, proposed_bytes = 0U;
    UmiStatus status = UmiDocumentSourceBatchRead(set->batch, index, &previous, &previous_bytes);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchProposed(set->batch, index, &proposed, &proposed_bytes);
    if (status == UMI_STATUS_OK)
    {
        *out_previous = previous;
        *out_previous_bytes = previous_bytes;
        *out_proposed = proposed;
        *out_proposed_bytes = proposed_bytes;
    }
    return status;
}
UmiStatus UmiDocumentReplacementSetCheck(const UmiDocumentReplacementSet *set)
{
    return set != NULL ? UmiDocumentSourceBatchCheck(set->coordinator, set->batch)
                       : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus UmiDocumentReplacementSetReview(UmiDocumentReplacementSet *set, size_t index,
                                          uint64_t presented_revision)
{
    if (set == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (index >= set->summary.document_count)
        return UMI_STATUS_NOT_FOUND;
    if (set->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    if (presented_revision != set->summary.revision)
        return UMI_STATUS_BUSY;
    UmiStatus status = UmiDocumentReplacementSetCheck(set);
    UmiDocumentSourceRequestSummary document;
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceBatchAt(set->batch, index, &document);
    if (status == UMI_STATUS_OK && !set->reviewed[index])
    {
        set->reviewed[index] = 1U;
        if (document.text_changes)
            ++set->summary.reviewed_count;
    }
    return status;
}
UmiStatus UmiDocumentReplacementSetApply(UmiDocumentReplacementSet *set, uint64_t reviewed_revision,
                                         int approved)
{
    if (set == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    if (set->summary.applied)
        return UMI_STATUS_INVALID_STATE;
    if (reviewed_revision != set->summary.revision)
        return UMI_STATUS_BUSY;
    if (set->summary.reviewed_count != set->summary.changed_count)
        return UMI_STATUS_PERMISSION_DENIED;
    UmiStatus status = UmiDocumentSourceBatchApply(set->coordinator, set->batch, reviewed_revision, 1);
    if (status == UMI_STATUS_OK)
        set->summary.applied = 1;
    return status;
}
