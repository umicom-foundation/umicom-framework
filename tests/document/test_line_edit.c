/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_line_edit.c
 * PURPOSE: Exercise document-owned line editing, selection and Undo against complete in-memory drafts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/line_edit.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1], *cases[] = {"delete",
                                            "duplicate",
                                            "move-up",
                                            "move-down",
                                            "join",
                                            "indent",
                                            "outdent",
                                            "comment",
                                            "trim",
                                            "selected-indent",
                                            "selected-comment",
                                            "caret-only",
                                            "crlf",
                                            "unicode",
                                            "empty",
                                            "pending-typing",
                                            "read-only",
                                            "invalid-kind",
                                            "invalid-token",
                                            "no-neighbor",
                                            "no-change",
                                            "bare-cr",
                                            "closed",
                                            "null-owner",
                                            "undo-redo",
                                            "selection-history",
                                            "other-tab"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture f = {0};
    const char *source = "a\nbb\nccc", *expected = "a\nbb\nbb\nccc";
    UmiEditorEditCommandKind kind = UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE;
    UmiDocumentLineEditOptions options = {0};
    UmiStatus wanted = UMI_STATUS_OK;
    size_t cursor = 2U, selected = 0U, after_cursor = 5U, after_selected = 0U;
    int changed = 1;
    if (strcmp(name, "delete") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_DELETE_LINE;
        expected = "a\nccc";
        after_cursor = 2U;
    }
    if (strcmp(name, "move-up") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP;
        expected = "bb\na\nccc";
        after_cursor = 0U;
    }
    if (strcmp(name, "move-down") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN;
        expected = "a\nccc\nbb";
        after_cursor = 6U;
    }
    if (strcmp(name, "join") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT;
        expected = "a\nbb ccc";
        after_cursor = 5U;
    }
    if (strcmp(name, "indent") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        expected = "a\n    bb\nccc";
        after_cursor = 2U;
    }
    if (strcmp(name, "outdent") == 0)
    {
        source = "a\n    bb\nccc";
        kind = UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES;
        expected = "a\nbb\nccc";
        after_cursor = 2U;
    }
    if (strcmp(name, "comment") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT;
        expected = "a\n// bb\nccc";
        after_cursor = 2U;
    }
    if (strcmp(name, "trim") == 0)
    {
        source = "a \nbb\t\nccc ";
        cursor = 3U;
        kind = UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE;
        expected = "a\nbb\nccc";
        after_cursor = 0U;
    }
    if (strcmp(name, "selected-indent") == 0 || strcmp(name, "selection-history") == 0)
    {
        cursor = 0U;
        selected = 5U;
        kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        expected = "    a\n    bb\nccc";
        after_cursor = 0U;
        after_selected = 13U;
    }
    if (strcmp(name, "selected-comment") == 0)
    {
        cursor = 0U;
        selected = 5U;
        kind = UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT;
        options.line_comment = "#";
        expected = "# a\n# bb\nccc";
        after_cursor = 0U;
        after_selected = 9U;
    }
    if (strcmp(name, "caret-only") == 0)
        selected = 6U;
    if (strcmp(name, "crlf") == 0)
    {
        source = "a\r\nbb";
        cursor = 4U;
        expected = "a\r\nbb\r\nbb";
        after_cursor = 8U;
    }
    if (strcmp(name, "unicode") == 0)
    {
        source = "a\n\xe9\x9b\xaa";
        expected = "a\n\xe9\x9b\xaa\n\xe9\x9b\xaa";
        after_cursor = 6U;
    }
    if (strcmp(name, "empty") == 0)
    {
        source = "";
        cursor = 0U;
        expected = "\n";
        after_cursor = 1U;
    }
    if (strcmp(name, "read-only") == 0)
        wanted = UMI_STATUS_PERMISSION_DENIED;
    if (strcmp(name, "invalid-kind") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "invalid-token") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_INDENT_LINES;
        options.indent_text = "bad";
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "no-neighbor") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP;
        cursor = 0U;
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(name, "no-change") == 0)
    {
        kind = UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES;
        expected = source;
        after_cursor = cursor;
        changed = 0;
    }
    if (strcmp(name, "bare-cr") == 0)
    {
        source = "a\rbb";
        wanted = UMI_STATUS_NOT_IMPLEMENTED;
    }
    CHECK(Start(&f, source) == 0);
    if (strcmp(name, "pending-typing") == 0)
    {
        source = "a\nbbb\nccc";
        expected = "a\nbbb\nbbb\nccc";
        after_cursor = 6U;
        CHECK(Draft(&f, source) == UMI_STATUS_OK);
    }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = cursor;
    view.selection_length = selected;
    view.read_only = strcmp(name, "read-only") == 0;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    if (strcmp(name, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorEditLines(f.documents, kind, NULL) == UMI_STATUS_NOT_FOUND);
        Stop(&f);
        return 0;
    }
    if (strcmp(name, "null-owner") == 0)
    {
        CHECK(UmiDocumentCoordinatorEditLines(NULL, kind, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        Stop(&f);
        return 0;
    }
    if (strcmp(name, "other-tab") == 0)
    {
        char other[UMI_UI_ID_CAPACITY];
        CHECK(umi_document_coordinator_new(f.documents, "other.c", other, sizeof(other)) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorEditLines(f.documents, kind, NULL) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, source) == 0);
        char *text = NULL;
        size_t bytes = 0U;
        CHECK(UmiUiDocumentViewModelCopyText(views, other, &text, &bytes) == UMI_STATUS_OK && bytes == 1U &&
              strcmp(text, "\n") == 0);
        UmiUiDocumentViewModelFreeText(text);
        Stop(&f);
        return 0;
    }
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorEditLines(f.documents, kind, &options) == wanted);
    CHECK(ExpectText(&f, wanted == UMI_STATUS_OK ? expected : source) == 0);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK);
    if (wanted != UMI_STATUS_OK || !changed)
    {
        CHECK(before.revision == after.revision && before.undo_count == after.undo_count &&
              before.redo_count == after.redo_count && before.dirty == after.dirty);
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == cursor && view.selection_length == selected);
    }
    else
    {
        CHECK(after.undo_count == before.undo_count + (strcmp(name, "pending-typing") == 0 ? 2U : 1U));
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == after_cursor && view.selection_length == after_selected);
        CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK && ExpectText(&f, source) == 0);
        /* Shared history now restores the captured command selection. The
         * earlier text-only expectation remains for engineering review. */
#if 0
        /* Existing document history restores text, then clamps the current
         * selection. A line command does not create a second cursor history. */
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset <= strlen(source) &&
              view.selection_length <= strlen(source) - view.cursor_offset);
#endif
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == cursor && view.selection_length == selected);
        CHECK(umi_document_coordinator_redo(f.documents) == UMI_STATUS_OK && ExpectText(&f, expected) == 0);
        if (strcmp(name, "pending-typing") == 0)
        {
            CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
            CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
            CHECK(ExpectText(&f, "a\nbb\nccc") == 0);
        }
    }
    Stop(&f);
    return 0;
}
