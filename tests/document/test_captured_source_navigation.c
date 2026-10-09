/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_captured_source_navigation.c
 * PURPOSE: Guard source navigation against changed drafts without saving or redirecting a document.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/navigation_history.h"
#include "umicom/document/source_navigation.h"
/* These cases exercise the document owner's real draft and navigation stores.
 * A diagnostic source proof must not save text or create an Undo entry. */
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"same",    "inactive", "caret",    "read-only", "unicode",
                           "changed", "shorter",  "renamed",  "split",     "reversed",
                           "outside", "unknown",  "arguments"};
    bool recognized = false;
    for (size_t index = 0U; index < sizeof known / sizeof known[0]; ++index)
        if (strcmp(mode, known[index]) == 0)
            recognized = true;
    CHECK(recognized);
    const char *source = "a\xf0\x9f\x8c\x8d\r\nxy";
    ReplacementFixture fixture = {0};
    CHECK(Start(&fixture, source) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(fixture.workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
    char captured_uri[UMI_DOCUMENT_URI_CAPACITY];
    strcpy(captured_uri, view.uri);
    UmiEditorTextPosition first = {1U, 0U}, last = {1U, 2U};
    size_t expected_offset = 7U, expected_bytes = 2U;
    UmiStatus expected = UMI_STATUS_OK;
    if (strcmp(mode, "unicode") == 0)
    {
        first = (UmiEditorTextPosition){0U, 1U};
        last = (UmiEditorTextPosition){0U, 3U};
        expected_offset = 1U;
        expected_bytes = 4U;
    }
    if (strcmp(mode, "caret") == 0)
    {
        view.cursor_offset = 1U;
        view.selection_length = 4U;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "read-only") == 0)
    {
        view.read_only = 1;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "changed") == 0)
    {
        CHECK(Draft(&fixture, "b\xf0\x9f\x8c\x8d\r\nxy") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "shorter") == 0)
    {
        CHECK(Draft(&fixture, "short") == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "renamed") == 0)
    {
        strcpy(view.uri, "untitled:renamed.c");
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        expected = UMI_STATUS_INVALID_STATE;
    }
    if (strcmp(mode, "split") == 0)
    {
        first = (UmiEditorTextPosition){0U, 2U};
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "reversed") == 0)
    {
        first = (UmiEditorTextPosition){1U, 2U};
        last = (UmiEditorTextPosition){1U, 0U};
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "outside") == 0)
    {
        last.line = 5U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    }
    if (strcmp(mode, "inactive") == 0)
    {
        char other[UMI_UI_ID_CAPACITY];
        CHECK(umi_document_coordinator_new(fixture.documents, "other.c", other, sizeof other) ==
              UMI_STATUS_OK);
    }
    UmiDocumentId target = strcmp(mode, "unknown") == 0 ? UINT64_MAX : fixture.id;
    if (strcmp(mode, "unknown") == 0)
        expected = UMI_STATUS_NOT_FOUND;
    UmiDocumentWorkingCopySnapshot before, after;
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &before) == UMI_STATUS_OK);
    UmiDocumentNavigationHistorySnapshot history, following;
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(fixture.documents, &history) == UMI_STATUS_OK);
    size_t offset = 111U, bytes = 222U;
    const char *proof = strcmp(mode, "arguments") == 0 ? NULL : source;
    if (proof == NULL)
        expected = UMI_STATUS_INVALID_ARGUMENT;
    CHECK(UmiDocumentCoordinatorSelectCapturedSourceRange(fixture.documents, target, captured_uri,
                                                          proof, strlen(source), first, last,
                                                          &offset, &bytes) == expected);
    CHECK(umi_document_coordinator_active_snapshot(fixture.documents, &after) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(fixture.documents, &following) == UMI_STATUS_OK);
    if (expected == UMI_STATUS_OK)
    {
        CHECK(offset == expected_offset && bytes == expected_bytes &&
              after.document_id == fixture.id);
        CHECK(umi_ui_document_view_model_find(views, fixture.viewId, &view) == UMI_STATUS_OK);
        CHECK(view.cursor_offset == offset && view.selection_length == bytes);
        CHECK(ExpectText(&fixture, source) == 0);
        if (strcmp(mode, "inactive") != 0)
            CHECK(after.undo_count == before.undo_count && after.redo_count == before.redo_count &&
                  after.dirty == before.dirty);
    }
    else
    {
        CHECK(offset == 111U && bytes == 222U && after.document_id == before.document_id);
        CHECK(following.revision == history.revision);
        CHECK(after.undo_count == before.undo_count && after.redo_count == before.redo_count);
    }
    Stop(&fixture);
    return 0;
}
