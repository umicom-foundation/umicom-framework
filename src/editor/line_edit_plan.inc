/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/line_edit_plan.inc
 * PURPOSE: Adapt existing line transformations to immutable proposals with bounded input and no private Undo history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/editor/line_edit_plan.h"
#include "umicom/editor/text_position.h"
#define LINE_EDIT_TEXT_LIMIT (8U * 1024U * 1024U)
struct UmiEditorLineEditPlan
{
    char *text;
    UmiEditorLineEditSummary summary;
};
static int LineEditPrefix(UmiEditorEditCommandKind kind)
{
    return kind == UMI_EDITOR_EDIT_COMMAND_INDENT_LINES || kind == UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES ||
           kind == UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT;
}
static int LineEditSupported(UmiEditorEditCommandKind kind)
{
    return LineEditPrefix(kind) || kind == UMI_EDITOR_EDIT_COMMAND_DELETE_LINE ||
           kind == UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE || kind == UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP ||
           kind == UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN ||
           kind == UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT ||
           kind == UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE;
}
static UmiStatus LineEditToken(UmiEditorEditCommandRequest *request)
{
    if (!LineEditPrefix(request->kind))
        return UMI_STATUS_OK;
    if (request->kind == UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT)
    {
        if (request->line_comment == NULL && request->line_comment_byte_count != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (request->line_comment_byte_count == 0U)
        {
            request->line_comment = "//";
            request->line_comment_byte_count = 2U;
        }
        if (request->line_comment_byte_count > 32U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = 0U; i < request->line_comment_byte_count; ++i)
        {
            unsigned char value = (unsigned char)request->line_comment[i];
            if (value <= 0x20U || value >= 0x7fU)
                return UMI_STATUS_INVALID_ARGUMENT;
        }
    }
    else
    {
        if (request->indent_text == NULL && request->indent_byte_count != 0U)
            return UMI_STATUS_INVALID_ARGUMENT;
        if (request->indent_byte_count == 0U)
        {
            request->indent_text = "    ";
            request->indent_byte_count = 4U;
        }
        if (request->indent_byte_count > 16U)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        for (size_t i = 0U; i < request->indent_byte_count; ++i)
            if (request->indent_text[i] != ' ' && request->indent_text[i] != '\t')
                return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
UmiStatus UmiEditorLineEditPlanCreate(const char *source, size_t source_bytes,
                                      const UmiEditorEditCommandRequest *requested,
                                      const UmiCancellationToken *cancel, UmiEditorLineEditPlan **out)
{
    if (out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = NULL;
    if (source == NULL || requested == NULL || requested->struct_size != (uint32_t)sizeof(*requested) ||
        requested->api_version != UMI_EDITOR_EDIT_COMMAND_API_VERSION || !LineEditSupported(requested->kind))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (source_bytes > LINE_EDIT_TEXT_LIMIT)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_cancellation_token_is_requested(cancel))
        return UMI_STATUS_CANCELLED;
    if (memchr(source, '\0', source_bytes) != NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    for (size_t i = 0U; i < source_bytes; ++i)
        if (source[i] == '\r' && (i + 1U == source_bytes || source[i + 1U] != '\n'))
            return UMI_STATUS_NOT_IMPLEMENTED;
    UmiEditorTextBufferView view = {0};
    view.struct_size = (uint32_t)sizeof(view);
    view.api_version = UMI_EDITOR_TEXT_BUFFER_API_VERSION;
    view.bytes = source;
    view.byte_count = view.capacity = source_bytes;
    UmiEditorTextPosition position;
    UmiStatus status = UmiEditorTextViewPositionAt(&view, source_bytes, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, requested->cursor_offset, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, requested->selection_start, &position);
    if (status == UMI_STATUS_OK)
        status = UmiEditorTextViewPositionAt(&view, requested->selection_end, &position);
    UmiEditorEditCommandRequest request = *requested;
    if (status == UMI_STATUS_OK)
        status = LineEditToken(&request);
    if (!LineEditPrefix(request.kind))
        request.selection_start = request.selection_end = request.cursor_offset;
    /* Only the line index is needed by the shared transformation functions.
     * The temporary owner has no mutable buffer or transaction array: the
     * document coordinator remains the one authority for applying and Undo. */
    UmiEditorEditEngine projection = {0};
    if (status == UMI_STATUS_OK)
        status = umi_editor_line_index_create(&projection.line_index);
    if (status == UMI_STATUS_OK)
        status = umi_editor_line_index_rebuild(projection.line_index, source, source_bytes, 0U);
    size_t first = 0U, last = 0U;
    if (status == UMI_STATUS_OK)
        status = selected_lines(&projection, source_bytes, &request, &first, &last);
    size_t growth = 0U, start = 0U, end = 0U;
    if (status == UMI_STATUS_OK)
        status = umi_editor_line_index_line_range(projection.line_index, first, &start, &end);
    if (status == UMI_STATUS_OK)
    {
        if (request.kind == UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE)
            growth = block_end(&projection, first, source_bytes) - start +
                     (first + 1U == umi_editor_line_index_count(projection.line_index) ? 2U : 0U);
        else if (request.kind == UMI_EDITOR_EDIT_COMMAND_INDENT_LINES ||
                 request.kind == UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT)
        {
            size_t token = request.kind == UMI_EDITOR_EDIT_COMMAND_INDENT_LINES
                               ? request.indent_byte_count
                               : request.line_comment_byte_count + 1U;
            if (last - first + 1U > (LINE_EDIT_TEXT_LIMIT - source_bytes) / token)
                status = UMI_STATUS_CAPACITY_EXCEEDED;
            else
                growth = (last - first + 1U) * token;
        }
        if (status == UMI_STATUS_OK && growth > LINE_EDIT_TEXT_LIMIT - source_bytes)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
    }
    char *updated = NULL;
    size_t updated_bytes = 0U, cursor = 0U, lines = 0U, selection_bytes = 0U;
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
        status = generate_after(&projection, &view, &request, &updated, &updated_bytes, &cursor, &lines);
    int changed = status == UMI_STATUS_OK &&
                  (updated_bytes != source_bytes || memcmp(updated, source, source_bytes) != 0);
    if (status == UMI_STATUS_OK && changed && LineEditPrefix(request.kind))
    {
        status = umi_editor_line_index_rebuild(projection.line_index, updated, updated_bytes, 0U);
        if (status == UMI_STATUS_OK)
            status = umi_editor_line_index_line_range(projection.line_index, first, &cursor, &end);
        if (status == UMI_STATUS_OK && requested->selection_start != requested->selection_end)
            selection_bytes = block_end(&projection, last, updated_bytes) - cursor;
    }
    else if (status == UMI_STATUS_OK && changed &&
             request.kind == UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE)
        cursor = 0U;
    else if (status == UMI_STATUS_OK && !changed)
    {
        cursor = requested->selection_start < requested->selection_end ? requested->selection_start
                                                                       : requested->selection_end;
        selection_bytes = requested->selection_start < requested->selection_end
                              ? requested->selection_end - requested->selection_start
                              : requested->selection_start - requested->selection_end;
        if (selection_bytes == 0U)
            cursor = requested->cursor_offset;
    }
    umi_editor_line_index_destroy(projection.line_index);
    if (status == UMI_STATUS_OK)
    {
        view.bytes = updated;
        view.byte_count = view.capacity = updated_bytes;
        if (updated_bytes > LINE_EDIT_TEXT_LIMIT)
            status = UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, updated_bytes, &position);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, cursor, &position);
        if (status == UMI_STATUS_OK)
            status = UmiEditorTextViewPositionAt(&view, cursor + selection_bytes, &position);
    }
    if (status == UMI_STATUS_OK && umi_cancellation_token_is_requested(cancel))
        status = UMI_STATUS_CANCELLED;
    UmiEditorLineEditPlan *plan = NULL;
    if (status == UMI_STATUS_OK)
    {
        plan = calloc(1U, sizeof(*plan));
        if (plan == NULL)
            status = UMI_STATUS_OUT_OF_MEMORY;
    }
    if (status != UMI_STATUS_OK)
    {
        free(updated);
        return status;
    }
    plan->text = updated;
    plan->summary = (UmiEditorLineEditSummary){request.kind,    source_bytes, updated_bytes, cursor,
                                               selection_bytes, lines,        changed};
    *out = plan;
    return UMI_STATUS_OK;
}
void UmiEditorLineEditPlanDestroy(UmiEditorLineEditPlan *plan)
{
    if (plan != NULL)
    {
        free(plan->text);
        free(plan);
    }
}
UmiStatus UmiEditorLineEditPlanInspect(const UmiEditorLineEditPlan *plan, UmiEditorLineEditSummary *out)
{
    if (plan == NULL || out == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *out = plan->summary;
    return UMI_STATUS_OK;
}
UmiStatus UmiEditorLineEditPlanRead(const UmiEditorLineEditPlan *plan, const char **text, size_t *bytes)
{
    if (text != NULL)
        *text = NULL;
    if (bytes != NULL)
        *bytes = 0U;
    if (plan == NULL || text == NULL || bytes == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    *text = plan->text;
    *bytes = plan->summary.proposed_bytes;
    return UMI_STATUS_OK;
}
