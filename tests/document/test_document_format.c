/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_document_format.c
 * PURPOSE: Verify format transitions retain source, save policy and selection across document history.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/format.h"
#include "umicom/document/line_edit.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1], *cases[] = {"lf",
                                            "crlf",
                                            "bare-cr",
                                            "unicode",
                                            "selection",
                                            "final",
                                            "empty",
                                            "encoding-only",
                                            "utf8-bom",
                                            "utf16-le",
                                            "utf16-be",
                                            "same-format",
                                            "pending-typing",
                                            "undo-redo",
                                            "ordinary-after-format",
                                            "redo-cleared",
                                            "read-only",
                                            "identity",
                                            "invalid-encoding",
                                            "invalid-ending",
                                            "invalid-final",
                                            "closed",
                                            "null-options"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture f = {0};
    const char *source = "a\nb\n", *expected = "a\r\nb\r\n";
    UmiDocumentFormatOptions options = {UMI_DOCUMENT_ENCODING_UNKNOWN, UMI_DOCUMENT_LINE_ENDING_CRLF, 0};
    UmiStatus wanted = UMI_STATUS_OK;
    size_t cursor = 2U, selected = 1U, new_cursor = 3U;
    if (strcmp(mode, "lf") == 0 || strcmp(mode, "bare-cr") == 0)
    {
        source = strcmp(mode, "bare-cr") == 0 ? "a\rb\r" : "a\r\nb\r\n";
        expected = "a\nb\n";
        cursor = strcmp(mode, "bare-cr") == 0 ? 2U : 3U;
        options.line_ending = UMI_DOCUMENT_LINE_ENDING_LF;
        new_cursor = 2U;
    }
    if (strcmp(mode, "unicode") == 0)
    {
        source = "\xe9\x9b\xaa\nb\n";
        expected = "\xe9\x9b\xaa\r\nb\r\n";
        cursor = 4U;
        new_cursor = 5U;
    }
    if (strcmp(mode, "selection") == 0)
    {
        cursor = 0U;
        selected = 4U;
        new_cursor = 0U;
    }
    if (strcmp(mode, "final") == 0)
    {
        source = "a\nb";
        expected = "a\r\nb\r\n";
        options.ensure_final_newline = 1;
    }
    if (strcmp(mode, "empty") == 0)
    {
        source = expected = "";
        cursor = selected = new_cursor = 0U;
    }
    if (strcmp(mode, "encoding-only") == 0 || strcmp(mode, "utf8-bom") == 0 ||
        strcmp(mode, "utf16-le") == 0 || strcmp(mode, "utf16-be") == 0)
    {
        options.line_ending = UMI_DOCUMENT_LINE_ENDING_NONE;
        options.encoding = strcmp(mode, "utf16-le") == 0   ? UMI_DOCUMENT_ENCODING_UTF16_LE
                           : strcmp(mode, "utf16-be") == 0 ? UMI_DOCUMENT_ENCODING_UTF16_BE
                                                           : UMI_DOCUMENT_ENCODING_UTF8_BOM;
        expected = source;
        new_cursor = cursor;
    }
    if (strcmp(mode, "same-format") == 0)
    {
        options.line_ending = UMI_DOCUMENT_LINE_ENDING_LF;
        expected = source;
        new_cursor = cursor;
    }
    CHECK(Start(&f, source) == 0);
    if (strcmp(mode, "pending-typing") == 0)
    {
        CHECK(Draft(&f, "later\ntext") == UMI_STATUS_OK);
        source = "later\ntext";
        expected = "later\r\ntext";
        cursor = 6U;
        new_cursor = 7U;
    }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = cursor;
    view.selection_length = selected;
    if (strcmp(mode, "read-only") == 0)
    {
        view.read_only = 1;
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(mode, "identity") == 0)
    {
        strcpy(view.document_id, "foreign");
        wanted = UMI_STATUS_INVALID_STATE;
    }
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
    if (strcmp(mode, "invalid-encoding") == 0)
    {
        options.encoding = UMI_DOCUMENT_ENCODING_BINARY;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-ending") == 0)
    {
        options.line_ending = UMI_DOCUMENT_LINE_ENDING_MIXED;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "invalid-final") == 0)
    {
        options.ensure_final_newline = 2;
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
        wanted = UMI_STATUS_NOT_FOUND;
    }
    if (strcmp(mode, "null-options") == 0)
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    CHECK(UmiDocumentCoordinatorSetFormat(f.documents, strcmp(mode, "null-options") == 0 ? NULL : &options) ==
          wanted);
    if (strcmp(mode, "closed") == 0)
        goto done;
    CHECK(ExpectText(&f, wanted == UMI_STATUS_OK ? expected : source) == 0);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK);
    int changed = wanted == UMI_STATUS_OK && strcmp(mode, "same-format") != 0;
    CHECK(after.undo_count ==
          before.undo_count + (changed ? 1U : 0U) + (strcmp(mode, "pending-typing") == 0 ? 1U : 0U));
    if (!changed)
    {
        CHECK(after.encoding == before.encoding && after.line_ending == before.line_ending);
        goto done;
    }
    CHECK(after.dirty && after.revision != before.revision);
    CHECK(after.encoding ==
          (options.encoding == UMI_DOCUMENT_ENCODING_UNKNOWN ? before.encoding : options.encoding));
    CHECK(after.line_ending ==
          (options.line_ending == UMI_DOCUMENT_LINE_ENDING_NONE ? before.line_ending : options.line_ending));
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == new_cursor);
    CHECK(view.selection_length == (strcmp(mode, "selection") == 0 ? 6U : selected));
    if (strcmp(mode, "ordinary-after-format") == 0)
    {
        CHECK(UmiDocumentCoordinatorEditLines(f.documents, UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE, NULL) ==
              UMI_STATUS_OK);
        CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
        CHECK(ExpectText(&f, expected) == 0);
    }
    CHECK(umi_document_coordinator_undo(f.documents) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, source) == 0);
    UmiDocumentWorkingCopySnapshot restored;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &restored) == UMI_STATUS_OK &&
          restored.encoding == before.encoding && restored.line_ending == before.line_ending);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == cursor && view.selection_length == selected);
    if (strcmp(mode, "redo-cleared") == 0)
    {
        CHECK(UmiDocumentCoordinatorEditLines(f.documents, UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE, NULL) ==
              UMI_STATUS_OK);
        CHECK(umi_document_coordinator_redo(f.documents) == UMI_STATUS_NOT_FOUND);
        goto done;
    }
    CHECK(umi_document_coordinator_redo(f.documents) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, expected) == 0);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &restored) == UMI_STATUS_OK &&
          restored.encoding == after.encoding && restored.line_ending == after.line_ending);
done:
    Stop(&f);
    return 0;
}
