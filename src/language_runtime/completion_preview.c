/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/completion_preview.c
 * PURPOSE: Resolve a completion and its caret on private text before a document review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/language_runtime/completion_preview.h"
#include "umicom/editor/text_position.h"
#include <stdlib.h>
#include <string.h>

#define COMPLETION_PREVIEW_MAXIMUM_BYTES (16U * 1024U * 1024U)
struct UmiLanguageCompletionPreview
{
    UmiEditorTextBuffer *buffer;
    size_t cursor, edits;
};
void UmiLanguageCompletionPreviewDestroy(UmiLanguageCompletionPreview *preview)
{
    if (preview == NULL)
        return;
    umi_editor_text_buffer_destroy(preview->buffer);
    free(preview);
}
/* Track growth and shrinkage separately. Unsigned subtraction of two lengths
 * could otherwise wrap when an import is removed instead of inserted. */
static UmiStatus PreviewAdjustedSize(size_t initial, size_t added, size_t removed, size_t *out_size)
{
    if (removed > initial || added > SIZE_MAX - (initial - removed))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    *out_size = initial - removed + added;
    return UMI_STATUS_OK;
}
static UmiStatus PreviewResultShape(UmiEditorWorkspaceEditSet *edits, size_t source_bytes, size_t *out_cursor)
{
    size_t count = umi_editor_workspace_edit_set_count(edits);
    UmiEditorWorkspaceTextEdit primary = {0};
    int found = 0;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiEditorWorkspaceTextEdit edit;
        UmiStatus status = umi_editor_workspace_edit_set_at(edits, i, &edit);
        if (status != UMI_STATUS_OK)
            return status;
        if (strcmp(edit.id, "lsp.completion.primary") == 0)
        {
            primary = edit;
            found = 1;
        }
    }
    if (!found || primary.location.byte_offset > source_bytes)
        return UMI_STATUS_INVALID_STATE;
    size_t added = 0U, removed = 0U, before_added = 0U, before_removed = 0U;
    for (size_t i = 0U; i < count; ++i)
    {
        UmiEditorWorkspaceTextEdit edit;
        UmiStatus status = umi_editor_workspace_edit_set_at(edits, i, &edit);
        if (status != UMI_STATUS_OK)
            return status;
        if (edit.location.end_byte_offset < edit.location.byte_offset ||
            edit.location.end_byte_offset > source_bytes)
            return UMI_STATUS_INVALID_STATE;
        size_t insertion = strlen(edit.replacement_text);
        size_t deletion = (size_t)(edit.location.end_byte_offset - edit.location.byte_offset);
        if (insertion > SIZE_MAX - added || deletion > SIZE_MAX - removed)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        added += insertion;
        removed += deletion;
        if (strcmp(edit.id, "lsp.completion.primary") != 0 &&
            edit.location.end_byte_offset <= primary.location.byte_offset)
        {
            before_added += insertion;
            before_removed += deletion;
        }
    }
    size_t final_size, start;
    UmiStatus status = PreviewAdjustedSize(source_bytes, added, removed, &final_size);
    if (status != UMI_STATUS_OK)
        return status;
    if (final_size > COMPLETION_PREVIEW_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    status = PreviewAdjustedSize((size_t)primary.location.byte_offset, before_added, before_removed, &start);
    size_t insertion = strlen(primary.replacement_text);
    if (status != UMI_STATUS_OK)
        return status;
    if (start > final_size || insertion > final_size - start)
        return UMI_STATUS_INVALID_STATE;
    *out_cursor = start + insertion;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCompletionPreviewCreate(const UmiLanguageCompletionCatalogue *catalogue, size_t choice,
                                             const char *document_uri, const char *provider_id,
                                             const char *source, size_t source_bytes, size_t cursor_offset,
                                             size_t fallback_begin, size_t fallback_end,
                                             UmiLanguageCompletionAcceptance acceptance,
                                             const UmiCancellationToken *cancel,
                                             UmiLanguageCompletionPreview **out_preview)
{
    if (out_preview == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_preview = NULL;
    if (catalogue == NULL || source == NULL || document_uri == NULL || provider_id == NULL ||
        fallback_begin > cursor_offset || cursor_offset > fallback_end || fallback_end > source_bytes)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (source_bytes > COMPLETION_PREVIEW_MAXIMUM_BYTES)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (memchr(source, '\0', source_bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = source_bytes;
    view.capacity = source_bytes;
    UmiEditorTextPosition end, caret, begin, finish;
    /* Scan to the end as well as to the selected positions. Invalid UTF-8
     * after the caret must not escape into a proposed complete document. */
    UmiStatus status = UmiEditorTextViewPositionAt(&view, source_bytes, &end);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, cursor_offset, &caret);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, fallback_begin, &begin);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, fallback_end, &finish);
    if (status != UMI_STATUS_OK)
        return status;
    if (begin.line != finish.line || caret.line > INT32_MAX || caret.utf16_column > INT32_MAX ||
        begin.utf16_column > INT32_MAX || finish.utf16_column > INT32_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiLanguageCompletionPreview *preview = calloc(1U, sizeof(*preview));
    if (preview == NULL)
        return UMI_STATUS_OUT_OF_MEMORY;
    UmiEditorWorkspaceEditSet *edits = NULL;
    status = umi_editor_text_buffer_create(source_bytes + 1U, &preview->buffer);
    if (status == UMI_STATUS_OK)
        status = umi_editor_text_buffer_set(preview->buffer, source, source_bytes);
    UmiLanguageCompletionContext context = {0};
    context.document_uri = document_uri;
    context.provider_id = provider_id;
    context.request_revision = umi_editor_text_buffer_revision(preview->buffer);
    context.request_position =
        (UmiLanguageCompletionPosition){(uint32_t)caret.line, (uint32_t)caret.utf16_column};
    context.fallback_range.start =
        (UmiLanguageCompletionPosition){(uint32_t)begin.line, (uint32_t)begin.utf16_column};
    context.fallback_range.end =
        (UmiLanguageCompletionPosition){(uint32_t)finish.line, (uint32_t)finish.utf16_column};
    context.acceptance = acceptance;
    if (status == UMI_STATUS_OK)
        status =
            UmiLanguageCompletionPlanCreate(catalogue, choice, &context, preview->buffer, cancel, &edits);
    if (status == UMI_STATUS_OK)
        status = PreviewResultShape(edits, source_bytes, &preview->cursor);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = umi_editor_workspace_edit_set_apply_document(edits, document_uri, preview->buffer, 1,
                                                              &preview->edits);
    if (status == UMI_STATUS_OK)
        status = umi_editor_text_buffer_view(preview->buffer, &view);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, view.byte_count, &end);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, preview->cursor, &caret);
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    umi_editor_workspace_edit_set_destroy(edits);
    if (status != UMI_STATUS_OK)
    {
        UmiLanguageCompletionPreviewDestroy(preview);
        return status;
    }
    *out_preview = preview;
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCompletionPreviewRead(const UmiLanguageCompletionPreview *preview, const char **out_text,
                                           size_t *out_bytes, size_t *out_cursor)
{
    if (out_text != NULL)
        *out_text = NULL;
    if (out_bytes != NULL)
        *out_bytes = 0U;
    if (out_cursor != NULL)
        *out_cursor = 0U;
    if (preview == NULL || out_text == NULL || out_bytes == NULL || out_cursor == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiEditorTextBufferView view;
    UmiStatus status = umi_editor_text_buffer_view(preview->buffer, &view);
    if (status == UMI_STATUS_OK)
    {
        *out_text = view.bytes;
        *out_bytes = view.byte_count;
        *out_cursor = preview->cursor;
    }
    return status;
}
size_t UmiLanguageCompletionPreviewEditCount(const UmiLanguageCompletionPreview *preview)
{
    return preview == NULL ? 0U : preview->edits;
}
