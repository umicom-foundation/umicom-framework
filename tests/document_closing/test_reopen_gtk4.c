/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_closing/test_reopen_gtk4.c
 * PURPOSE: Verify native reopening, busy decisions and completion lifetimes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/document/local_provider.h"
#include "umicom/platform/filesystem.h"
#include <gtk/gtk.h>
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
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)

typedef struct Result {
    unsigned count;
    UmiStatus status;
    UmiGtk4Adapter *detach;
} Result;

/* Detaching in completion verifies that the shared adapter does not access
 * its borrowed host context after delivering the result. */
static void Completed(void *context, UmiStatus status)
{
    Result *result = context;
    ++result->count; result->status = status;
    if (result->detach != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(result->detach, NULL, NULL, NULL);
}

typedef struct ReadContext {
    UmiDocumentProvider local;
    UmiGtk4Adapter *detach;
} ReadContext;

/* A provider may detach the UI while keeping the document service alive.
 * The coordinator finishes normally, but the detached UI gets no callback. */
static UmiStatus Read(void *context, const char *path, unsigned char **bytes, size_t *length)
{
    ReadContext *reader = context;
    if (reader->detach != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(reader->detach, NULL, NULL, NULL);
    return reader->local.read(reader->local.instance, path, bytes, length);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    if (strcmp(name, "link") == 0) {
        return UmiGtk4AdapterReopenDocument(NULL) == UMI_STATUS_INVALID_ARGUMENT &&
            UmiGtk4AdapterForgetClosedDocument(NULL) == UMI_STATUS_INVALID_ARGUMENT &&
            !UmiGtk4AdapterReopenDocumentEnabled(NULL) ? 0 : 1;
    }
    const char *cases[] = {"current-disk", "already-open", "missing", "forget", "save-busy",
        "close-busy", "complete-unbind", "read-detach", "unbound"};
    int known = 0;
    for (size_t index = 0U; index < sizeof(cases) / sizeof(cases[0]); ++index)
        if (strcmp(name, cases[index]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkApplication *application = NULL;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
    char moved[UMI_PATH_CAPACITY], leaf[128], view_id[UMI_UI_ID_CAPACITY];
    char *text = NULL; size_t text_bytes = 0U;
    int failed = 0, made_root = 0;
    Result result = {0};
    ReadContext reader = {0};
    reader.local = umi_document_local_provider();
    UmiDocumentProvider provider = reader.local;
    /* The standard local callbacks ignore their instance; only Read uses the
     * explicit fixture context to exercise binding detachment. */
    provider.instance = &reader; provider.read = Read;
    application = gtk_application_new("org.umicom.reopen.test", G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application), NULL, NULL));
    REQUIRE(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("umicom.reopen.test", commands, &workbench) == UMI_STATUS_OK);
    REQUIRE(umi_ui_application_shell_create("org.umicom.reopen.test", "Umicom Notes", workbench, &shell) == UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store) == UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_create(store, workbench, &provider, &documents) == UMI_STATUS_OK);
    REQUIRE(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    (void)snprintf(leaf, sizeof(leaf), "umicom-reopen-native-%ld-%s", (long)PROCESS_ID(), name);
    REQUIRE(umi_fs_join(root, sizeof(root), temp, leaf) == UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));
    REQUIRE(umi_fs_make_directories(root) == UMI_STATUS_OK); made_root = 1;
    REQUIRE(umi_fs_join(path, sizeof(path), root, "notes.c") == UMI_STATUS_OK);
    REQUIRE(umi_fs_join(moved, sizeof(moved), root, "moved.c") == UMI_STATUS_OK);
    REQUIRE(umi_fs_write_text(path, "saved text\n") == UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_open(documents, path, view_id, sizeof(view_id)) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Completed, &result) == UMI_STATUS_OK);
    REQUIRE(!UmiGtk4AdapterReopenDocumentEnabled(adapter));
    REQUIRE(UmiGtk4AdapterReopenDocument(adapter) == UMI_STATUS_NOT_FOUND);
    REQUIRE(result.count == 1U && result.status == UMI_STATUS_NOT_FOUND);
    REQUIRE(UmiGtk4AdapterRequestDocumentClose(adapter, view_id) == UMI_STATUS_OK);
    REQUIRE(result.count == 2U && result.status == UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_count(documents) == 0U && UmiGtk4AdapterReopenDocumentEnabled(adapter));
    result.count = 0U;

    if (strcmp(name, "save-busy") == 0 || strcmp(name, "close-busy") == 0) {
        REQUIRE(umi_document_coordinator_new(documents, "draft.c", NULL, 0U) == UMI_STATUS_OK);
        if (strcmp(name, "save-busy") == 0)
            REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter, NULL, NULL) == UMI_STATUS_OK);
        else REQUIRE(UmiGtk4AdapterRequestDocumentClose(adapter, NULL) == UMI_STATUS_OK);
        REQUIRE(!UmiGtk4AdapterReopenDocumentEnabled(adapter));
        REQUIRE(UmiGtk4AdapterReopenDocument(adapter) == UMI_STATUS_BUSY);
        REQUIRE(UmiGtk4AdapterForgetClosedDocument(adapter) == UMI_STATUS_BUSY);
        REQUIRE(result.count == 2U && result.status == UMI_STATUS_BUSY);
        UmiDocumentReopenSnapshot history;
        REQUIRE(UmiDocumentCoordinatorReopenSnapshot(documents, &history) == UMI_STATUS_OK && history.count == 1U);
        goto cleanup;
    }
    if (strcmp(name, "unbound") == 0) {
        REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        REQUIRE(!UmiGtk4AdapterReopenDocumentEnabled(adapter));
        REQUIRE(UmiGtk4AdapterReopenDocument(adapter) == UMI_STATUS_UNAVAILABLE);
        REQUIRE(UmiGtk4AdapterForgetClosedDocument(adapter) == UMI_STATUS_UNAVAILABLE);
        REQUIRE(result.count == 0U && umi_document_coordinator_count(documents) == 0U);
        goto cleanup;
    }
    if (strcmp(name, "forget") == 0) {
        REQUIRE(UmiGtk4AdapterForgetClosedDocument(adapter) == UMI_STATUS_OK);
        REQUIRE(result.count == 1U && result.status == UMI_STATUS_OK);
        REQUIRE(!UmiGtk4AdapterReopenDocumentEnabled(adapter) && umi_fs_exists(path));
        REQUIRE(umi_document_coordinator_count(documents) == 0U);
        goto cleanup;
    }
    if (strcmp(name, "missing") == 0) {
        REQUIRE(umi_fs_rename(path, moved) == UMI_STATUS_OK);
        REQUIRE(UmiGtk4AdapterReopenDocument(adapter) != UMI_STATUS_OK);
        REQUIRE(result.count == 1U && result.status != UMI_STATUS_OK);
        REQUIRE(UmiGtk4AdapterReopenDocumentEnabled(adapter));
        REQUIRE(umi_document_coordinator_count(documents) == 0U);
        REQUIRE(umi_fs_rename(moved, path) == UMI_STATUS_OK);
        result.count = 0U;
    }
    UmiDocumentId existing = 0U;
    if (strcmp(name, "already-open") == 0) {
        UmiUiDocumentViewSnapshot view;
        UmiDocumentWorkingCopySnapshot snapshot;
        REQUIRE(umi_document_coordinator_open(documents, path, view_id, sizeof(view_id)) == UMI_STATUS_OK);
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &snapshot) == UMI_STATUS_OK);
        existing = snapshot.document_id;
        REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), view_id, &view) == UMI_STATUS_OK);
        view.dirty = 1;
        REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench), &view, "keep my draft\n", 14U) == UMI_STATUS_OK);
    }
    if (strcmp(name, "current-disk") == 0)
        REQUIRE(umi_fs_write_text(path, "new disk text\n") == UMI_STATUS_OK);
    if (strcmp(name, "complete-unbind") == 0) result.detach = adapter;
    if (strcmp(name, "read-detach") == 0) reader.detach = adapter;
    REQUIRE(UmiGtk4AdapterReopenDocument(adapter) == UMI_STATUS_OK);
    REQUIRE(result.count == (strcmp(name, "read-detach") == 0 ? 0U : 1U));
    if (result.count != 0U) REQUIRE(result.status == UMI_STATUS_OK);
    REQUIRE(!UmiGtk4AdapterReopenDocumentEnabled(adapter));
    REQUIRE(umi_document_coordinator_count(documents) == 1U);
    UmiDocumentWorkingCopySnapshot snapshot;
    REQUIRE(umi_document_coordinator_active_snapshot(documents, &snapshot) == UMI_STATUS_OK);
    if (existing != 0U) REQUIRE(snapshot.document_id == existing && snapshot.dirty);
    REQUIRE(UmiUiDocumentViewModelCopyText(umi_ui_workbench_documents(workbench), snapshot.view_id, &text, &text_bytes) == UMI_STATUS_OK);
    const char *expected = strcmp(name, "already-open") == 0 ? "keep my draft\n" :
        strcmp(name, "current-disk") == 0 ? "new disk text\n" : "saved text\n";
    REQUIRE(text_bytes == strlen(expected) && memcmp(text, expected, text_bytes) == 0);
cleanup:
    UmiUiDocumentViewModelFreeText(text);
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (adapter != NULL) umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents); umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell); umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands); g_clear_object(&application);
    if (made_root) (void)umi_fs_remove_tree(root);
    return failed;
}
