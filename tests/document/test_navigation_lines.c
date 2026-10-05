/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_navigation_lines.c
 * PURPOSE: Exercise line jumps, history and bookmarks against mixed line endings without changing source bytes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "replacement_fixture.h"
#include "umicom/document/navigation.h"
#include "umicom/document/navigation_history.h"
#include "umicom/document/bookmarks.h"
#include "umicom/document/source_navigation.h"
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *mode = argv[1];
    const char *known[] = {"lf",           "crlf",          "cr",           "mixed",      "unicode",
                           "column-clamp", "unicode-clamp", "missing-line", "empty-tail", "bookmark",
                           "source-range", "crlf-interior", "large"};
    int found = 0;
    for (size_t i = 0U; i < sizeof(known) / sizeof(known[0]); ++i)
        if (strcmp(mode, known[i]) == 0)
            found = 1;
    CHECK(found);
    const char *text = "first\rsecond\rthird";
    size_t target = 7U, line = 2U, column = 2U;
    char *owned = NULL;
    if (strcmp(mode, "lf") == 0)
        text = "first\nsecond\nthird";
    if (strcmp(mode, "crlf") == 0)
    {
        text = "first\r\nsecond\r\nthird";
        target = 8U;
    }
    if (strcmp(mode, "mixed") == 0)
    {
        text = "first\rsecond\r\nthird\nfourth";
        line = 3U;
        target = 15U;
    }
    if (strcmp(mode, "unicode") == 0 || strcmp(mode, "unicode-clamp") == 0)
    {
        text = "a\rx\xf0\x9f\x8c\x8d"
               "y\rz";
        column = strcmp(mode, "unicode") == 0 ? 6U : 3U;
        target = strcmp(mode, "unicode") == 0 ? 7U : 3U;
    }
    if (strcmp(mode, "column-clamp") == 0)
    {
        column = 999U;
        target = 12U;
    }
    if (strcmp(mode, "empty-tail") == 0)
    {
        text = "a\r\nb\r";
        line = 3U;
        column = 99U;
        target = 5U;
    }
    if (strcmp(mode, "large") == 0)
    {
        owned = malloc(9005U);
        CHECK(owned != NULL);
        memset(owned, 'x', 9000U);
        memcpy(owned + 9000U, "\rabc", 5U);
        text = owned;
        target = 9002U;
    }
    ReplacementFixture f = {0};
    CHECK(Start(&f, text) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f.workbench);
    UmiUiDocumentViewSnapshot view;
    UmiDocumentNavigationHistorySnapshot history;
    size_t offset = 999U;
    CHECK(UmiDocumentCoordinatorGoToPosition(f.documents, line, column, &offset) == UMI_STATUS_OK &&
          offset == target);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == target);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorTravel(f.documents, -1, history.revision, NULL) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == 0U);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorTravel(f.documents, 1, history.revision, NULL) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
          view.cursor_offset == target);
    if (strcmp(mode, "missing-line") == 0)
    {
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history) == UMI_STATUS_OK);
        uint64_t revision = history.revision;
        offset = 999U;
        CHECK(UmiDocumentCoordinatorGoToPosition(f.documents, 9U, 1U, &offset) == UMI_STATUS_NOT_FOUND &&
              offset == 999U);
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history) == UMI_STATUS_OK &&
              history.revision == revision);
    }
    if (strcmp(mode, "bookmark") == 0 || strcmp(mode, "crlf-interior") == 0)
    {
        if (strcmp(mode, "crlf-interior") == 0)
        {
            text = "first\r\nsecond";
            CHECK(Draft(&f, text) == UMI_STATUS_OK);
            CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
            view.cursor_offset = 6U;
            CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
            line = 1U;
            column = 6U;
            target = 5U;
        }
        CHECK(UmiDocumentCoordinatorToggleBookmark(f.documents, NULL) == UMI_STATUS_OK);
        UmiDocumentBookmark bookmark;
        CHECK(UmiDocumentCoordinatorBookmarkAt(f.documents, 0U, &bookmark) == UMI_STATUS_OK);
        CHECK(bookmark.line == line && bookmark.column == column);
        CHECK(UmiDocumentCoordinatorGoToPosition(f.documents, 3U == line ? 1U : 3U, 1U, NULL) ==
              (strcmp(mode, "crlf-interior") == 0 ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
        /* Move the caret without creating a jump, then use the retained bookmark. */
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK);
        view.cursor_offset = 0U;
        CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
        UmiDocumentBookmarksSnapshot bookmarks;
        CHECK(UmiDocumentCoordinatorBookmarks(f.documents, &bookmarks) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorTravelBookmark(f.documents, 1, bookmarks.revision) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == target);
    }
    if (strcmp(mode, "source-range") == 0)
    {
        UmiEditorTextPosition start = {2U, 1U}, end = {2U, 3U};
        CHECK(UmiDocumentCoordinatorSelectSourceRange(f.documents, f.id, start, end, NULL, NULL) ==
              UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(f.documents, &history) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorTravel(f.documents, -1, history.revision, NULL) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, f.viewId, &view) == UMI_STATUS_OK &&
              view.cursor_offset == target);
    }
    CHECK(ExpectText(&f, text) == 0);
    Stop(&f);
    free(owned);
    return 0;
}
