/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_bookmarks.c
 * PURPOSE: Check bookmark identity, bounded storage and navigation without saving drafts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/document.h"
#include "umicom/document/navigation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)
typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiUiDocumentViewSnapshot view;
    UmiDocumentBookmarksSnapshot bookmarks;
} Fixture;

/* Inspect the live view: bookmarks must never replace it with captured text. */
static int Observe(Fixture *f)
{
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), active.view_id, &f->view) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorBookmarks(f->documents, &f->bookmarks) == UMI_STATUS_OK);
    CHECK(!f->bookmarks.busy);
    return 0;
}
/* Mouse-like caret movement avoids creating unrelated navigation history. */
static int Caret(Fixture *f, size_t offset)
{
    CHECK(Observe(f) == 0); f->view.cursor_offset = offset; f->view.selection_length = 0U;
    CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &f->view) == UMI_STATUS_OK);
    return 0;
}
static int Draft(Fixture *f, const char *text)
{
    CHECK(Observe(f) == 0); f->view.dirty = 1; f->view.cursor_offset = 0U; f->view.selection_length = 0U;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &f->view, text, strlen(text)) == UMI_STATUS_OK);
    return 0;
}
static int Visit(Fixture *f, int direction, size_t offset)
{
    CHECK(Observe(f) == 0);
    CHECK(UmiDocumentCoordinatorTravelBookmark(f->documents, direction, f->bookmarks.revision) == UMI_STATUS_OK);
    CHECK(Observe(f) == 0 && f->view.cursor_offset == offset);
    return 0;
}
static int Run(Fixture *f, const char *name)
{
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.bookmarks", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(f->store, f->workbench, NULL, &f->documents) == UMI_STATUS_OK);
    int added = 99;
    CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, &added) == UMI_STATUS_NOT_FOUND && added == 99);
    CHECK(umi_document_coordinator_new(f->documents, "bookmarks.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(Draft(f, "first\nsecond\nthird\nfourth\n") == 0);
    CHECK(Observe(f) == 0 && f->bookmarks.count == 0U);
    UmiDocumentBookmark row = {0}; row.line = 999U;
    if (strcmp(name, "invalid-empty") == 0) {
        CHECK(UmiDocumentCoordinatorBookmarks(NULL, &f->bookmarks) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorBookmarks(f->documents, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorBookmarkAt(f->documents, 0U, &row) == UMI_STATUS_NOT_FOUND && row.line == 999U);
        CHECK(UmiDocumentCoordinatorBookmarkAt(f->documents, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorToggleBookmark(NULL, &added) == UMI_STATUS_INVALID_ARGUMENT && added == 99);
        CHECK(UmiDocumentCoordinatorTravelBookmark(NULL, 1, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorTravelBookmark(f->documents, 0, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorTravelBookmark(f->documents, 1, 0U) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiDocumentCoordinatorClearBookmarks(f->documents, 0U) == UMI_STATUS_OK);
        return 0;
    }
    CHECK(Caret(f, 7U) == 0);
    CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, &added) == UMI_STATUS_OK && added == 1);
    CHECK(Observe(f) == 0 && f->bookmarks.count == 1U && f->bookmarks.open_count == 1U);
    CHECK(UmiDocumentCoordinatorBookmarkAt(f->documents, 0U, &row) == UMI_STATUS_OK);
    CHECK(row.line == 2U && row.column == 2U && row.open && strcmp(row.display_name, "bookmarks.c") == 0);
    uint64_t revision = f->bookmarks.revision;
    if (strcmp(name, "toggle-line") == 0) {
        CHECK(Caret(f, 10U) == 0);
        CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, &added) == UMI_STATUS_OK && added == 0);
        CHECK(Observe(f) == 0 && f->bookmarks.count == 0U && f->bookmarks.revision == revision + 1U);
    } else if (strcmp(name, "stale") == 0) {
        CHECK(Caret(f, 13U) == 0);
        CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorTravelBookmark(f->documents, 1, revision) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentCoordinatorClearBookmarks(f->documents, revision) == UMI_STATUS_INVALID_STATE);
        CHECK(Observe(f) == 0 && f->bookmarks.count == 2U && f->view.cursor_offset == 13U);
    } else if (strcmp(name, "clear") == 0) {
        CHECK(UmiDocumentCoordinatorClearBookmarks(f->documents, revision) == UMI_STATUS_OK);
        CHECK(Observe(f) == 0 && f->bookmarks.count == 0U && f->view.cursor_offset == 7U && f->view.dirty);
        CHECK(UmiDocumentCoordinatorClearBookmarks(f->documents, revision) == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "readonly") == 0) {
        f->view.read_only = 1; f->view.pinned = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &f->view) == UMI_STATUS_OK);
        CHECK(Caret(f, 0U) == 0 && Visit(f, 1, 7U) == 0);
        CHECK(f->view.read_only && f->view.pinned && f->view.dirty);
        CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, &added) == UMI_STATUS_OK && !added);
    } else if (strcmp(name, "deleted-line") == 0) {
        CHECK(Draft(f, "short") == 0);
        UmiDocumentNavigationHistorySnapshot before, after;
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(f->documents, &before) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorTravelBookmark(f->documents, 1, revision) == UMI_STATUS_NOT_FOUND);
        CHECK(Observe(f) == 0 && f->view.cursor_offset == 0U);
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(f->documents, &after) == UMI_STATUS_OK && before.revision == after.revision);
    } else if (strcmp(name, "utf8-clamp") == 0) {
        CHECK(Caret(f, 10U) == 0);
        CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        CHECK(Draft(f, "first\ncaf\xc3\xa9\n") == 0);
        CHECK(Visit(f, 1, 9U) == 0);
    } else if (strcmp(name, "column-clamp") == 0) {
        CHECK(Draft(f, "first\n\n") == 0 && Visit(f, 1, 6U) == 0);
    } else if (strcmp(name, "text-history") == 0) {
        /* Location metadata must not synchronise pending typing or grow Undo. */
        UmiDocumentWorkingCopySnapshot before, after;
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &before) == UMI_STATUS_OK);
        CHECK(Caret(f, 19U) == 0 && Visit(f, 1, 7U) == 0);
        CHECK(UmiDocumentCoordinatorClearBookmarks(f->documents, revision) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &after) == UMI_STATUS_OK);
        CHECK(before.document_id == after.document_id && before.undo_count == after.undo_count && before.redo_count == after.redo_count);
        CHECK(Observe(f) == 0 && f->view.dirty && strcmp(f->view.source_text, "first\nsecond\nthird\nfourth\n") == 0);
    } else if (strcmp(name, "full-draft") == 0) {
        /* A location past the presentation preview still uses the entire draft. */
        CHECK(UmiDocumentCoordinatorClearBookmarks(f->documents, revision) == UMI_STATUS_OK);
        size_t bytes = UMI_UI_DOCUMENT_CONTENT_CAPACITY + 100U;
        char *text = malloc(bytes + 1U); CHECK(text != NULL);
        memset(text, 'x', bytes); text[bytes - 6U] = '\n'; text[bytes] = '\0';
        int result = Draft(f, text); free(text); CHECK(result == 0);
        CHECK(Caret(f, bytes - 2U) == 0 && UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        CHECK(Caret(f, 0U) == 0 && Visit(f, 1, bytes - 2U) == 0);
        UmiUiDocumentTextInfo info;
        CHECK(UmiUiDocumentViewModelTextInfo(umi_ui_workbench_documents(f->workbench), f->view.view_id, &info) == UMI_STATUS_OK);
        CHECK(info.byte_count == bytes && !info.preview_complete);
    } else if (strcmp(name, "closed-identity") == 0 || strcmp(name, "skip-closed") == 0) {
        CHECK(UmiDocumentCoordinatorClose(f->documents, row.document_id, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorBookmarkAt(f->documents, 0U, &row) == UMI_STATUS_OK && !row.open);
        CHECK(umi_document_coordinator_new(f->documents, "bookmarks.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(Draft(f, "new\ntext\n") == 0);
        CHECK(UmiDocumentCoordinatorTravelBookmark(f->documents, 1, revision) == UMI_STATUS_NOT_FOUND);
        CHECK(Observe(f) == 0 && f->view.cursor_offset == 0U && !f->bookmarks.open_count);
        if (strcmp(name, "skip-closed") == 0) {
            CHECK(Caret(f, 4U) == 0 && UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
            CHECK(Caret(f, 0U) == 0 && Visit(f, 1, 4U) == 0);
        }
    } else if (strcmp(name, "capacity") == 0) {
        CHECK(UmiDocumentCoordinatorClearBookmarks(f->documents, revision) == UMI_STATUS_OK);
        char lines[256]; memset(lines, '\n', sizeof lines - 1U); lines[sizeof lines - 1U] = '\0';
        CHECK(Draft(f, lines) == 0);
        for (size_t index = 0U; index < UMI_DOCUMENT_BOOKMARK_CAPACITY; ++index) {
            CHECK(Caret(f, index) == 0);
            CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        }
        CHECK(Caret(f, UMI_DOCUMENT_BOOKMARK_CAPACITY) == 0);
        added = 99;
        CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, &added) == UMI_STATUS_CAPACITY_EXCEEDED && added == 99);
        CHECK(Observe(f) == 0 && f->bookmarks.count == UMI_DOCUMENT_BOOKMARK_CAPACITY);
        CHECK(Caret(f, 0U) == 0 && UmiDocumentCoordinatorToggleBookmark(f->documents, &added) == UMI_STATUS_OK && !added);
    } else if (strcmp(name, "cross-document") == 0) {
        char first[UMI_UI_ID_CAPACITY]; strcpy(first, f->view.view_id);
        CHECK(umi_document_coordinator_new(f->documents, "second.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(Draft(f, "one\ntwo\n") == 0 && Caret(f, 4U) == 0);
        CHECK(UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        CHECK(Visit(f, 1, 7U) == 0 && strcmp(first, f->view.view_id) == 0);
        CHECK(Visit(f, -1, 4U) == 0 && strcmp(first, f->view.view_id) != 0);
    } else if (strcmp(name, "back-history") == 0) {
        CHECK(Caret(f, 19U) == 0 && Visit(f, 1, 7U) == 0);
        UmiDocumentNavigationHistorySnapshot history;
        CHECK(UmiDocumentCoordinatorNavigationSnapshot(f->documents, &history) == UMI_STATUS_OK && history.can_go_back);
        CHECK(UmiDocumentCoordinatorTravel(f->documents, -1, history.revision, NULL) == UMI_STATUS_OK);
        CHECK(Observe(f) == 0 && f->view.cursor_offset == 19U);
    } else if (strcmp(name, "wrap") == 0) {
        CHECK(Caret(f, 13U) == 0 && UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        CHECK(Caret(f, 19U) == 0 && UmiDocumentCoordinatorToggleBookmark(f->documents, NULL) == UMI_STATUS_OK);
        CHECK(Visit(f, 1, 7U) == 0 && Visit(f, -1, 19U) == 0);
        CHECK(Caret(f, 0U) == 0 && Visit(f, -1, 19U) == 0);
    } else return 2;
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"invalid-empty", "toggle-line", "stale", "clear", "readonly", "deleted-line", "utf8-clamp", "column-clamp", "closed-identity", "skip-closed", "capacity", "cross-document", "back-history", "wrap", "text-history", "full-draft"};
    int known = 0;
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index) if (strcmp(argv[1], cases[index]) == 0) known = 1;
    if (!known) return 2;
    Fixture *f = calloc(1U, sizeof *f); if (f == NULL) return 1;
    int result = Run(f, argv[1]);
    umi_document_coordinator_destroy(f->documents); umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench); umi_command_registry_destroy(f->commands); free(f);
    return result;
}
