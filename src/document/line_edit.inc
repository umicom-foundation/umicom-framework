/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/document/line_edit.inc
 *
 * PURPOSE:
 *   Capture live line-edit intent, prepare complete source and publish through existing
 *   document history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/line_edit.h"
/* Explicit document identity now shares the same line-edit transaction. The original active-only implementation is retained for review; its public entry point remains below. */
#if 0
UmiStatus UmiDocumentCoordinatorEditLines(UmiDocumentCoordinator *coordinator, UmiEditorEditCommandKind kind,
                                          const UmiDocumentLineEditOptions *options)
{
    if (coordinator == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot before;
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &before);
    if (status != UMI_STATUS_OK)
        return status;
    if (before.read_only)
        return UMI_STATUS_PERMISSION_DENIED;
    char *source = NULL;
    size_t bytes = 0U;
    status = UmiUiDocumentViewModelCopyText(views, before.view_id, &source, &bytes);
    UmiEditorEditCommandRequest request;
    if (status == UMI_STATUS_OK)
        status = umi_editor_edit_command_request_initialize(&request, kind, before.cursor_offset);
    if (status == UMI_STATUS_OK &&
        (before.cursor_offset > bytes || before.selection_length > bytes - before.cursor_offset))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        request.selection_start = before.cursor_offset;
        request.selection_end = before.cursor_offset + before.selection_length;
        if (options != NULL)
        {
            request.indent_text = options->indent_text;
            request.indent_byte_count = options->indent_text == NULL ? 0U : strlen(options->indent_text);
            request.line_comment = options->line_comment;
            request.line_comment_byte_count =
                options->line_comment == NULL ? 0U : strlen(options->line_comment);
        }
    }
    UmiEditorLineEditPlan *plan = NULL;
    UmiEditorLineEditSummary summary;
    const char *updated = NULL;
    size_t updated_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiEditorLineEditPlanCreate(source, bytes, &request, NULL, &plan);
    if (status == UMI_STATUS_OK)
        status = UmiEditorLineEditPlanInspect(plan, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiEditorLineEditPlanRead(plan, &updated, &updated_bytes);
    if (status == UMI_STATUS_OK && summary.changed)
    {
        UmiUiDocumentViewSnapshot after = before;
        after.cursor_offset = summary.cursor_offset;
        after.selection_length = summary.selection_bytes;
        /* The plan owns no history. This one commit reserves the native draft
         * and keeps earlier unsynchronized typing separate from this command. */
        status = CommitViewText(coordinator, index, &before, &after, source, bytes, updated, updated_bytes);
    }
    UmiEditorLineEditPlanDestroy(plan);
    UmiUiDocumentViewModelFreeText(source);
    return status;
}

#endif


/* Frontends share admission rules without allocating edit plans or changing
 * history. This is a current-state hint; execution still validates the full
 * text transformation and may refuse a changed owner or missing neighbor. */
UmiStatus UmiDocumentCoordinatorCheckLineEdit(UmiDocumentCoordinator *coordinator,
    UmiDocumentId documentId, UmiEditorEditCommandKind kind)
{
    if (coordinator == NULL || documentId == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    int needsSelection = 0;
    switch (kind) {
    case UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE:
    case UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE:
    case UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING:
    case UMI_EDITOR_EDIT_COMMAND_SORT_LINES_DESCENDING:
        needsSelection = 1;
        break;
    case UMI_EDITOR_EDIT_COMMAND_DELETE_LINE:
    case UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE:
    case UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP:
    case UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN:
    case UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT:
    case UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE:
    case UMI_EDITOR_EDIT_COMMAND_INDENT_LINES:
    case UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES:
    case UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT:
        break;
    default:
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U) return UMI_STATUS_BUSY;
    size_t index = EditDocumentIndex(coordinator, documentId);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiUiDocumentTextInfo text;
    UmiStatus status = umi_ui_document_view_model_find(views,
        coordinator->entries[index].view_id, &view);
    if (status != UMI_STATUS_OK) return status;
    char identity[UMI_UI_ID_CAPACITY];
    int length = snprintf(identity, sizeof identity, "document.%llu",
        (unsigned long long)documentId);
    if (length < 0 || (size_t)length >= sizeof identity ||
        strcmp(view.document_id, identity) != 0) return UMI_STATUS_INVALID_STATE;
    status = UmiUiDocumentViewModelTextInfo(views, view.view_id, &text);
    if (status != UMI_STATUS_OK) return status;
    if (view.cursor_offset > text.byte_count ||
        view.selection_length > text.byte_count - view.cursor_offset)
        return UMI_STATUS_INVALID_STATE;
    if (view.read_only) return UMI_STATUS_PERMISSION_DENIED;
    if (needsSelection && view.selection_length == 0U) return UMI_STATUS_NOT_FOUND;
    return UMI_STATUS_OK;
}

UmiStatus UmiDocumentCoordinatorEditLinesForDocument(UmiDocumentCoordinator *coordinator,
                                          UmiDocumentId documentId, UmiEditorEditCommandKind kind,
                                          const UmiDocumentLineEditOptions *options)
{
    if (coordinator == NULL || documentId == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress ||
        coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t index = EditDocumentIndex(coordinator, documentId);
    if (index == SIZE_MAX)
        return UMI_STATUS_NOT_FOUND;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(coordinator->workbench);
    UmiUiDocumentViewSnapshot before;
    UmiStatus status = umi_ui_document_view_model_find(views, coordinator->entries[index].view_id, &before);
    if (status != UMI_STATUS_OK)
        return status;
    /* A view ID may survive a projection replacement. Verify the canonical
     * document identity before planning even an unchanged edit; otherwise text
     * from an unrelated projection could be committed to this working copy. */
    char identity[UMI_UI_ID_CAPACITY];
    (void)snprintf(identity, sizeof identity, "document.%llu", (unsigned long long)documentId);
    if (strcmp(before.document_id, identity) != 0)
        return UMI_STATUS_INVALID_STATE;
    if (before.read_only)
        return UMI_STATUS_PERMISSION_DENIED;
    char *source = NULL;
    size_t bytes = 0U;
    status = UmiUiDocumentViewModelCopyText(views, before.view_id, &source, &bytes);
    UmiEditorEditCommandRequest request;
    /* An explicit options object retains the original literal-token behavior.
     * Otherwise use the coordinator policy. Outdent removes a tab or up to the
     * configured width of spaces even when new indentation uses tabs. */
    char indentation[UMI_EDITOR_INDENTATION_MAXIMUM_WIDTH + 1U];
    UmiDocumentLineEditOptions defaults = {NULL, NULL};
    if (status == UMI_STATUS_OK && options == NULL) {
        /* Preserve the global-only lookup; named working copies now own overrides. */
#if 0
        UmiEditorIndentation policy = coordinator->indentation;
#endif
        UmiDocumentIndentationReview review;
        status = UmiDocumentCoordinatorInspectIndentation(coordinator, documentId, &review);
        UmiEditorIndentation policy = status == UMI_STATUS_OK ? review.effective : coordinator->indentation;
        if (kind == UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES) policy.insert_spaces = 1;
        size_t indentationBytes = 0U;
        /* Do not overwrite a failed identity/policy read with successful token formatting. */
        if (status == UMI_STATUS_OK)
            status = UmiEditorIndentationUnit(&policy, indentation, sizeof indentation, &indentationBytes);
        defaults.indent_text = indentation;
        options = &defaults;
    }
    if (status == UMI_STATUS_OK)
        status = umi_editor_edit_command_request_initialize(&request, kind, before.cursor_offset);
    if (status == UMI_STATUS_OK &&
        (before.cursor_offset > bytes || before.selection_length > bytes - before.cursor_offset))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status == UMI_STATUS_OK)
    {
        request.selection_start = before.cursor_offset;
        request.selection_end = before.cursor_offset + before.selection_length;
        if (options != NULL)
        {
            request.indent_text = options->indent_text;
            request.indent_byte_count = options->indent_text == NULL ? 0U : strlen(options->indent_text);
            request.line_comment = options->line_comment;
            request.line_comment_byte_count =
                options->line_comment == NULL ? 0U : strlen(options->line_comment);
        }
    }
    UmiEditorLineEditPlan *plan = NULL;
    UmiEditorLineEditSummary summary;
    const char *updated = NULL;
    size_t updated_bytes = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiEditorLineEditPlanCreate(source, bytes, &request, NULL, &plan);
    if (status == UMI_STATUS_OK)
        status = UmiEditorLineEditPlanInspect(plan, &summary);
    if (status == UMI_STATUS_OK)
        status = UmiEditorLineEditPlanRead(plan, &updated, &updated_bytes);
    if (status == UMI_STATUS_OK && summary.changed)
    {
        UmiUiDocumentViewSnapshot after = before;
        after.cursor_offset = summary.cursor_offset;
        after.selection_length = summary.selection_bytes;
        /* The plan owns no history. This one commit reserves the native draft
         * and keeps earlier unsynchronized typing separate from this command. */
        status = CommitViewText(coordinator, index, &before, &after, source, bytes, updated, updated_bytes);
    }
    UmiEditorLineEditPlanDestroy(plan);
    UmiUiDocumentViewModelFreeText(source);
    return status;
}


/* Toolbar and headless callers keep the established active-document API.
 * Native source views can instead retain their own document identity. */
UmiStatus UmiDocumentCoordinatorEditLines(UmiDocumentCoordinator *coordinator,
    UmiEditorEditCommandKind kind, const UmiDocumentLineEditOptions *options)
{
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (coordinator->save_in_progress || coordinator->reopen_in_progress || coordinator->navigation_depth != 0U)
        return UMI_STATUS_BUSY;
    size_t index = active_index(coordinator);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    return UmiDocumentCoordinatorEditLinesForDocument(coordinator,
        coordinator->entries[index].document_id, kind, options);
}
