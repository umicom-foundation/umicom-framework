/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/snippet_review.c
 * PURPOSE: Compose reviewed snippet insertions through the shared document owner without duplicating history or persistence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/snippet_review.h"
#include "umicom/editor/text_position.h"
#include <stdlib.h>
#include <string.h>

struct UmiDocumentSnippetReview
{
    UmiDocumentSourceRequest *source;
    UmiEditorSnippetSession *session;
    uint64_t revision, staged_revision;
    int has_template, prepared, applied;
};
void UmiDocumentSnippetReviewDestroy(UmiDocumentSnippetReview *review)
{
    if (review == NULL)
        return;
    UmiDocumentSourceRequestDestroy(review->source);
    umi_editor_snippet_session_destroy(review->session);
    free(review);
}
UmiStatus UmiDocumentSnippetReviewCreate(UmiDocumentCoordinator *coordinator, UmiDocumentId document_id,
                                         UmiDocumentSnippetReview **out_review)
{
    if (out_review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_review = NULL;
    UmiDocumentSnippetReview *review = calloc(1U, sizeof(*review));
    if (review == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiDocumentSourceRequestCreate(coordinator, document_id, &review->source);
    if (status == UMI_STATUS_OK)
        status = umi_editor_snippet_session_create(&review->session);
    if (status != UMI_STATUS_OK)
    {
        UmiDocumentSnippetReviewDestroy(review);
        return status;
    }
    review->revision = 1U;
    *out_review = review;
    return UMI_STATUS_OK;
}
UmiStatus UmiDocumentSnippetReviewInspect(const UmiDocumentSnippetReview *review,
                                          UmiDocumentSnippetSummary *out_summary)
{
    if (review == NULL || out_summary == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiDocumentSnippetSummary summary = {0};
    UmiStatus status = UmiDocumentSourceRequestInspect(review->source, &summary.source);
    UmiEditorSnippetSessionSnapshot snippet;
    if (status == UMI_STATUS_OK)
        status = umi_editor_snippet_session_snapshot(review->session, &snippet);
    if (status != UMI_STATUS_OK)
        return status;
    summary.revision = review->revision;
    summary.placeholder_count = snippet.placeholder_count;
    summary.expanded_bytes = snippet.expanded_length;
    summary.has_template = review->has_template;
    summary.prepared = review->prepared;
    summary.applied = review->applied;
    /* The inner owner may retain an older proposal after values are edited.
     * Hide its presentation fields until a new complete proposal is prepared. */
    if (!review->prepared)
    {
        summary.source.has_proposal = 0;
        summary.source.proposed_bytes = 0U;
        summary.source.proposed_cursor = 0U;
        summary.source.text_changes = 0;
    }
    *out_summary = summary;
    return UMI_STATUS_OK;
}
static UmiStatus SnippetReviewEditable(const UmiDocumentSnippetReview *review, uint64_t expected_revision,
                                       const UmiCancellationToken *cancel)
{
    if (review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (review->applied)
        return UMI_STATUS_INVALID_STATE;
    if (review->revision != expected_revision)
        return UMI_STATUS_BUSY;
    if (review->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    return UMI_STATUS_OK;
}
static void SnippetReviewChanged(UmiDocumentSnippetReview *review)
{
    ++review->revision;
    review->prepared = 0;
    review->staged_revision = 0U;
}
UmiStatus UmiDocumentSnippetReviewTemplate(UmiDocumentSnippetReview *review, uint64_t expected_revision,
                                           const UmiEditorSnippetTemplate *snippet,
                                           const UmiCancellationToken *cancel)
{
    UmiStatus status = SnippetReviewEditable(review, expected_revision, cancel);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDocumentSourceRequestSummary source;
    status = UmiDocumentSourceRequestInspect(review->source, &source);
    if (status == UMI_STATUS_OK)
        status = UmiEditorSnippetSessionRestart(review->session,
                                                umi_editor_snippet_session_revision(review->session), snippet,
                                                source.cursor_offset, cancel);
    if (status == UMI_STATUS_OK)
    {
        review->has_template = 1;
        SnippetReviewChanged(review);
    }
    return status;
}
UmiStatus UmiDocumentSnippetReviewValue(UmiDocumentSnippetReview *review, uint64_t expected_revision,
                                        uint32_t ordinal, const char *text, size_t bytes,
                                        const UmiCancellationToken *cancel)
{
    UmiStatus status = SnippetReviewEditable(review, expected_revision, cancel);
    if (status != UMI_STATUS_OK)
        return status;
    if (!review->has_template)
        return UMI_STATUS_INVALID_STATE;
    status = UmiEditorSnippetSessionReplace(
        review->session, umi_editor_snippet_session_revision(review->session), ordinal, text, bytes, cancel);
    if (status == UMI_STATUS_OK)
        SnippetReviewChanged(review);
    return status;
}
UmiStatus UmiDocumentSnippetReviewPlaceholder(const UmiDocumentSnippetReview *review, size_t index,
                                              UmiEditorSnippetPlaceholder *out_placeholder)
{
    if (review == NULL || out_placeholder == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (!review->has_template)
        return UMI_STATUS_INVALID_STATE;
    return umi_editor_snippet_session_placeholder_at(review->session, index, out_placeholder);
}
static UmiStatus SnippetReviewTextArguments(const UmiDocumentSnippetReview *review, const char **out_text,
                                            size_t *out_bytes)
{
    if (out_text != NULL)
        *out_text = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    return review == NULL || out_text == NULL || out_bytes == NULL ? UMI_STATUS_INVALID_ARGUMENT
                                                                   : UMI_STATUS_OK;
}
UmiStatus UmiDocumentSnippetReviewOriginal(const UmiDocumentSnippetReview *review, const char **out_text,
                                           size_t *out_bytes)
{
    UmiStatus status = SnippetReviewTextArguments(review, out_text, out_bytes);
    return status == UMI_STATUS_OK ? UmiDocumentSourceRequestRead(review->source, out_text, out_bytes)
                                   : status;
}
UmiStatus UmiDocumentSnippetReviewExpanded(const UmiDocumentSnippetReview *review, const char **out_text,
                                           size_t *out_bytes)
{
    UmiStatus status = SnippetReviewTextArguments(review, out_text, out_bytes);
    if (status != UMI_STATUS_OK)
        return status;
    if (!review->has_template)
        return UMI_STATUS_INVALID_STATE;
    return UmiEditorSnippetSessionRead(review->session, out_text, out_bytes);
}
UmiStatus UmiDocumentSnippetReviewProposed(const UmiDocumentSnippetReview *review, const char **out_text,
                                           size_t *out_bytes)
{
    UmiStatus status = SnippetReviewTextArguments(review, out_text, out_bytes);
    if (status != UMI_STATUS_OK)
        return status;
    if (!review->prepared || review->applied)
        return UMI_STATUS_INVALID_STATE;
    return UmiDocumentSourceRequestProposed(review->source, out_text, out_bytes);
}
UmiStatus UmiDocumentSnippetReviewCheck(UmiDocumentCoordinator *coordinator,
                                        const UmiDocumentSnippetReview *review)
{
    if (review == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (review->applied)
        return UMI_STATUS_INVALID_STATE;
    return UmiDocumentSourceRequestCheck(coordinator, review->source);
}
UmiStatus UmiDocumentSnippetReviewPrepare(UmiDocumentCoordinator *coordinator,
                                          UmiDocumentSnippetReview *review, uint64_t expected_revision,
                                          const UmiCancellationToken *cancel)
{
    UmiStatus status = SnippetReviewEditable(review, expected_revision, cancel);
    if (status != UMI_STATUS_OK)
        return status;
    if (!review->has_template)
        return UMI_STATUS_INVALID_STATE;
    status = UmiDocumentSnippetReviewCheck(coordinator, review);
    if (status != UMI_STATUS_OK)
        return status;
    UmiDocumentSourceRequestSummary source;
    const char *original = NULL, *expanded = NULL;
    size_t original_bytes = 0U, expanded_bytes = 0U;
    status = UmiDocumentSourceRequestInspect(review->source, &source);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentSourceRequestRead(review->source, &original, &original_bytes);
    if (status == UMI_STATUS_OK)
        status = UmiEditorSnippetSessionRead(review->session, &expanded, &expanded_bytes);
    if (status != UMI_STATUS_OK)
        return status;
    if (source.cursor_offset > original_bytes ||
        source.selection_bytes > original_bytes - source.cursor_offset)
        return UMI_STATUS_INVALID_STATE;
    size_t suffix = source.cursor_offset + source.selection_bytes;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = original;
    view.byte_count = view.capacity = original_bytes;
    UmiEditorTextPosition boundary;
    status = UmiEditorTextViewPositionAt(&view, source.cursor_offset, &boundary);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, suffix, &boundary);
    if (status != UMI_STATUS_OK)
        return status;
    size_t retained = original_bytes - source.selection_bytes;
    if (retained > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES ||
        expanded_bytes > UMI_UI_DOCUMENT_TEXT_MAXIMUM_BYTES - retained)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    size_t bytes = retained + expanded_bytes, cursor = source.cursor_offset + expanded_bytes;
    for (size_t i = 0U; i < umi_editor_snippet_session_placeholder_count(review->session); ++i)
    {
        UmiEditorSnippetPlaceholder placeholder;
        status = umi_editor_snippet_session_placeholder_at(review->session, i, &placeholder);
        if (status != UMI_STATUS_OK)
            return status;
        if (placeholder.final_stop)
        {
            if (placeholder.start_byte_offset > bytes)
                return UMI_STATUS_INVALID_STATE;
            cursor = (size_t)placeholder.start_byte_offset;
            break;
        }
    }
    char *proposed = malloc(bytes + 1U);
    if (proposed == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    /* Assemble prefix, expanded template and untouched suffix from owned source.
     * The document owner performs the only live edit after explicit review. */
    memcpy(proposed, original, source.cursor_offset);
    memcpy(proposed + source.cursor_offset, expanded, expanded_bytes);
    memcpy(proposed + source.cursor_offset + expanded_bytes, original + suffix, original_bytes - suffix);
    proposed[bytes] = '\0';
    if (umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    else
        status = UmiDocumentSourceRequestStage(review->source, source.revision, proposed, bytes, cursor);
    free(proposed);
    if (status == UMI_STATUS_OK)
    {
        ++review->revision;
        review->prepared = 1;
        review->staged_revision = source.revision + 1U;
    }
    return status;
}
UmiStatus UmiDocumentSnippetReviewApply(UmiDocumentCoordinator *coordinator, UmiDocumentSnippetReview *review,
                                        uint64_t reviewed_revision, int approved)
{
    if (review == NULL || coordinator == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (review->applied || !review->prepared)
        return UMI_STATUS_INVALID_STATE;
    if (review->revision != reviewed_revision)
        return UMI_STATUS_BUSY;
    if (!approved)
        return UMI_STATUS_PERMISSION_DENIED;
    UmiStatus status =
        UmiDocumentSourceRequestApply(coordinator, review->source, review->staged_revision, approved);
    if (status == UMI_STATUS_OK)
    {
        review->applied = 1;
        review->prepared = 0;
    }
    return status;
}
