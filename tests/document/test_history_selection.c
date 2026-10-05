/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_history_selection.c
 * PURPOSE: Verify source and selection travel together through bounded document Undo and Redo.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/line_edit.h"
static int Position(ReplacementFixture *f, size_t cursor, size_t selected)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, f->viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = cursor;
    view.selection_length = selected;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    return 0;
}
static int ExpectPosition(ReplacementFixture *f, size_t cursor, size_t selected)
{
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->viewId, &view) ==
          UMI_STATUS_OK);
    CHECK(view.cursor_offset == cursor && view.selection_length == selected);
    return 0;
}
static UmiStatus Replace(ReplacementFixture *f, const char *text)
{
    UmiDocumentEditPlan *plan = NULL;
    UmiStatus status = UmiDocumentCoordinatorPrepareEdit(f->documents, f->id, &plan);
    if (status == UMI_STATUS_OK)
        status = UmiDocumentCoordinatorApplyEdit(f->documents, plan, text, strlen(text));
    UmiDocumentEditPlanDestroy(plan);
    return status;
}
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1], *cases[] = {"insert",
                                            "selection",
                                            "delete",
                                            "unicode",
                                            "crlf",
                                            "first-line",
                                            "end",
                                            "empty",
                                            "duplicate",
                                            "indent",
                                            "pending-typing",
                                            "moved-before-undo",
                                            "moved-before-redo",
                                            "read-only",
                                            "invalid-text",
                                            "no-change",
                                            "targeted-other-tab",
                                            "eviction",
                                            "redo-cleared",
                                            "fallback-unicode",
                                            "fallback-crlf"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture f = {0};
    const char *source = "a\nbb\nccc", *replacement = "LONG", *expected = "a\nLONG\nccc";
    size_t cursor = 2U, selected = 2U;
    UmiStatus wanted = UMI_STATUS_OK;
    if (strcmp(name, "insert") == 0)
    {
        selected = 0U;
        replacement = "X";
        expected = "a\nXbb\nccc";
    }
    if (strcmp(name, "delete") == 0)
    {
        replacement = "";
        expected = "a\n\nccc";
    }
    if (strcmp(name, "unicode") == 0)
    {
        source = "a\n\xe9\x9b\xaa\nccc";
        selected = 3U;
    }
    if (strcmp(name, "crlf") == 0)
    {
        source = "a\r\nbb\r\nccc";
        cursor = 3U;
        expected = "a\r\nLONG\r\nccc";
    }
    if (strcmp(name, "first-line") == 0)
    {
        cursor = 0U;
        selected = 1U;
        expected = "LONG\nbb\nccc";
    }
    if (strcmp(name, "end") == 0)
    {
        cursor = 8U;
        selected = 0U;
        expected = "a\nbb\ncccLONG";
    }
    if (strcmp(name, "empty") == 0)
    {
        source = "";
        cursor = selected = 0U;
        expected = "LONG";
    }
    if (strcmp(name, "duplicate") == 0)
    {
        selected = 0U;
        expected = "a\nbb\nbb\nccc";
    }
    if (strcmp(name, "indent") == 0)
    {
        cursor = 0U;
        selected = 5U;
        expected = "    a\n    bb\nccc";
    }
    if (strcmp(name, "fallback-unicode") == 0)
        source = "\xe9\x9b\xaaxyz";
    if (strcmp(name, "fallback-crlf") == 0)
        source = "a\r\nb";
    CHECK(Start(&f, source) == 0);
    if (strncmp(name, "fallback-", 9U) == 0)
    {
        size_t newer_cursor = strcmp(name, "fallback-unicode") == 0 ? 1U : 2U;
        size_t old_cursor = strcmp(name, "fallback-unicode") == 0 ? 0U : 1U;
        CHECK(Draft(&f, "abc") == UMI_STATUS_OK && Position(&f, newer_cursor, 0U) == 0);
        CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK && ExpectText(&f, source) == 0 &&
              ExpectPosition(&f, old_cursor, 0U) == 0);
        CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK && ExpectText(&f, "abc") == 0 &&
              ExpectPosition(&f, newer_cursor, 0U) == 0);
        Stop(&f);
        return 0;
    }
    if (strcmp(name, "eviction") == 0)
    {
        for (size_t i = 0U; i < UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY + 8U; ++i)
        {
            CHECK(Position(&f, i, 0U) == 0);
            CHECK(Replace(&f, "x") == UMI_STATUS_OK);
        }
        UmiDocumentWorkingCopySnapshot state;
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &state) == UMI_STATUS_OK &&
              state.undo_count == UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY);
        for (size_t remaining = UMI_DOCUMENT_COORDINATOR_HISTORY_CAPACITY; remaining > 0U; --remaining)
        {
            CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
                  ExpectPosition(&f, remaining + 7U, 0U) == 0);
        }
        CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_NOT_FOUND);
        Stop(&f);
        return 0;
    }
    if (strcmp(name, "pending-typing") == 0)
    {
        source = "a\nbb!\nccc";
        selected = 3U;
        CHECK(Draft(&f, source) == UMI_STATUS_OK);
    }
    CHECK(Position(&f, cursor, selected) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    if (strcmp(name, "read-only") == 0)
    {
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        wanted = UMI_STATUS_PERMISSION_DENIED;
    }
    if (strcmp(name, "invalid-text") == 0)
    {
        replacement = "\xc0\x80";
        wanted = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "no-change") == 0)
    {
        replacement = "bb";
        expected = source;
    }
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);
    UmiStatus actual =
        strcmp(name, "duplicate") == 0
            ? UmiDocumentCoordinatorEditLines(f.documents, UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE, NULL)
        : strcmp(name, "indent") == 0
            ? UmiDocumentCoordinatorEditLines(f.documents, UMI_EDITOR_EDIT_COMMAND_INDENT_LINES, NULL)
            : Replace(&f, replacement);
    CHECK(actual == wanted);
    if (wanted != UMI_STATUS_OK || strcmp(name, "no-change") == 0)
    {
        CHECK(ExpectText(&f, source) == 0);
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK &&
              after.undo_count == before.undo_count);
        Stop(&f);
        return 0;
    }
    CHECK(ExpectText(&f, expected) == 0);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    size_t departure_cursor = view.cursor_offset, departure_selection = view.selection_length;
    if (strcmp(name, "moved-before-undo") == 0)
    {
        departure_cursor = 1U;
        departure_selection = 0U;
        CHECK(Position(&f, departure_cursor, departure_selection) == 0);
    }
    UmiDocumentId active = f.id;
    if (strcmp(name, "targeted-other-tab") == 0)
    {
        CHECK(umi_document_coordinator_new(f.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK);
        active = after.document_id;
    }
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK);
    CHECK(ExpectText(&f, source) == 0 && ExpectPosition(&f, cursor, selected) == 0);
    if (strcmp(name, "moved-before-redo") == 0)
        CHECK(Position(&f, 0U, 1U) == 0);
    if (strcmp(name, "redo-cleared") == 0)
    {
        CHECK(Replace(&f, "new") == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
              ExpectPosition(&f, cursor, selected) == 0);
        Stop(&f);
        return 0;
    }
    CHECK(UmiDocumentCoordinatorRedo(f.documents, f.id) == UMI_STATUS_OK && ExpectText(&f, expected) == 0 &&
          ExpectPosition(&f, departure_cursor, departure_selection) == 0);
    CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK && ExpectText(&f, source) == 0);
    CHECK(ExpectPosition(&f, strcmp(name, "moved-before-redo") == 0 ? 0U : cursor,
                         strcmp(name, "moved-before-redo") == 0 ? 1U : selected) == 0);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &after) == UMI_STATUS_OK &&
          after.document_id == active);
    if (strcmp(name, "pending-typing") == 0)
        CHECK(UmiDocumentCoordinatorUndo(f.documents, f.id) == UMI_STATUS_OK &&
              ExpectText(&f, "a\nbb\nccc") == 0);
    Stop(&f);
    return 0;
}
