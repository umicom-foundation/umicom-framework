/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_navigation_history.c
 * PURPOSE: Exercise live document history, current drafts and failed destinations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/navigation_history.h"
#include "umicom/platform/filesystem.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <process.h>
#define PROCESS_ID _getpid
#else
#include <unistd.h>
#define PROCESS_ID getpid
#endif
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); return 1; } } while (0)

/* All state is heap-owned. Test failures still return through the outer cleanup
 * path, and temporary files are confined to a newly created private directory. */
typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiUiDocumentViewSnapshot view;
    UmiDocumentNavigationHistorySnapshot history;
    char root[UMI_PATH_CAPACITY];
    int made_root;
} Fixture;

static int Observe(Fixture *f)
{
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f->documents, &f->history) == UMI_STATUS_OK);
    CHECK(!f->history.busy && f->history.last_record_status == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot active;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), active.view_id, &f->view) == UMI_STATUS_OK);
    return 0;
}

static int Draft(Fixture *f, const char *text)
{
    CHECK(Observe(f) == 0);
    f->view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &f->view, text, strlen(text)) == UMI_STATUS_OK);
    return 0;
}

static int Move(Fixture *f, int direction)
{
    CHECK(Observe(f) == 0);
    CHECK(UmiDocumentCoordinatorTravel(f->documents, direction, f->history.revision, NULL) == UMI_STATUS_OK);
    return Observe(f);
}

static int Run(Fixture *f, const char *name)
{
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.navigation", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(f->store, f->workbench, NULL, &f->documents) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f->documents, &f->history) == UMI_STATUS_OK);
    CHECK(f->history.count == 0U && !f->history.can_go_back && !f->history.can_go_forward);
    UmiDocumentId id = 999U;
    CHECK(UmiDocumentCoordinatorTravel(f->documents, -1, f->history.revision, &id) == UMI_STATUS_NOT_FOUND && id == 999U);
    CHECK(UmiDocumentCoordinatorTravel(NULL, -1, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiDocumentCoordinatorNavigationSnapshot(f->documents, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_document_coordinator_new(f->documents, "first.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(Draft(f, "first\nsecond\nthird\nfourth\n") == 0);
    char first[UMI_UI_ID_CAPACITY]; strcpy(first, f->view.view_id);
    if (strcmp(name, "empty-invalid") == 0) {
        CHECK(UmiDocumentCoordinatorTravel(f->documents, 2, f->history.revision, &id) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorClearNavigation(f->documents, f->history.revision) == UMI_STATUS_OK);
        CHECK(Observe(f) == 0 && f->history.count == 0U);
        return 0;
    }
    CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 2U, 1U, NULL) == UMI_STATUS_OK);
    CHECK(Observe(f) == 0 && f->history.count == 2U && f->view.cursor_offset == 6U);
    if (strcmp(name, "roundtrip") == 0) {
        /* A mouse-like metadata edit changes the departure without adding a jump. */
        f->view.cursor_offset = 13U;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &f->view) == UMI_STATUS_OK);
        CHECK(Move(f, -1) == 0 && f->view.cursor_offset == 0U);
        CHECK(Move(f, 1) == 0 && f->view.cursor_offset == 13U && f->view.dirty);
    } else if (strcmp(name, "stale") == 0) {
        uint64_t before = f->history.revision;
        CHECK(Move(f, -1) == 0);
        CHECK(UmiDocumentCoordinatorTravel(f->documents, 1, before, &id) == UMI_STATUS_INVALID_STATE && id == 999U);
        CHECK(UmiDocumentCoordinatorClearNavigation(f->documents, before) == UMI_STATUS_INVALID_STATE);
        CHECK(Observe(f) == 0 && f->view.cursor_offset == 0U);
    } else if (strcmp(name, "branch") == 0) {
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 3U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(Move(f, -1) == 0 && f->history.can_go_forward);
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 4U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(Observe(f) == 0 && !f->history.can_go_forward && f->history.count == 3U);
    } else if (strcmp(name, "missing-line") == 0) {
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 3U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 2U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(Draft(f, "short") == 0 && Observe(f) == 0);
        uint64_t revision = f->history.revision;
        size_t offset = f->view.cursor_offset;
        CHECK(UmiDocumentCoordinatorTravel(f->documents, -1, revision, &id) == UMI_STATUS_NOT_FOUND && id == 999U);
        CHECK(Observe(f) == 0 && f->history.revision == revision && f->view.cursor_offset == offset);
    } else if (strcmp(name, "utf8") == 0) {
        CHECK(Draft(f, "caf\xc3\xa9\nnext\n") == 0);
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 1U, 5U, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 2U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(Move(f, -1) == 0 && f->view.cursor_offset == 3U);
    } else if (strcmp(name, "readonly") == 0) {
        f->view.read_only = 1; f->view.pinned = 1;
        CHECK(umi_ui_document_view_model_upsert(umi_ui_workbench_documents(f->workbench), &f->view) == UMI_STATUS_OK);
        CHECK(Move(f, -1) == 0 && f->view.read_only && f->view.pinned && f->view.dirty);
        CHECK(strcmp(f->view.source_text, "first\nsecond\nthird\nfourth\n") == 0);
    } else if (strcmp(name, "noop-failure") == 0) {
        uint64_t revision = f->history.revision;
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 2U, 1U, NULL) == UMI_STATUS_OK);
        size_t offset = 777U;
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 50U, 1U, &offset) == UMI_STATUS_NOT_FOUND && offset == 777U);
        CHECK(Observe(f) == 0 && f->history.revision == revision);
    } else if (strcmp(name, "clear") == 0) {
        CHECK(UmiDocumentCoordinatorClearNavigation(f->documents, f->history.revision) == UMI_STATUS_OK);
        CHECK(Observe(f) == 0 && f->history.count == 0U && f->view.cursor_offset == 6U && f->view.dirty);
    } else if (strcmp(name, "capacity") == 0) {
        for (size_t index = 0U; index < UMI_DOCUMENT_NAVIGATION_CAPACITY + 3U; ++index)
            CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 1U + index % 4U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(Observe(f) == 0 && f->history.count == UMI_DOCUMENT_NAVIGATION_CAPACITY);
        size_t visited = 0U;
        while (f->history.can_go_back) { CHECK(Move(f, -1) == 0); ++visited; }
        CHECK(visited == UMI_DOCUMENT_NAVIGATION_CAPACITY - 1U);
    } else if (strcmp(name, "large") == 0) {
        size_t length = UMI_UI_DOCUMENT_CONTENT_CAPACITY + 100U;
        char *large = malloc(length + 1U);
        CHECK(large != NULL);
        memset(large, 'x', length); large[length - 6U] = '\n'; large[length] = '\0';
        UmiStatus status = UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &f->view, large, length);
        free(large); CHECK(status == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 2U, 4U, NULL) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 1U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(Move(f, -1) == 0 && f->view.cursor_offset == length - 2U);
        UmiUiDocumentTextInfo info;
        CHECK(UmiUiDocumentViewModelTextInfo(umi_ui_workbench_documents(f->workbench), first, &info) == UMI_STATUS_OK);
        CHECK(info.byte_count == length && !info.preview_complete);
    } else if (strcmp(name, "cross-document") == 0 || strcmp(name, "closed") == 0) {
        UmiDocumentWorkingCopySnapshot original;
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &original) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_new(f->documents, "second.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(Draft(f, "one\ntwo\nthree\n") == 0);
        CHECK(UmiDocumentCoordinatorGoToPosition(f->documents, 3U, 1U, NULL) == UMI_STATUS_OK);
        CHECK(Move(f, -1) == 0 && strcmp(first, f->view.view_id) != 0);
        if (strcmp(name, "closed") == 0) {
            CHECK(UmiDocumentCoordinatorClose(f->documents, original.document_id, 1) == UMI_STATUS_OK);
            CHECK(Observe(f) == 0);
            uint64_t revision = f->history.revision;
            CHECK(UmiDocumentCoordinatorTravel(f->documents, -1, revision, &id) == UMI_STATUS_NOT_FOUND);
            CHECK(Observe(f) == 0 && f->history.revision == revision && strcmp(first, f->view.view_id) != 0);
        } else {
            CHECK(Move(f, -1) == 0 && strcmp(first, f->view.view_id) == 0 && f->view.cursor_offset == 6U);
            CHECK(Move(f, 1) == 0 && strcmp(first, f->view.view_id) != 0);
        }
    } else if (strcmp(name, "search-open") == 0) {
        char temp[UMI_PATH_CAPACITY], leaf[80], path[UMI_PATH_CAPACITY];
        CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
        (void)snprintf(leaf, sizeof(leaf), "umicom-navigation-%ld", (long)PROCESS_ID());
        CHECK(umi_fs_join(f->root, sizeof(f->root), temp, leaf) == UMI_STATUS_OK);
        CHECK(!umi_fs_exists(f->root));
        CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK); f->made_root = 1;
        CHECK(umi_fs_join(path, sizeof(path), f->root, "source.c") == UMI_STATUS_OK);
        CHECK(umi_fs_write_text(path, "heading\nneedle here\n") == UMI_STATUS_OK);
        UmiSearchMatch match = {0}; strcpy(match.path, path); match.line = 2U; match.column = 1U;
        CHECK(UmiDocumentCoordinatorOpenSearchMatch(f->documents, &match, "needle", 1, NULL) == UMI_STATUS_OK);
        CHECK(Observe(f) == 0 && f->history.count == 3U && f->view.cursor_offset == 8U);
        CHECK(Move(f, -1) == 0 && strcmp(first, f->view.view_id) == 0 && f->view.cursor_offset == 6U);
        CHECK(Move(f, 1) == 0 && f->view.cursor_offset == 8U);
        CHECK(UmiDocumentCoordinatorCycle(f->documents, -1, NULL) == UMI_STATUS_OK);
        CHECK(Move(f, -1) == 0 && f->view.cursor_offset == 8U);
    } else return 2;
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *f = calloc(1U, sizeof(*f));
    if (f == NULL) return 1;
    int result = Run(f, argv[1]);
    umi_document_coordinator_destroy(f->documents); umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench); umi_command_registry_destroy(f->commands);
    if (f->made_root && umi_fs_remove_tree(f->root) != UMI_STATUS_OK) result = 1;
    free(f);
    return result;
}
