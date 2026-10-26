/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_line_edit_admission.c
 *
 * PURPOSE:
 *   Check read-only line-command admission against real document state without
 *   changing the active tab, draft, selection or Undo history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "replacement_fixture.h"
#include "umicom/document/line_edit.h"

static const UmiEditorEditCommandKind caretKinds[] = {
    UMI_EDITOR_EDIT_COMMAND_DELETE_LINE, UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE,
    UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP, UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_DOWN,
    UMI_EDITOR_EDIT_COMMAND_JOIN_LINE_WITH_NEXT,
    UMI_EDITOR_EDIT_COMMAND_TRIM_TRAILING_WHITESPACE,
    UMI_EDITOR_EDIT_COMMAND_INDENT_LINES, UMI_EDITOR_EDIT_COMMAND_OUTDENT_LINES,
    UMI_EDITOR_EDIT_COMMAND_TOGGLE_LINE_COMMENT
};
static const UmiEditorEditCommandKind selectionKinds[] = {
    UMI_EDITOR_EDIT_COMMAND_TRANSFORM_UPPERCASE, UMI_EDITOR_EDIT_COMMAND_TRANSFORM_LOWERCASE,
    UMI_EDITOR_EDIT_COMMAND_SORT_LINES_ASCENDING, UMI_EDITOR_EDIT_COMMAND_SORT_LINES_DESCENDING
};

static int ExpectKinds(ReplacementFixture *f, const UmiEditorEditCommandKind *kinds,
    size_t count, UmiStatus expected)
{
    for (size_t i = 0U; i < count; ++i)
        CHECK(UmiDocumentCoordinatorCheckLineEdit(f->documents, f->id, kinds[i]) == expected);
    return 0;
}

int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const char *cases[] = {"caret-kinds", "empty-selection", "selected-kinds",
        "read-only", "selection-transition", "readonly-transition", "zero-id",
        "null-owner", "unknown-id", "unsupported-kind", "closed", "other-tab",
        "history", "neighbor-deferred"};
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(name, cases[i]) == 0) known = 1;
    CHECK(known);
    ReplacementFixture f = {0};
    CHECK(Start(&f, "a\nbb\nccc") == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view, after;
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    view.cursor_offset = 2U;
    view.selection_length = 0U;
    if (!strcmp(name, "selected-kinds") || !strcmp(name, "read-only") ||
        !strcmp(name, "readonly-transition")) view.selection_length = 2U;
    if (!strcmp(name, "read-only") || !strcmp(name, "readonly-transition")) view.read_only = 1;
    if (!strcmp(name, "neighbor-deferred")) view.cursor_offset = 0U;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    if (!strcmp(name, "history")) {
        CHECK(UmiDocumentCoordinatorEditLinesForDocument(f.documents, f.id,
            UMI_EDITOR_EDIT_COMMAND_DUPLICATE_LINE, NULL) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
    }
    if (!strcmp(name, "other-tab")) {
        char second[UMI_UI_ID_CAPACITY];
        CHECK(umi_document_coordinator_new(f.documents, "second.c", second, sizeof second) == UMI_STATUS_OK);
    }
    UmiDocumentWorkingCopySnapshot before, latest;
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &before) == UMI_STATUS_OK);

    if (!strcmp(name, "caret-kinds") || !strcmp(name, "history") || !strcmp(name, "other-tab"))
        CHECK(ExpectKinds(&f, caretKinds, sizeof caretKinds / sizeof caretKinds[0], UMI_STATUS_OK) == 0);
    if (!strcmp(name, "empty-selection") || !strcmp(name, "selection-transition")) {
        CHECK(ExpectKinds(&f, selectionKinds, sizeof selectionKinds / sizeof selectionKinds[0], UMI_STATUS_NOT_FOUND) == 0);
        if (!strcmp(name, "selection-transition")) {
            view.selection_length = 2U;
            CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
            CHECK(ExpectKinds(&f, selectionKinds, sizeof selectionKinds / sizeof selectionKinds[0], UMI_STATUS_OK) == 0);
        }
    }
    if (!strcmp(name, "selected-kinds"))
        CHECK(ExpectKinds(&f, selectionKinds, sizeof selectionKinds / sizeof selectionKinds[0], UMI_STATUS_OK) == 0);
    if (!strcmp(name, "read-only") || !strcmp(name, "readonly-transition")) {
        CHECK(ExpectKinds(&f, caretKinds, sizeof caretKinds / sizeof caretKinds[0], UMI_STATUS_PERMISSION_DENIED) == 0);
        CHECK(ExpectKinds(&f, selectionKinds, sizeof selectionKinds / sizeof selectionKinds[0], UMI_STATUS_PERMISSION_DENIED) == 0);
        if (!strcmp(name, "readonly-transition")) {
            view.read_only = 0;
            CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
            CHECK(ExpectKinds(&f, selectionKinds, sizeof selectionKinds / sizeof selectionKinds[0], UMI_STATUS_OK) == 0);
        }
    }
    if (!strcmp(name, "zero-id"))
        CHECK(UmiDocumentCoordinatorCheckLineEdit(f.documents, 0U, caretKinds[0]) == UMI_STATUS_INVALID_ARGUMENT);
    if (!strcmp(name, "null-owner"))
        CHECK(UmiDocumentCoordinatorCheckLineEdit(NULL, f.id, caretKinds[0]) == UMI_STATUS_INVALID_ARGUMENT);
    if (!strcmp(name, "unknown-id"))
        CHECK(UmiDocumentCoordinatorCheckLineEdit(f.documents, UINT64_MAX, caretKinds[0]) == UMI_STATUS_NOT_FOUND);
    if (!strcmp(name, "unsupported-kind"))
        CHECK(UmiDocumentCoordinatorCheckLineEdit(f.documents, f.id,
            UMI_EDITOR_EDIT_COMMAND_INSERT_TEXT) == UMI_STATUS_INVALID_ARGUMENT);
    if (!strcmp(name, "neighbor-deferred")) {
        /* Admission does not pretend to have prepared a complete edit. The
         * executor still refuses moving the first line above its boundary. */
        CHECK(UmiDocumentCoordinatorCheckLineEdit(f.documents, f.id,
            UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorEditLinesForDocument(f.documents, f.id,
            UMI_EDITOR_EDIT_COMMAND_MOVE_LINE_UP, NULL) == UMI_STATUS_NOT_FOUND);
    }
    if (!strcmp(name, "closed")) {
        CHECK(umi_document_coordinator_close_active(f.documents, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorCheckLineEdit(f.documents, f.id, caretKinds[0]) == UMI_STATUS_NOT_FOUND);
        Stop(&f);
        return 0;
    }

    CHECK(umi_ui_document_view_model_find(views, f.viewId, &after) == UMI_STATUS_OK);
    CHECK(after.cursor_offset == view.cursor_offset && after.selection_length == view.selection_length &&
          after.read_only == view.read_only);
    CHECK(umi_document_coordinator_active_snapshot(f.documents, &latest) == UMI_STATUS_OK);
    CHECK(latest.document_id == before.document_id && latest.revision == before.revision &&
          latest.undo_count == before.undo_count && latest.redo_count == before.redo_count);
    CHECK(ExpectText(&f, !strcmp(name, "history") ? "a\nbb\nbb\nccc" : "a\nbb\nccc") == 0);
    Stop(&f);
    return 0;
}
