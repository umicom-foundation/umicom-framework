/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_delimiter_navigation.c
 * PURPOSE: Verify delimiter navigation uses complete drafts and existing location history without changing source.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/delimiter_navigation.h"
#include "umicom/document/navigation_history.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    const char *cases[] = {"match",          "reverse",        "contents",      "pair",      "empty-pair",
                           "read-only",      "pending-typing", "unicode",       "crlf",      "selection",
                           "other-tab",      "history",        "no-pair",       "crossed",   "closed",
                           "invalid-action", "invalid-syntax", "zero-document", "null-owner"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(name, cases[i]) == 0)
            known = 1;
    CHECK(known);
    ReplacementFixture fixture = {0};
    const char *text = "zero {one [two] end} tail";
    if (strcmp(name, "empty-pair") == 0)
        text = "()";
    if (strcmp(name, "unicode") == 0)
        text = "(\xe9\x9b\xaa)";
    if (strcmp(name, "crlf") == 0)
        text = "{\r\n(x)\r\n}";
    if (strcmp(name, "no-pair") == 0)
        text = "plain words";
    if (strcmp(name, "crossed") == 0)
        text = "([)]";
    CHECK(Start(&fixture, text) == 0);
    if (strcmp(name, "pending-typing") == 0)
    {
        text = "(new)";
        CHECK(Draft(&fixture, text) == UMI_STATUS_OK);
    }
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
    size_t caret = 10U, expected_offset = 14U, expected_selection = 0U;
    UmiDocumentDelimiterAction action = UMI_DOCUMENT_DELIMITER_MATCH;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(name, "reverse") == 0)
    {
        caret = 14U;
        expected_offset = 10U;
    }
    if (strcmp(name, "contents") == 0)
    {
        caret = 12U;
        action = UMI_DOCUMENT_DELIMITER_CONTENTS;
        expected_offset = 11U;
        expected_selection = 3U;
    }
    if (strcmp(name, "pair") == 0)
    {
        caret = 12U;
        action = UMI_DOCUMENT_DELIMITER_PAIR;
        expected_offset = 10U;
        expected_selection = 5U;
    }
    if (strcmp(name, "empty-pair") == 0)
    {
        caret = 1U;
        action = UMI_DOCUMENT_DELIMITER_CONTENTS;
        expected_offset = 1U;
    }
    if (strcmp(name, "pending-typing") == 0 || strcmp(name, "unicode") == 0)
    {
        caret = 0U;
        expected_offset = 4U;
    }
    if (strcmp(name, "crlf") == 0)
    {
        caret = 3U;
        expected_offset = 5U;
    }
    if (strcmp(name, "no-pair") == 0 || strcmp(name, "crossed") == 0)
    {
        caret = 0U;
        expected = UMI_STATUS_NOT_FOUND;
    }
    view.cursor_offset = caret;
    view.selection_length = strcmp(name, "selection") == 0 ? 5U : 0U;
    view.read_only = strcmp(name, "read-only") == 0;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &before) == UMI_STATUS_OK);
    if (strcmp(name, "closed") == 0)
    {
        CHECK(umi_document_coordinator_close_active(fixture.documents, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigateDelimiter(fixture.documents, fixture.id, action,
                                                      UMI_EDITOR_DELIMITER_LITERAL) == UMI_STATUS_NOT_FOUND);
        Stop(&fixture);
        return 0;
    }
    if (strcmp(name, "other-tab") == 0)
        CHECK(umi_document_coordinator_new(fixture.documents, "other.c", NULL, 0U) == UMI_STATUS_OK);
    UmiDocumentCoordinator *owner = fixture.documents;
    UmiDocumentId document = fixture.id;
    UmiEditorDelimiterSyntax syntax = UMI_EDITOR_DELIMITER_LITERAL;
    if (strcmp(name, "invalid-action") == 0)
    {
        action = (UmiDocumentDelimiterAction)90;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "invalid-syntax") == 0)
    {
        syntax = (UmiEditorDelimiterSyntax)90;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "zero-document") == 0)
    {
        document = 0U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(name, "null-owner") == 0)
    {
        owner = NULL;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    CHECK(UmiDocumentCoordinatorNavigateDelimiter(owner, document, action, syntax) == expected);
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
    CHECK(view.cursor_offset == (expected == UMI_STATUS_OK ? expected_offset : caret));
    CHECK(view.selection_length == (expected == UMI_STATUS_OK ? expected_selection : 0U));
    CHECK(ExpectText(&fixture, text) == 0);
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &after) == UMI_STATUS_OK);
    CHECK(after.document_id == fixture.id && after.revision == before.revision &&
          after.undo_count == before.undo_count && after.redo_count == before.redo_count &&
          after.dirty == before.dirty);
    if (strcmp(name, "history") == 0)
    {
        UmiDocumentNavigationHistorySnapshot history;
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(fixture.documents, &history) == UMI_STATUS_OK &&
              history.can_go_back);
        CHECK(UmiDocumentCoordinatorTravel(fixture.documents, -1, history.revision, NULL) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == caret);
    }
    Stop(&fixture);
    return 0;
}
