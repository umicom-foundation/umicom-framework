/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/language_runtime/completion_plan.c
 * PURPOSE: Translate one selected completion into an all-or-nothing editor plan.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "completion_catalogue_internal.h"
#include "umicom/editor/text_position.h"
#include <stdio.h>
#include <string.h>

/* Protocol positions count UTF-16 units. Keep them as coordinates until the
 * Editor resolves them against the exact buffer revision that was requested. */
static UmiStatus CompletionPosition(const UmiJsonTree *tree, int object,
                                    UmiLanguageCompletionPosition *position)
{
    int line, character;
    UmiStatus status = CompletionMember(tree, object, "line", &line);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, object, "character", &character);
    if (status == UMI_STATUS_OK)
        status = CompletionInteger(tree, line, &position->line);
    if (status == UMI_STATUS_OK)
        status = CompletionInteger(tree, character, &position->character);
    return status;
}
static int CompletionCompare(UmiLanguageCompletionPosition a, UmiLanguageCompletionPosition b)
{
    if (a.line != b.line)
        return a.line < b.line ? -1 : 1;
    if (a.character != b.character)
        return a.character < b.character ? -1 : 1;
    return 0;
}
static UmiStatus CompletionRange(const UmiJsonTree *tree, int object, UmiLanguageCompletionRange *range)
{
    int start, end;
    UmiStatus status = CompletionMember(tree, object, "start", &start);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, object, "end", &end);
    if (status == UMI_STATUS_OK)
        status = CompletionPosition(tree, start, &range->start);
    if (status == UMI_STATUS_OK)
        status = CompletionPosition(tree, end, &range->end);
    if (status == UMI_STATUS_OK && CompletionCompare(range->start, range->end) > 0)
        status = UMI_STATUS_PARSE_ERROR;
    return status;
}
static int CompletionContains(UmiLanguageCompletionRange range, UmiLanguageCompletionPosition position)
{
    return range.start.line == range.end.line && CompletionCompare(range.start, position) <= 0 &&
           CompletionCompare(position, range.end) <= 0;
}
static UmiStatus CompletionPrimaryRange(const UmiJsonTree *tree, int object, int is_default,
                                        const UmiLanguageCompletionContext *context,
                                        UmiLanguageCompletionRange *range)
{
    int ordinary, insert, replace;
    UmiStatus status = CompletionMember(tree, object, is_default ? "start" : "range", &ordinary);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, object, "insert", &insert);
    if (status == UMI_STATUS_OK)
        status = CompletionMember(tree, object, "replace", &replace);
    if (status != UMI_STATUS_OK)
        return status;
    if (ordinary >= 0)
    {
        if (insert >= 0 || replace >= 0)
            return UMI_STATUS_PARSE_ERROR;
        status = CompletionRange(tree, is_default ? object : ordinary, range);
    }
    else
    {
        if (insert < 0 || replace < 0)
            return UMI_STATUS_PARSE_ERROR;
        UmiLanguageCompletionRange insertion, replacement;
        status = CompletionRange(tree, insert, &insertion);
        if (status == UMI_STATUS_OK)
            status = CompletionRange(tree, replace, &replacement);
        if (status != UMI_STATUS_OK)
            return status;
        if (!CompletionContains(insertion, context->request_position) ||
            !CompletionContains(replacement, context->request_position) ||
            CompletionCompare(insertion.start, replacement.start) != 0 ||
            CompletionCompare(insertion.end, replacement.end) > 0)
            return UMI_STATUS_PARSE_ERROR;
        *range = context->acceptance == UMI_LANGUAGE_COMPLETION_INSERT ? insertion : replacement;
    }
    if (status == UMI_STATUS_OK && !CompletionContains(*range, context->request_position))
        status = UMI_STATUS_PARSE_ERROR;
    return status;
}
static UmiStatus CompletionStage(UmiEditorWorkspaceEditSet *edits,
                                 const UmiLanguageCompletionContext *context,
                                 UmiLanguageCompletionRange range, const char *text, size_t index)
{
    UmiEditorWorkspaceTextEdit edit = {0};
    edit.struct_size = (uint32_t)sizeof(edit);
    edit.api_version = UMI_EDITOR_WORKSPACE_EDIT_API_VERSION;
    if (strlen(context->provider_id) >= sizeof(edit.provider_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    memcpy(edit.provider_id, context->provider_id, strlen(context->provider_id) + 1U);
    if (index == 0U)
        (void)snprintf(edit.id, sizeof(edit.id), "lsp.completion.primary");
    else
        (void)snprintf(edit.id, sizeof(edit.id), "lsp.completion.additional.%zu", index);
    UmiStatus status = umi_editor_source_location_initialize(&edit.location, context->document_uri,
                                                             range.start.line, range.start.character);
    if (status != UMI_STATUS_OK)
        return status;
    edit.location.kind = UMI_EDITOR_SOURCE_LOCATION_EDIT;
    edit.location.end_line = range.end.line;
    edit.location.end_column = range.end.character;
    memcpy(edit.replacement_text, text, strlen(text) + 1U);
    edit.state = UMI_EDITOR_WORKSPACE_EDIT_UNRESOLVED;
    edit.required = 1;
    return umi_editor_workspace_edit_set_upsert_unresolved(edits, &edit);
}
/* The protocol excludes two insertions at one position. The general Editor
 * also serves other producers, so apply the stricter completion rule here.
 * Insertions at a replacement's end are adjacent; ones at its start conflict. */
static UmiStatus CompletionCheckOverlap(const UmiEditorWorkspaceEditSet *edits,
                                        const UmiCancellationToken *cancel)
{
    size_t count = umi_editor_workspace_edit_set_count(edits);
    for (size_t i = 0U; i < count; ++i)
    {
        if (CompletionCancelled(cancel))
            return UMI_STATUS_CANCELLED;
        UmiEditorWorkspaceTextEdit left;
        UmiStatus status = umi_editor_workspace_edit_set_at(edits, i, &left);
        if (status != UMI_STATUS_OK)
            return status;
        for (size_t j = i + 1U; j < count; ++j)
        {
            UmiEditorWorkspaceTextEdit right;
            status = umi_editor_workspace_edit_set_at(edits, j, &right);
            if (status != UMI_STATUS_OK)
                return status;
            uint64_t a = left.location.byte_offset, b = left.location.end_byte_offset;
            uint64_t c = right.location.byte_offset, d = right.location.end_byte_offset;
            if (a == c || (a < d && c < b))
                return UMI_STATUS_INVALID_STATE;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiLanguageCompletionPlanCreate(const UmiLanguageCompletionCatalogue *catalogue, size_t index,
                                          const UmiLanguageCompletionContext *context,
                                          const UmiEditorTextBuffer *buffer,
                                          const UmiCancellationToken *cancel,
                                          UmiEditorWorkspaceEditSet **out_edits)
{
    if (out_edits == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out_edits = NULL;
    if (catalogue == NULL || context == NULL || buffer == NULL || context->document_uri == NULL ||
        context->document_uri[0] == '\0' || context->provider_id == NULL || context->provider_id[0] == '\0' ||
        (context->acceptance != UMI_LANGUAGE_COMPLETION_INSERT &&
         context->acceptance != UMI_LANGUAGE_COMPLETION_REPLACE) ||
        context->request_position.line > INT32_MAX || context->request_position.character > INT32_MAX)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (CompletionCancelled(cancel))
        return UMI_STATUS_CANCELLED;
    if (umi_editor_text_buffer_revision(buffer) != context->request_revision)
        return UMI_STATUS_INVALID_STATE;
    /* Validate the cursor as well as the replacement endpoints. A range can
     * contain a numeric column that would split a UTF-16 surrogate pair. */
    UmiEditorTextBufferView view;
    UmiStatus position_status = umi_editor_text_buffer_view(buffer, &view);
    size_t cursor_offset;
    if (position_status == UMI_STATUS_OK)
        position_status = UmiEditorTextViewResolvePosition(
            &view,
            (UmiEditorTextPosition){context->request_position.line, context->request_position.character},
            &cursor_offset);
    if (position_status != UMI_STATUS_OK)
        return position_status;
    UmiLanguageCompletionChoice choice;
    UmiStatus status = UmiLanguageCompletionCatalogueAt(catalogue, index, &choice);
    if (status != UMI_STATUS_OK)
        return status;
    if (choice.insert_text_format != 1U || choice.insert_text_mode != 1U || choice.has_command)
        return UMI_STATUS_NOT_IMPLEMENTED;
    if (choice.additional_edit_count > 64U)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    const UmiJsonTree *tree = catalogue->tree;
    int item = catalogue->items[index], edit, range_node, text_node;
    char replacement[UMI_EDITOR_WORKSPACE_EDIT_TEXT_CAPACITY];
    UmiLanguageCompletionRange range;
    status = CompletionMember(tree, item, "textEdit", &edit);
    if (status != UMI_STATUS_OK)
        return status;
    if (edit >= 0)
    {
        status = CompletionPrimaryRange(tree, edit, 0, context, &range);
        if (status == UMI_STATUS_OK)
            status = CompletionMember(tree, edit, "newText", &text_node);
    }
    else
    {
        status = CompletionMember(tree, catalogue->defaults, "editRange", &range_node);
        if (status != UMI_STATUS_OK)
            return status;
        if (range_node >= 0)
        {
            status = CompletionPrimaryRange(tree, range_node, 1, context, &range);
            if (status == UMI_STATUS_OK)
                status = CompletionMember(tree, item, "textEditText", &text_node);
            /* With a default editRange the protocol's fallback is label,
             * not insertText. An explicit empty textEditText is still used. */
        }
        else
        {
            range = context->fallback_range;
            if (range.start.line > INT32_MAX || range.start.character > INT32_MAX ||
                range.end.line > INT32_MAX || range.end.character > INT32_MAX ||
                !CompletionContains(range, context->request_position))
                return UMI_STATUS_INVALID_ARGUMENT;
            status = CompletionMember(tree, item, "insertText", &text_node);
        }
        if (status == UMI_STATUS_OK && text_node < 0)
            status = CompletionMember(tree, item, "label", &text_node);
    }
    if (status == UMI_STATUS_OK)
        status = CompletionText(tree, text_node, replacement, sizeof(replacement));
    if (status != UMI_STATUS_OK)
        return status;
    UmiEditorWorkspaceEditSet *edits = NULL;
    status = umi_editor_workspace_edit_set_create(&edits);
    if (status != UMI_STATUS_OK)
        return status;
    status = CompletionStage(edits, context, range, replacement, 0U);
    if (status != UMI_STATUS_OK)
        goto failure;
    int additional;
    status = CompletionMember(tree, item, "additionalTextEdits", &additional);
    if (status != UMI_STATUS_OK)
        goto failure;
    int node = UmiJsonTreeFirst(tree, additional);
    for (size_t i = 0U; i < choice.additional_edit_count; ++i)
    {
        if (CompletionCancelled(cancel))
        {
            status = UMI_STATUS_CANCELLED;
            goto failure;
        }
        status = CompletionMember(tree, node, "range", &range_node);
        if (status == UMI_STATUS_OK)
            status = CompletionRange(tree, range_node, &range);
        if (status == UMI_STATUS_OK)
            status = CompletionMember(tree, node, "newText", &text_node);
        if (status == UMI_STATUS_OK)
            status = CompletionText(tree, text_node, replacement, sizeof(replacement));
        if (status == UMI_STATUS_OK)
            status = CompletionStage(edits, context, range, replacement, i + 1U);
        if (status != UMI_STATUS_OK)
            goto failure;
        node = UmiJsonTreeNext(tree, node);
    }
    /* Resolve into a private candidate. Only publish after every range, span,
     * overlap and revision has been checked; the user's buffer is never edited. */
    if (CompletionCancelled(cancel))
    {
        status = UMI_STATUS_CANCELLED;
        goto failure;
    }
    status = umi_editor_workspace_edit_set_resolve_document(edits, context->document_uri, buffer);
    if (status == UMI_STATUS_OK)
        status = CompletionCheckOverlap(edits, cancel);
    if (status != UMI_STATUS_OK)
        goto failure;
    if (umi_editor_text_buffer_revision(buffer) != context->request_revision)
    {
        status = UMI_STATUS_INVALID_STATE;
        goto failure;
    }
    *out_edits = edits;
    return UMI_STATUS_OK;
failure:
    umi_editor_workspace_edit_set_destroy(edits);
    return status;
}
