/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_reopen.c
 * PURPOSE: Exercise closed-document history through real providers and working copies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/reopen.h"
#include "umicom/document/close.h"
#include "umicom/document/local_provider.h"
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
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)

/* The wrapper counts real reads/writes and probes nested calls at the provider
 * boundary. Its only synthetic input is an explicitly selected read failure. */
typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiDocumentProvider local;
    UmiDocumentClosePlan *plan;
    char root[UMI_PATH_CAPACITY];
    char path[UMI_DOCUMENT_REOPEN_CAPACITY + 2U][UMI_PATH_CAPACITY];
    char view[UMI_UI_ID_CAPACITY];
    UmiDocumentId active;
    size_t reads, writes;
    int made_root, fail_read, redirect_relative, inspect_nested;
    uint64_t expected;
    UmiStatus nested_reopen, nested_forget, nested_close;
    int nested_busy;
} Fixture;

static UmiStatus Read(void *context, const char *path, unsigned char **bytes, size_t *size)
{
    Fixture *f = context;
    ++f->reads;
    if (f->inspect_nested) {
        UmiDocumentId document = 77U;
        UmiDocumentReopenSnapshot snapshot;
        f->nested_reopen = UmiDocumentCoordinatorReopenLast(f->documents, f->expected, &document);
        f->nested_forget = UmiDocumentCoordinatorForgetClosed(f->documents, f->expected);
        f->nested_close = UmiDocumentCoordinatorClose(f->documents, f->active, 1);
        f->nested_busy = UmiDocumentCoordinatorReopenSnapshot(f->documents, &snapshot) == UMI_STATUS_OK &&
            snapshot.busy && document == 77U;
    }
    if (f->fail_read) return UMI_STATUS_IO_ERROR;
    return f->local.read(f->local.instance, f->redirect_relative ? f->path[0] : path, bytes, size);
}

static UmiStatus Write(void *context, const char *path, const void *bytes, size_t size, int atomic)
{
    Fixture *f = context;
    ++f->writes;
    return f->local.write(f->local.instance, path, bytes, size, atomic);
}

static UmiStatus Stat(void *context, const char *path, UmiDocumentFileInfo *info)
{
    Fixture *f = context;
    return f->local.stat(f->local.instance, f->redirect_relative ? f->path[0] : path, info);
}

static void Release(void *context, void *bytes)
{
    Fixture *f = context;
    umi_document_provider_release_bytes(&f->local, bytes);
}

/* Every case gets a newly created absolute directory, independent of CWD. */
static int Start(Fixture *f, const char *name)
{
    char temp[UMI_PATH_CAPACITY], leaf[128];
    CHECK(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof(leaf), "umicom-reopen-%ld-%s", (long)PROCESS_ID(), name);
    CHECK(umi_fs_join(f->root, sizeof(f->root), temp, leaf) == UMI_STATUS_OK);
    CHECK(!umi_fs_exists(f->root));
    CHECK(umi_fs_make_directories(f->root) == UMI_STATUS_OK); f->made_root = 1;
    for (size_t index = 0U; index < UMI_DOCUMENT_REOPEN_CAPACITY + 2U; ++index) {
        (void)snprintf(leaf, sizeof(leaf), "notes-%zu.c", index);
        CHECK(umi_fs_join(f->path[index], sizeof(f->path[index]), f->root, leaf) == UMI_STATUS_OK);
    }
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.document.reopen", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    f->local = umi_document_local_provider();
    UmiDocumentProvider provider = f->local;
    provider.instance = f; provider.read = Read; provider.write = Write;
    provider.stat = Stat; provider.release_bytes = Release;
    /* This fixture delegates only read, write and stat; do not expose unused
     * local callbacks with a different instance pointer. */
    provider.remove = NULL; provider.rename = NULL;
    provider.flags &= ~(uint32_t)(UMI_DOCUMENT_PROVIDER_REMOVE | UMI_DOCUMENT_PROVIDER_RENAME);
    CHECK(umi_document_coordinator_create(f->store, f->workbench, &provider, &f->documents) == UMI_STATUS_OK);
    return 0;
}

static void Stop(Fixture *f)
{
    UmiDocumentClosePlanDestroy(f->plan);
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
    if (f->made_root) (void)umi_fs_remove_tree(f->root);
}

static int Open(Fixture *f, size_t index, int create)
{
    if (create) CHECK(umi_fs_write_text(f->path[index], "saved text\n") == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_open(f->documents,
        f->redirect_relative ? "notes-0.c" : f->path[index], f->view, sizeof(f->view)) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
    f->active = snapshot.document_id;
    return 0;
}

static int Draft(Fixture *f, const char *text)
{
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(f->workbench), &view, text, strlen(text)) == UMI_STATUS_OK);
    return 0;
}

static int ActiveText(Fixture *f, const char *expected)
{
    UmiDocumentWorkingCopySnapshot snapshot;
    char *text = NULL; size_t bytes = 0U;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench), snapshot.view_id, &text, &bytes) == UMI_STATUS_OK);
    int matches = bytes == strlen(expected) && memcmp(text, expected, bytes) == 0;
    UmiUiDocumentViewModelFreeText(text);
    CHECK(matches); return 0;
}

static int Run(Fixture *f, const char *name)
{
    UmiDocumentReopenSnapshot before, after;
    UmiDocumentId reopened = 77U;
    CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &before) == UMI_STATUS_OK);
    CHECK(before.count == 0U && before.revision == 0U && !before.busy && before.next_name[0] == '\0');
    if (strcmp(name, "empty-invalid") == 0) {
        after = before; after.count = 99U;
        CHECK(UmiDocumentCoordinatorReopenSnapshot(NULL, &after) == UMI_STATUS_INVALID_ARGUMENT && after.count == 99U);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorReopenLast(NULL, 0U, &reopened) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorReopenLast(f->documents, 0U, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorReopenLast(f->documents, 0U, &reopened) == UMI_STATUS_NOT_FOUND && reopened == 77U);
        CHECK(UmiDocumentCoordinatorForgetClosed(NULL, 0U) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiDocumentCoordinatorForgetClosed(f->documents, 0U) == UMI_STATUS_NOT_FOUND);
        CHECK(f->reads == 0U && f->writes == 0U); return 0;
    }
    if (strcmp(name, "untitled") == 0) {
        CHECK(umi_document_coordinator_new(f->documents, "unsaved.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_close_active(f->documents, 1) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK && after.count == 0U);
        CHECK(f->reads == 0U && f->writes == 0U); return 0;
    }
    if (strcmp(name, "capacity") == 0) {
        for (size_t index = 0U; index < UMI_DOCUMENT_REOPEN_CAPACITY + 2U; ++index) {
            CHECK(Open(f, index, 1) == 0);
            CHECK(UmiDocumentCoordinatorClose(f->documents, f->active, 0) == UMI_STATUS_OK);
        }
        for (size_t left = UMI_DOCUMENT_REOPEN_CAPACITY; left > 0U; --left) {
            CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &before) == UMI_STATUS_OK && before.count == left);
            CHECK(UmiDocumentCoordinatorReopenLast(f->documents, before.revision, &reopened) == UMI_STATUS_OK);
            UmiDocumentWorkingCopySnapshot active;
            CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
            CHECK(umi_path_equal(active.path, f->path[left + 1U]));
        }
        CHECK(umi_document_coordinator_count(f->documents) == UMI_DOCUMENT_REOPEN_CAPACITY);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK && after.count == 0U);
        CHECK(f->writes == 0U); return 0;
    }
    if (strcmp(name, "relative") == 0) f->redirect_relative = 1;
    CHECK(Open(f, 0U, 1) == 0);
    if (strcmp(name, "cancel") == 0 || strcmp(name, "discard") == 0 || strcmp(name, "save") == 0) {
        CHECK(Draft(f, "draft text\n") == 0);
        CHECK(UmiDocumentCoordinatorPrepareClose(f->documents, f->active, &f->plan) == UMI_STATUS_OK);
        UmiDocumentCloseDecision decision = strcmp(name, "cancel") == 0 ? UMI_DOCUMENT_CLOSE_CANCEL :
            strcmp(name, "save") == 0 ? UMI_DOCUMENT_CLOSE_SAVE : UMI_DOCUMENT_CLOSE_DISCARD;
        CHECK(UmiDocumentCoordinatorApplyClose(f->documents, f->plan, decision, NULL) ==
            (decision == UMI_DOCUMENT_CLOSE_CANCEL ? UMI_STATUS_CANCELLED : UMI_STATUS_OK));
        if (decision == UMI_DOCUMENT_CLOSE_CANCEL) {
            CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK && after.count == 0U);
            CHECK(ActiveText(f, "draft text\n") == 0 && f->writes == 0U); return 0;
        }
    } else CHECK(UmiDocumentCoordinatorClose(f->documents, f->active, 0) == UMI_STATUS_OK);
    CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &before) == UMI_STATUS_OK);
    if (strcmp(name, "relative") == 0) { CHECK(before.count == 0U && before.revision == 0U); return 0; }
    CHECK(before.count == 1U && before.revision == 1U && strcmp(before.next_name, "notes-0.c") == 0);
    size_t reads = f->reads;
    if (strcmp(name, "stale") == 0) {
        CHECK(Open(f, 1U, 1) == 0);
        CHECK(UmiDocumentCoordinatorClose(f->documents, f->active, 0) == UMI_STATUS_OK);
        reads = f->reads;
        CHECK(UmiDocumentCoordinatorReopenLast(f->documents, before.revision, &reopened) == UMI_STATUS_INVALID_STATE);
        CHECK(reopened == 77U && f->reads == reads);
        CHECK(UmiDocumentCoordinatorForgetClosed(f->documents, before.revision) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK && after.count == 2U);
        return 0;
    }
    if (strcmp(name, "forget") == 0) {
        CHECK(UmiDocumentCoordinatorForgetClosed(f->documents, before.revision) == UMI_STATUS_OK);
        CHECK(umi_fs_exists(f->path[0]) && f->reads == reads && f->writes == 0U);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK && after.count == 0U && after.revision == 2U);
        return 0;
    }
    if (strcmp(name, "duplicate") == 0) {
        CHECK(Open(f, 1U, 1) == 0);
        CHECK(UmiDocumentCoordinatorClose(f->documents, f->active, 0) == UMI_STATUS_OK);
        CHECK(Open(f, 0U, 0) == 0);
        CHECK(UmiDocumentCoordinatorClose(f->documents, f->active, 0) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &before) == UMI_STATUS_OK);
        CHECK(before.count == 2U && before.revision == 3U && strcmp(before.next_name, "notes-0.c") == 0);
        CHECK(UmiDocumentCoordinatorForgetClosed(f->documents, before.revision) == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK);
        CHECK(after.count == 1U && strcmp(after.next_name, "notes-1.c") == 0); return 0;
    }
    if (strcmp(name, "read-failure") == 0 || strcmp(name, "missing") == 0) {
        if (strcmp(name, "missing") == 0) CHECK(umi_fs_rename(f->path[0], f->path[1]) == UMI_STATUS_OK);
        else f->fail_read = 1;
        CHECK(UmiDocumentCoordinatorReopenLast(f->documents, before.revision, &reopened) != UMI_STATUS_OK && reopened == 77U);
        CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK);
        CHECK(after.count == before.count && after.revision == before.revision && !after.busy);
        CHECK(umi_document_coordinator_count(f->documents) == 0U);
        if (strcmp(name, "missing") == 0) CHECK(umi_fs_rename(f->path[1], f->path[0]) == UMI_STATUS_OK);
        f->fail_read = 0;
    }
    if (strcmp(name, "current-disk") == 0) CHECK(umi_fs_write_text(f->path[0], "new disk text\n") == UMI_STATUS_OK);
    if (strcmp(name, "already-open") == 0) {
        CHECK(Open(f, 0U, 0) == 0);
        CHECK(Draft(f, "keep this draft\n") == 0);
        reads = f->reads;
    }
    if (strcmp(name, "nested") == 0) { f->expected = before.revision; f->inspect_nested = 1; }
    CHECK(UmiDocumentCoordinatorReopenLast(f->documents, before.revision, &reopened) == UMI_STATUS_OK);
    CHECK(reopened != 0U && umi_document_coordinator_count(f->documents) == 1U);
    CHECK(UmiDocumentCoordinatorReopenSnapshot(f->documents, &after) == UMI_STATUS_OK);
    CHECK(after.count == 0U && after.revision == before.revision + 1U && !after.busy);
    const char *expected = strcmp(name, "current-disk") == 0 ? "new disk text\n" :
        strcmp(name, "already-open") == 0 ? "keep this draft\n" : strcmp(name, "save") == 0 ? "draft text\n" : "saved text\n";
    CHECK(ActiveText(f, expected) == 0);
    if (strcmp(name, "already-open") == 0) CHECK(reopened == f->active && f->reads == reads);
    else CHECK(reopened != f->active);
    if (strcmp(name, "nested") == 0)
        CHECK(f->nested_reopen == UMI_STATUS_BUSY && f->nested_forget == UMI_STATUS_BUSY &&
            f->nested_close == UMI_STATUS_BUSY && f->nested_busy);
    CHECK(f->writes == (strcmp(name, "save") == 0 ? 1U : 0U));
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *cases[] = {"empty-invalid", "untitled", "capacity", "relative", "cancel", "discard", "save",
        "stale", "forget", "duplicate", "read-failure", "missing", "current-disk", "already-open", "nested"};
    int known = 0;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index)
        if (strcmp(argv[1], cases[index]) == 0) known = 1;
    if (!known) return 2;
    Fixture *fixture = calloc(1U, sizeof(*fixture));
    if (fixture == NULL) return 1;
    int result = Start(fixture, argv[1]);
    if (result == 0) result = Run(fixture, argv[1]);
    Stop(fixture); free(fixture);
    return result;
}
