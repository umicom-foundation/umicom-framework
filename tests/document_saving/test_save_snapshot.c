/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_save_snapshot.c
 * PURPOSE: Exercise provider callbacks through the real document coordinator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/document/document.h"
#include "umicom/document/save_session.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); return 1; } } while (0)
#define DRAFT "draft to save\n"
#define LATE "newer unsaved text\n"

typedef struct Fixture {
    UmiCommandRegistry *commands;
    UmiUiWorkbench *workbench;
    UmiDocumentStore *store;
    UmiDocumentCoordinator *documents;
    UmiDocumentSaveSession *session;
    UmiDocumentId id, other, pending;
    char view[UMI_UI_ID_CAPACITY];
    char path[UMI_PATH_CAPACITY];
    char storedPath[UMI_PATH_CAPACITY];
    char *bytes;
    size_t length;
    unsigned writes;
    const char *hook;
    int hookOnRead;
    UmiStatus hookStatus, readStatus, writeStatus;
} Fixture;

static UmiStatus Edit(Fixture *f, const char *text)
{
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiUiDocumentViewSnapshot view;
    UmiStatus status = umi_ui_document_view_model_find(views, f->view, &view);
    if (status != UMI_STATUS_OK) return status;
    view.dirty = 1;
    return UmiUiDocumentViewModelUpsertText(views, &view, text, strlen(text));
}

static UmiStatus Callback(Fixture *f)
{
    const char *hook = f->hook;
    f->hook = NULL; /* Exactly one callback, even if a regression allows reentry. */
    if (hook == NULL) return UMI_STATUS_OK;
    if (strcmp(hook, "draft") == 0) return Edit(f, LATE);
    if (strcmp(hook, "restored-draft") == 0) {
        UmiStatus status = Edit(f, LATE);
        return status == UMI_STATUS_OK ? Edit(f, DRAFT) : status;
    }
    if (strcmp(hook, "store") == 0)
        return umi_document_store_replace_text(f->store, f->id, LATE, strlen(LATE));
    if (strcmp(hook, "close") == 0) return UmiDocumentCoordinatorClose(f->documents, f->id, 1);
    if (strcmp(hook, "close-other") == 0) return UmiDocumentCoordinatorClose(f->documents, f->other, 1);
    if (strcmp(hook, "close-pending") == 0) return UmiDocumentCoordinatorClose(f->documents, f->pending, 1);
    if (strcmp(hook, "nested") == 0) return UmiDocumentCoordinatorSaveAs(f->documents, f->id, f->path);
    if (strcmp(hook, "nested-all") == 0) {
        size_t saved = 999U;
        UmiStatus status = UmiDocumentCoordinatorSaveAll(f->documents, &saved);
        return saved == 0U ? status : UMI_STATUS_INTERNAL_ERROR;
    }
    if (strcmp(hook, "rename") == 0) return umi_document_store_mark_saved_as(f->store, f->id, "renamed.txt");
    if (strcmp(hook, "external") == 0) return umi_document_store_mark_external_change(f->store, f->id, 1);
    if (strcmp(hook, "occupy") == 0) {
        /* Save As target becomes owned by a different working copy during I/O. */
        return umi_document_store_create_loaded(f->store, "occupied.txt", f->path, "other", 5U, &f->other);
    }
    if (strcmp(hook, "activate") == 0)
        return umi_document_coordinator_new(f->documents, "another.txt", NULL, 0U);
    if (strcmp(hook, "caret") == 0) {
        UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
        UmiUiDocumentViewSnapshot view;
        UmiStatus status = umi_ui_document_view_model_find(views, f->view, &view);
        if (status != UMI_STATUS_OK) return status;
        view.cursor_offset = 2U;
        return umi_ui_document_view_model_upsert(views, &view);
    }
    return UMI_STATUS_INVALID_ARGUMENT;
}

static UmiStatus Read(void *context, const char *path, unsigned char **outBytes, size_t *outLength)
{
    Fixture *f = context;
    if (f->readStatus != UMI_STATUS_OK) return f->readStatus;
    if (!umi_path_equal(path, f->storedPath)) return UMI_STATUS_NOT_FOUND;
    unsigned char *copy = malloc(f->length + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    memcpy(copy, f->bytes, f->length + 1U);
    *outBytes = copy; *outLength = f->length;
    if (f->hookOnRead && f->hook != NULL) f->hookStatus = Callback(f);
    return UMI_STATUS_OK;
}

static UmiStatus Write(void *context, const char *path, const void *bytes, size_t length, int atomic)
{
    Fixture *f = context;
    (void)atomic;
    if (f->writeStatus != UMI_STATUS_OK) return f->writeStatus;
    char *copy = malloc(length + 1U);
    if (copy == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (length != 0U) memcpy(copy, bytes, length);
    copy[length] = '\0';
    free(f->bytes); f->bytes = copy; f->length = length;
    (void)snprintf(f->storedPath, sizeof(f->storedPath), "%s", path);
    ++f->writes;
    if (!f->hookOnRead && f->hook != NULL) f->hookStatus = Callback(f);
    return UMI_STATUS_OK;
}

static UmiStatus Stat(void *context, const char *path, UmiDocumentFileInfo *out)
{
    Fixture *f = context;
    *out = (UmiDocumentFileInfo){0};
    out->exists = umi_path_equal(path, f->storedPath);
    out->regular_file = 1; out->readable = 1; out->writable = 1;
    out->byte_count = f->length;
    return UMI_STATUS_OK;
}
static void Release(void *context, void *bytes) { (void)context; free(bytes); }

static int Start(Fixture *f, int untitled)
{
    /* The provider is entirely in memory. No fixture touches a real file. */
    char root[UMI_PATH_CAPACITY];
    CHECK(umi_fs_temp_directory(root, sizeof(root)) == UMI_STATUS_OK);
    CHECK(umi_fs_join(f->path, sizeof(f->path), root, "umicom-snapshot-\xc3\xa9.txt") == UMI_STATUS_OK);
    CHECK(Write(f, f->path, "original\n", 9U, 1) == UMI_STATUS_OK); f->writes = 0U;
    CHECK(umi_command_registry_create(&f->commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.save.snapshot", f->commands, &f->workbench) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&f->store) == UMI_STATUS_OK);
    UmiDocumentProvider provider = { .struct_size = sizeof(UmiDocumentProvider),
        .abi_version = UMI_DOCUMENT_PROVIDER_ABI_VERSION, .provider_id = "test.snapshot", .scheme = "file",
        .flags = UMI_DOCUMENT_PROVIDER_READ | UMI_DOCUMENT_PROVIDER_WRITE |
            UMI_DOCUMENT_PROVIDER_ATOMIC_WRITE | UMI_DOCUMENT_PROVIDER_STAT,
        .instance = f, .read = Read, .write = Write, .stat = Stat, .release_bytes = Release };
    CHECK(umi_document_coordinator_create(f->store, f->workbench, &provider, &f->documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(f->documents, "preceding.txt", NULL, 0U) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot snapshot;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
    f->other = snapshot.document_id;
    CHECK((untitled ? umi_document_coordinator_new(f->documents, "notes.txt", f->view, sizeof(f->view))
        : umi_document_coordinator_open(f->documents, f->path, f->view, sizeof(f->view))) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &snapshot) == UMI_STATUS_OK);
    f->id = snapshot.document_id;
    return 0;
}

static int TextIs(Fixture *f, const char *expected)
{
    char *text = NULL;
    size_t length = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(f->workbench),
        f->view, &text, &length) == UMI_STATUS_OK);
    int equal = length == strlen(expected) && strcmp(text, expected) == 0;
    UmiUiDocumentViewModelFreeText(text);
    CHECK(equal);
    return 0;
}

static int BatchIdentity(Fixture *f, const char *name)
{
    CHECK(Start(f, 0) == 0);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(f->workbench);
    UmiDocumentWorkingCopySnapshot previous;
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_document_coordinator_at(f->documents, 0U, &previous) == UMI_STATUS_OK);
    /* The preceding document is empty and already saved in this fixture. */
    CHECK(umi_document_store_mark_saved_as(f->store, f->other, "preceding.txt") == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, previous.view_id, &view) == UMI_STATUS_OK);
    view.dirty = 0;
    CHECK(umi_ui_document_view_model_upsert(views, &view) == UMI_STATUS_OK);
    CHECK(Edit(f, DRAFT) == UMI_STATUS_OK);
    char pendingView[UMI_UI_ID_CAPACITY];
    CHECK(umi_document_coordinator_new(f->documents, "pending.txt", pendingView, sizeof(pendingView)) == UMI_STATUS_OK);
    UmiDocumentWorkingCopySnapshot pending;
    CHECK(umi_document_coordinator_active_snapshot(f->documents, &pending) == UMI_STATUS_OK);
    f->pending = pending.document_id;
    CHECK(umi_document_store_mark_saved_as(f->store, f->pending, "pending.txt") == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, pendingView, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "pending draft\n", 14U) == UMI_STATUS_OK);
    int closed = strcmp(name, "batch-close-pending") == 0;
    int newTab = strcmp(name, "batch-new-tab") == 0;
    f->hook = closed ? "close-pending" : newTab ? "activate" : "close-other";
    size_t saved = 999U;
    CHECK(UmiDocumentCoordinatorSaveAll(f->documents, &saved) ==
        (closed ? UMI_STATUS_NOT_FOUND : UMI_STATUS_OK));
    CHECK(f->hookStatus == UMI_STATUS_OK && saved == (closed ? 1U : 2U));
    CHECK(f->writes == (closed ? 1U : 2U));
    CHECK(strcmp(f->bytes, closed ? DRAFT : "pending draft\n") == 0);
    UmiDocumentSnapshot stored;
    CHECK(umi_document_store_snapshot(f->store, f->id, &stored) == UMI_STATUS_OK && !stored.dirty);
    if (!closed)
        CHECK(umi_document_store_snapshot(f->store, f->pending, &stored) == UMI_STATUS_OK && !stored.dirty);
    if (newTab) {
        UmiDocumentWorkingCopySnapshot active;
        CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
        CHECK(active.document_id != f->pending && active.document_id != f->id && !active.has_path);
        CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK && view.dirty);
    }
    return 0;
}

static int Run(Fixture *f, const char *name)
{
    if (strncmp(name, "batch-", 6U) == 0) return BatchIdentity(f, name);
    int untitled = strcmp(name, "untitled") == 0 || strcmp(name, "empty") == 0 || strcmp(name, "occupy") == 0;
    CHECK(Start(f, untitled) == 0);
    /* Batch tests contain only the named target. The extra untitled tab is
     * useful for identity/compaction cases but would legitimately need a path. */
    if (strcmp(name, "save-session") == 0 || strcmp(name, "save-all") == 0)
        CHECK(UmiDocumentCoordinatorClose(f->documents, f->other, 1) == UMI_STATUS_OK);
    const char *draft = strcmp(name, "empty") == 0 ? "" : DRAFT;
    if (strcmp(name, "unicode") == 0) draft = "caf\xc3\xa9 \xe2\x98\x95\n";
    if (draft[0] != '\0') CHECK(Edit(f, draft) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_sync_active(f->documents) == UMI_STATUS_OK);
    UmiDocumentSnapshot before, after;
    CHECK(umi_document_store_snapshot(f->store, f->id, &before) == UMI_STATUS_OK);
    const char *hook = name;
    if (strncmp(name, "read-", 5U) == 0) { f->hookOnRead = 1; hook = name + 5; }
    int simple = strcmp(name, "normal") == 0 || untitled || strcmp(name, "unicode") == 0;
    if (strcmp(name, "read-error") == 0) f->readStatus = UMI_STATUS_IO_ERROR;
    else if (strcmp(name, "write-error") == 0) f->writeStatus = UMI_STATUS_IO_ERROR;
    else if (strcmp(name, "save-session") == 0 || strcmp(name, "save-all") == 0) f->hook = "draft";
    else if (!simple || strcmp(name, "occupy") == 0) f->hook = hook;

    UmiStatus status;
    size_t saved = 999U;
    if (strcmp(name, "save-session") == 0) {
        CHECK(UmiDocumentSaveSessionCreate(f->documents, &f->session) == UMI_STATUS_OK);
        status = UmiDocumentSaveSessionStep(f->session);
        UmiDocumentSaveProgress progress;
        CHECK(UmiDocumentSaveSessionProgress(f->session, &progress) == UMI_STATUS_OK);
        CHECK(progress.phase == UMI_DOCUMENT_SAVE_FAILED && progress.saved == 0U);
        CHECK(progress.last_status == UMI_STATUS_INVALID_STATE);
    } else if (strcmp(name, "save-all") == 0) {
        status = UmiDocumentCoordinatorSaveAll(f->documents, &saved);
        CHECK(saved == 0U);
    } else status = UmiDocumentCoordinatorSaveAs(f->documents, f->id, f->path);

    int failure = strcmp(name, "read-error") == 0 || strcmp(name, "write-error") == 0;
    int closed = strcmp(hook, "close") == 0;
    int success = (simple && strcmp(name, "occupy") != 0) || strcmp(hook, "close-other") == 0 ||
        strcmp(hook, "nested") == 0 || strcmp(hook, "nested-all") == 0 ||
        strcmp(hook, "activate") == 0 || strcmp(hook, "caret") == 0;
    UmiStatus expected = failure ? UMI_STATUS_IO_ERROR : closed ? UMI_STATUS_NOT_FOUND :
        success ? UMI_STATUS_OK : strcmp(name, "occupy") == 0 ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_INVALID_STATE;
    CHECK(status == expected);
    CHECK(f->hookStatus == (strcmp(hook, "nested") == 0 || strcmp(hook, "nested-all") == 0
        ? UMI_STATUS_BUSY : UMI_STATUS_OK));
    CHECK(f->writes == (failure || (f->hookOnRead && !success) ? 0U : 1U));
    if (f->writes != 0U) CHECK(f->length == strlen(draft) && strcmp(f->bytes, draft) == 0);
    if (closed) {
        CHECK(umi_document_store_snapshot(f->store, f->id, &after) == UMI_STATUS_NOT_FOUND);
        CHECK(umi_document_store_snapshot(f->store, f->other, &after) == UMI_STATUS_OK && !after.has_path);
        return 0;
    }
    CHECK(umi_document_store_snapshot(f->store, f->id, &after) == UMI_STATUS_OK);
    if (success) {
        CHECK(!after.dirty && after.saved_revision == before.revision && after.has_path);
        CHECK(TextIs(f, draft) == 0);
        if (strcmp(hook, "activate") == 0) {
            UmiDocumentWorkingCopySnapshot active;
            CHECK(umi_document_coordinator_active_snapshot(f->documents, &active) == UMI_STATUS_OK);
            CHECK(active.document_id != f->id && !active.has_path);
        }
        if (strcmp(hook, "caret") == 0) {
            UmiUiDocumentViewSnapshot view;
            CHECK(umi_ui_document_view_model_find(umi_ui_workbench_documents(f->workbench), f->view, &view) == UMI_STATUS_OK);
            CHECK(view.cursor_offset == 2U);
        }
    } else {
        CHECK(TextIs(f, strcmp(hook, "draft") == 0 || strcmp(name, "save-session") == 0 ||
            strcmp(name, "save-all") == 0 ? LATE : draft) == 0);
        if (strcmp(hook, "rename") == 0) CHECK(umi_path_equal(after.path, "renamed.txt"));
        else CHECK(after.saved_revision == before.saved_revision && strcmp(after.path, before.path) == 0);
        if (strcmp(hook, "store") == 0) {
            char *copy = NULL; size_t length = 0U;
            CHECK(umi_document_store_copy_text(f->store, f->id, &copy, &length) == UMI_STATUS_OK);
            CHECK(length == strlen(LATE) && strcmp(copy, LATE) == 0);
            umi_document_store_free_text(copy);
        }
        if (strcmp(hook, "external") == 0) CHECK(after.external_change);
    }
    /* Every failure releases the guard. A new destination lets the owner save
     * a retained draft without approving overwrite of the provider's old file. */
    if (failure || !success) {
        f->readStatus = UMI_STATUS_OK; f->writeStatus = UMI_STATUS_OK; f->hook = NULL;
        char root[UMI_PATH_CAPACITY], retry[UMI_PATH_CAPACITY];
        CHECK(umi_fs_temp_directory(root, sizeof(root)) == UMI_STATUS_OK);
        CHECK(umi_fs_join(retry, sizeof(retry), root, "umicom-snapshot-retained.txt") == UMI_STATUS_OK);
        CHECK(UmiDocumentCoordinatorSaveAs(f->documents, f->id, retry) == UMI_STATUS_OK);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    Fixture *f = calloc(1U, sizeof(*f));
    CHECK(f != NULL);
    int result = Run(f, argv[1]);
    UmiDocumentSaveSessionDestroy(f->session);
    umi_document_coordinator_destroy(f->documents);
    umi_document_store_destroy(f->store);
    umi_ui_workbench_destroy(f->workbench);
    umi_command_registry_destroy(f->commands);
    free(f->bytes); free(f);
    return result;
}
