/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_source_navigation.c
 * PURPOSE: Check exact draft selection, read-only navigation, identity lookup and unchanged history on failure.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/source_navigation.h"
#include "umicom/document/navigation_history.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"range",          "caret",     "unicode",   "split-start", "split-end",
                           "crlf",           "cr",        "read-only", "reversed",    "line-outside",
                           "column-outside", "inactive",  "draft",     "history",     "history-invalid",
                           "lookup",         "duplicate", "unknown",   "arguments",   "large"};
    int known_mode = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            known_mode = 1;
    CHECK(known_mode);
    char *large = NULL;
    const char *original = "source\n";
    UmiEditorTextPosition begin = {0U, 1U}, end = {0U, 3U};
    size_t wanted_offset = 1U, wanted_bytes = 2U;
    if (strcmp(mode, "caret") == 0)
    {
        end = begin;
        wanted_bytes = 0U;
    }
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "split-start") == 0 || strcmp(mode, "split-end") == 0 ||
        strcmp(mode, "crlf") == 0 || strcmp(mode, "cr") == 0)
    {
        original = strcmp(mode, "cr") == 0 ? "a\xf0\x9f\x8c\x8d\rxy" : "a\xf0\x9f\x8c\x8d\r\nxy";
        wanted_bytes = 4U;
    }
    if (strcmp(mode, "crlf") == 0 || strcmp(mode, "cr") == 0)
    {
        begin = (UmiEditorTextPosition){1U, 0U};
        end = (UmiEditorTextPosition){1U, 2U};
        wanted_offset = strcmp(mode, "cr") == 0 ? 6U : 7U;
        wanted_bytes = 2U;
    }
    if (strcmp(mode, "large") == 0)
    {
        large = malloc(9001U);
        CHECK(large != NULL);
        memset(large, 'x', 9000U);
        large[9000] = '\0';
        original = large;
        begin = (UmiEditorTextPosition){0U, 8500U};
        end = (UmiEditorTextPosition){0U, 8502U};
        wanted_offset = 8500U;
    }
    ReplacementFixture fixture = {0};
    CHECK(Start(&fixture, original) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
    if (strcmp(mode, "read-only") == 0)
    {
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "draft") == 0)
    {
        original = "changed draft";
        CHECK(Draft(&fixture, original) == UMI_STATUS_OK);
    }
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &before) == UMI_STATUS_OK);
    UmiDocumentId found = 0U;
    CHECK(UmiDocumentCoordinatorFindSourceUri(fixture.documents, view.uri, &found) == UMI_STATUS_OK &&
          found == fixture.id);
    char other[UMI_UI_ID_CAPACITY] = {0};
    if (strcmp(mode, "inactive") == 0 || strcmp(mode, "duplicate") == 0)
        CHECK(umi_document_coordinator_new(fixture.documents, "other.c", other, sizeof(other)) ==
              UMI_STATUS_OK);
    if (strcmp(mode, "duplicate") == 0)
    {
        UmiUiDocumentViewSnapshot second;
        CHECK(umi_ui_document_view_model_find(views, other, &second) == UMI_STATUS_OK);
        strcpy(second.uri, view.uri);
        CHECK(umi_ui_document_view_model_upsert(views, &second) == UMI_STATUS_OK);
        found = 999U;
        CHECK(UmiDocumentCoordinatorFindSourceUri(fixture.documents, view.uri, &found) ==
                  UMI_STATUS_ALREADY_EXISTS &&
              found == 999U);
        goto cleanup;
    }
    if (strcmp(mode, "lookup") == 0)
    {
        found = 999U;
        CHECK(UmiDocumentCoordinatorFindSourceUri(fixture.documents, "untitled:absent", &found) ==
                  UMI_STATUS_NOT_FOUND &&
              found == 999U);
    }
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "split-start") == 0)
    {
        begin.utf16_column = 2U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "split-end") == 0)
    {
        end.utf16_column = 2U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "reversed") == 0)
    {
        begin.utf16_column = 4U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "line-outside") == 0 || strcmp(mode, "history-invalid") == 0)
    {
        end.line = 5U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "column-outside") == 0)
    {
        end.utf16_column = 99U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiDocumentId target = fixture.id;
    if (strcmp(mode, "unknown") == 0)
    {
        target = UINT64_MAX;
        expected = UMI_STATUS_NOT_FOUND;
    }
    UmiDocumentNavigationHistorySnapshot history_before, history_after;
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(fixture.documents, &history_before) == UMI_STATUS_OK);
    size_t offset = 12345U, selected = 67890U;
    CHECK(UmiDocumentCoordinatorSelectSourceRange(fixture.documents, target, begin, end, &offset,
                                                  &selected) == expected);
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &after) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(fixture.documents, &history_after) == UMI_STATUS_OK);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(offset == wanted_offset && selected == wanted_bytes && after.document_id == fixture.id);
        CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
        CHECK(view.cursor_offset == offset && view.selection_length == selected);
        CHECK(after.undo_count == before.undo_count && after.redo_count == before.redo_count);
        if (strcmp(mode, "history") == 0)
        {
            CHECK(history_after.can_go_back);
            CHECK(UmiDocumentCoordinatorTravel(fixture.documents, -1, history_after.revision, NULL) ==
                  UMI_STATUS_OK);
            CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK &&
                  view.cursor_offset == 0U);
        }
    }
    else
    {
        CHECK(offset == 12345U && selected == 67890U);
        CHECK(history_after.revision == history_before.revision && after.document_id == before.document_id);
        CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == 0U && view.selection_length == 0U);
    }
    if (strcmp(mode, "arguments") == 0)
    {
        CHECK(UmiDocumentCoordinatorSelectSourceRange(NULL, fixture.id, begin, end, NULL, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorSelectSourceRange(fixture.documents, 0U, begin, end, NULL, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorFindSourceUri(fixture.documents, "", NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    CHECK(ExpectText(&fixture, original) == 0);
cleanup:
    Stop(&fixture);
    free(large);
    return 0;
}
