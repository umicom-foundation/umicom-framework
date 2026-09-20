/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_save_all_gtk4.c
 * PURPOSE: Check native Save All scheduling, cancellation and binding lifetime.
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
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed=1; goto cleanup; } } while (0)
typedef struct Result {
    unsigned count;
    UmiDocumentSaveProgress progress;
    UmiGtk4Adapter *detach;
    unsigned editCount;
    UmiStatus editStatus;
} Result;
static void Completed(void *context, const UmiDocumentSaveProgress *progress)
{
    Result *result = context;
    ++result->count; result->progress = *progress;
    if (result->detach != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(result->detach, NULL, NULL, NULL);
}
static void Edited(void *context, UmiStatus status)
{
    Result *result = context;
    ++result->editCount;
    result->editStatus = status;
}
static void Pump(void)
{
    for (unsigned n = 0U; n < 32U && g_main_context_pending(NULL); ++n)
        (void)g_main_context_iteration(NULL, FALSE);
}
static int Await(Result *result)
{
    gint64 end = g_get_monotonic_time() + 3000000;
    while (result->count == 0U && g_get_monotonic_time() < end) { Pump(); g_usleep(1000); }
    return result->count == 1U;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    /* Retained original selector: the expanded fixture also accepts progress,
     * blocked-paste and repeated-cancellation cases without dropping the old ones. */
    // if (strcmp(argv[1],"empty") != 0 && strcmp(argv[1],"named") != 0 &&
    // strcmp(argv[1],"cancel") != 0 && strcmp(argv[1],"unbind") != 0 &&
    // strcmp(argv[1],"complete-unbind") != 0) return 2;
    if (strcmp(argv[1],"empty") != 0 && strcmp(argv[1],"named") != 0 &&
        strcmp(argv[1],"cancel") != 0 && strcmp(argv[1],"unbind") != 0 &&
        strcmp(argv[1],"complete-unbind") != 0 && strcmp(argv[1],"progress") != 0 &&
        strcmp(argv[1],"paste-busy") != 0 && strcmp(argv[1],"repeat-cancel") != 0) return 2;
    if (!gtk_init_check()) return 77;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkApplication *application = NULL;
    char temp[UMI_PATH_CAPACITY], root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY], leaf[128];
    char viewId[UMI_UI_ID_CAPACITY];
    char *disk = NULL; size_t diskBytes = 0U;
    int failed = 0, madeRoot = 0;
    Result result = {0};
    application = gtk_application_new("org.umicom.saveall.test", G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application), NULL, NULL));
    REQUIRE(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("umicom.saveall.test", commands, &workbench) == UMI_STATUS_OK);
    REQUIRE(umi_ui_application_shell_create("org.umicom.saveall.test", "Umicom Notes", workbench, &shell) == UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store) == UMI_STATUS_OK);
    UmiDocumentProvider provider = umi_document_local_provider();
    REQUIRE(umi_document_coordinator_create(store, workbench, &provider, &documents) == UMI_STATUS_OK);
    REQUIRE(umi_fs_temp_directory(temp, sizeof(temp)) == UMI_STATUS_OK);
    REQUIRE(snprintf(leaf, sizeof(leaf), "umicom-save-native-%ld-%s", (long)PROCESS_ID(), argv[1]) > 0);
    REQUIRE(umi_fs_join(root, sizeof(root), temp, leaf) == UMI_STATUS_OK);
    REQUIRE(!umi_fs_exists(root));
    REQUIRE(umi_fs_make_directories(root) == UMI_STATUS_OK); madeRoot = 1;
    REQUIRE(umi_fs_join(path, sizeof(path), root, "notes.c") == UMI_STATUS_OK);
    REQUIRE(umi_fs_write_text(path, "original\n") == UMI_STATUS_OK);
    if (strcmp(argv[1], "empty") != 0) {
        REQUIRE(umi_document_coordinator_open(documents, path, viewId, sizeof(viewId)) == UMI_STATUS_OK);
        UmiUiDocumentViewSnapshot view;
        REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), viewId, &view) == UMI_STATUS_OK);
        view.dirty = 1;
        REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench), &view, "saved Notes\n", 12U) == UMI_STATUS_OK);
    }
    REQUIRE(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    /* Former test binding retained for reference:
     * REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, documents, NULL, NULL) == UMI_STATUS_OK);
     * The replacement observes rejected clipboard commands while preserving
     * every original save-session and teardown check. */
    REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Edited, &result) == UMI_STATUS_OK);
    UmiDocumentSaveProgress observed = {0};
    observed.total = 456U;
    REQUIRE(UmiGtk4AdapterDocumentSaveAllProgress(adapter, &observed) == UMI_STATUS_NOT_FOUND);
    REQUIRE(observed.total == 456U);
    REQUIRE(UmiGtk4AdapterDocumentSaveAllProgress(NULL, &observed) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(UmiGtk4AdapterDocumentSaveAllProgress(adapter, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    if (strcmp(argv[1], "complete-unbind") == 0) result.detach = adapter;
    REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter, Completed, &result) == UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterDocumentSaveAllBusy(adapter));
    REQUIRE(UmiGtk4AdapterDocumentSaveAllProgress(adapter, &observed) == UMI_STATUS_OK);
    size_t expectedTotal = strcmp(argv[1], "empty") == 0 ? 0U : 1U;
    REQUIRE(observed.total == expectedTotal && observed.saved == 0U);
    observed.total = 999U;
    REQUIRE(UmiGtk4AdapterDocumentSaveAllProgress(adapter, &observed) == UMI_STATUS_OK);
    REQUIRE(observed.total == expectedTotal);
    if (strcmp(argv[1], "paste-busy") == 0) {
        REQUIRE(!UmiGtk4AdapterDocumentCommandEnabled(adapter, "edit.paste"));
        REQUIRE(UmiGtk4AdapterDocumentCommand(adapter, "edit.paste") == UMI_STATUS_BUSY);
        REQUIRE(result.editCount == 1U && result.editStatus == UMI_STATUS_BUSY);
        REQUIRE(UmiGtk4AdapterDocumentSaveAllBusy(adapter));
    }
    REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter, Completed, &result) == UMI_STATUS_BUSY);
    if (strcmp(argv[1], "unbind") == 0) {
        REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        for (unsigned i=0U; i<50U; ++i) { Pump(); g_usleep(1000); }
        REQUIRE(result.count == 0U);
    } else {
        /* Previously only the cancel case requested cancellation. The new
         * paste-busy and repeat-cancel cases follow the same unchanged operation. */
        // if (strcmp(argv[1], "cancel") == 0)
        int cancelCase = strcmp(argv[1], "cancel") == 0 || strcmp(argv[1], "paste-busy") == 0 ||
            strcmp(argv[1], "repeat-cancel") == 0;
        if (strcmp(argv[1], "repeat-cancel") == 0)
            REQUIRE(UmiGtk4AdapterCancelDocumentSaveAll(adapter) == UMI_STATUS_OK);
        if (cancelCase)
            REQUIRE(UmiGtk4AdapterCancelDocumentSaveAll(adapter) == UMI_STATUS_OK);
        REQUIRE(Await(&result));
        // REQUIRE(result.progress.phase == (strcmp(argv[1],"cancel")==0 ? UMI_DOCUMENT_SAVE_CANCELLED : UMI_DOCUMENT_SAVE_COMPLETE));
        REQUIRE(result.progress.phase == (cancelCase ? UMI_DOCUMENT_SAVE_CANCELLED : UMI_DOCUMENT_SAVE_COMPLETE));
    }
    REQUIRE(!UmiGtk4AdapterDocumentSaveAllBusy(adapter));
    observed.total = 456U;
    REQUIRE(UmiGtk4AdapterDocumentSaveAllProgress(adapter, &observed) == UMI_STATUS_NOT_FOUND);
    REQUIRE(observed.total == 456U);
    REQUIRE(umi_fs_read_text(path, &disk, &diskBytes) == UMI_STATUS_OK);
    /* The progress-observation scenario also lets the original save complete.
     * Other original scenarios keep their prior saved-content expectations. */
    // const char *expected = (strcmp(argv[1],"named")==0 || strcmp(argv[1],"complete-unbind")==0) ? "saved Notes\n" : "original\n";
    const char *expected = (strcmp(argv[1],"named")==0 || strcmp(argv[1],"complete-unbind")==0 || strcmp(argv[1],"progress")==0) ? "saved Notes\n" : "original\n";
    REQUIRE(diskBytes == strlen(expected) && memcmp(disk, expected, diskBytes) == 0);
cleanup:
    free(disk);
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (adapter != NULL) umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    if (madeRoot) (void)umi_fs_remove_tree(root);
    return failed;
}
