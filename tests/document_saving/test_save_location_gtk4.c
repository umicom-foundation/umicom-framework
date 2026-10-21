/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document_saving/test_save_location_gtk4.c
 * PURPOSE: Verify captured Save All chooser location and explicit user destinations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/document/local_provider.h"
#include "umicom/platform/filesystem.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Substitute only the operating-system chooser boundary. Include the production
 * sequence so its actual GTK properties, lifetime checks and document writes run.
 * The separate native link check resolves the exported library implementation. */
static GTask *pendingTask;
static GAsyncReadyCallback pendingCallback;
static gpointer pendingContext;
static GFile *observedFolder;
static char *observedName;
static unsigned starts;
static void CaptureSave(GtkFileDialog *dialog, GtkWindow *parent, GCancellable *cancel,
    GAsyncReadyCallback callback, gpointer context)
{
    (void)parent;
    ++starts;
    GFile *folder = gtk_file_dialog_get_initial_folder(dialog);
    g_set_object(&observedFolder, folder);
    g_free(observedName);
    observedName = g_strdup(gtk_file_dialog_get_initial_name(dialog));
    pendingTask = g_task_new(dialog, cancel, NULL, NULL);
    g_task_set_check_cancellable(pendingTask, FALSE);
    pendingCallback = callback;
    pendingContext = context;
}
static GFile *FinishSave(GtkFileDialog *dialog, GAsyncResult *result, GError **error)
{
    (void)dialog;
    return g_task_propagate_pointer(G_TASK(result), error);
}
#define gtk_file_dialog_save CaptureSave
#define gtk_file_dialog_save_finish FinishSave
#include "../../adapters/gtk4/document_saving_gtk4.c"
#undef gtk_file_dialog_save
#undef gtk_file_dialog_save_finish

static void Choose(GFile *file)
{
    GTask *task = pendingTask;
    GAsyncReadyCallback callback = pendingCallback;
    gpointer context = pendingContext;
    pendingTask = NULL;
    pendingCallback = NULL;
    pendingContext = NULL;
    if (file != NULL) g_task_return_pointer(task, file, g_object_unref);
    else g_task_return_new_error(task, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED, "Dismissed");
    callback(g_task_get_source_object(task), G_ASYNC_RESULT(task), context);
    g_object_unref(task);
}
typedef struct LocationResult {
    unsigned count;
    UmiDocumentSaveProgress progress;
} LocationResult;
static void LocationCompleted(void *context, const UmiDocumentSaveProgress *progress)
{
    LocationResult *result = context;
    ++result->count;
    result->progress = *progress;
}
static void DrainPending(void)
{
    for (unsigned index = 0U; index < 64U && g_main_context_pending(NULL); ++index)
        (void)g_main_context_iteration(NULL, FALSE);
}
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)

int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    const char *scenario = argv[1];
    if (strcmp(scenario, "captured") != 0 && strcmp(scenario, "outside") != 0 &&
        strcmp(scenario, "default") != 0 && strcmp(scenario, "cancel") != 0 &&
        strcmp(scenario, "detach") != 0 && strcmp(scenario, "invalid") != 0) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0, madeRoot = 0, changedDirectory = 0;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkApplication *application = NULL;
    char *root = NULL, *folder = NULL, *destination = NULL, *hint = NULL, *disk = NULL;
    char *original = g_get_current_dir();
    char viewId[UMI_UI_ID_CAPACITY];
    GFile *expectedFolder = NULL;
    LocationResult result = {0};
    root = g_build_filename(argv[2], "save-location-XXXXXX", NULL);
    REQUIRE(g_mkdtemp(root) != NULL);
    madeRoot = 1;
    folder = g_build_filename(root, "chosen project caf\xc3\xa9", NULL);
    REQUIRE(umi_fs_make_directories(folder) == UMI_STATUS_OK);
    destination = g_build_filename(strcmp(scenario, "outside") == 0 ? root : folder, "chosen.c", NULL);
    hint = g_strdup(folder);
    expectedFolder = g_file_new_for_path(folder);
    application = gtk_application_new("org.umicom.save-location", G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application), NULL, NULL));
    REQUIRE(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    REQUIRE(umi_ui_workbench_create("save-location", commands, &workbench) == UMI_STATUS_OK);
    REQUIRE(umi_ui_application_shell_create("org.umicom.save-location", "Source", workbench, &shell) == UMI_STATUS_OK);
    REQUIRE(umi_document_store_create(&store) == UMI_STATUS_OK);
    UmiDocumentProvider provider = umi_document_local_provider();
    REQUIRE(umi_document_coordinator_create(store, workbench, &provider, &documents) == UMI_STATUS_OK);
    REQUIRE(umi_document_coordinator_new(documents, "untitled.c", viewId, sizeof viewId) == UMI_STATUS_OK);
    UmiUiDocumentViewSnapshot view;
    REQUIRE(umi_ui_document_view_model_find(umi_ui_workbench_documents(workbench), viewId, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    REQUIRE(UmiUiDocumentViewModelUpsertText(umi_ui_workbench_documents(workbench), &view,
        "chosen draft\n", 13U) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, documents, NULL, NULL) == UMI_STATUS_OK);
    if (strcmp(scenario, "invalid") == 0) {
        const char *invalid[] = {"relative", "file:///project", "\xff"
#ifdef _WIN32
            , "/project", "C:project"
#endif
        };
        for (size_t index = 0U; index < sizeof invalid / sizeof invalid[0]; ++index) {
            REQUIRE(UmiGtk4AdapterDocumentSaveAllAt(adapter, invalid[index], LocationCompleted, &result) == UMI_STATUS_INVALID_ARGUMENT);
            REQUIRE(!UmiGtk4AdapterDocumentSaveAllBusy(adapter));
        }
        REQUIRE(result.count == 0U && starts == 0U && !umi_fs_exists(destination));
        UmiDocumentWorkingCopySnapshot active;
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK && active.dirty);
        goto cleanup;
    }
    if (strcmp(scenario, "default") == 0)
        REQUIRE(UmiGtk4AdapterDocumentSaveAll(adapter, LocationCompleted, &result) == UMI_STATUS_OK);
    else
        REQUIRE(UmiGtk4AdapterDocumentSaveAllAt(adapter, hint, LocationCompleted, &result) == UMI_STATUS_OK);
    /* The borrowed hint and current directory may change before the first idle. */
    memset(hint, 'x', strlen(hint));
    REQUIRE(g_chdir(root) == 0);
    changedDirectory = 1;
    gint64 deadline = g_get_monotonic_time() + 3000000;
    while (pendingTask == NULL && g_get_monotonic_time() < deadline) { DrainPending(); g_usleep(1000U); }
    REQUIRE(pendingTask != NULL && starts == 1U);
    REQUIRE(strcmp(observedName, "untitled.c") == 0);
    if (strcmp(scenario, "default") == 0) REQUIRE(observedFolder == NULL);
    else REQUIRE(observedFolder != NULL && g_file_equal(observedFolder, expectedFolder));
    REQUIRE(!umi_fs_exists(destination));
    int cancelled = strcmp(scenario, "cancel") == 0;
    int detached = strcmp(scenario, "detach") == 0;
    if (detached) REQUIRE(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    Choose(cancelled ? NULL : g_file_new_for_path(destination));
    deadline = g_get_monotonic_time() + 3000000;
    while (!detached && result.count == 0U && g_get_monotonic_time() < deadline) { DrainPending(); g_usleep(1000U); }
    REQUIRE(!UmiGtk4AdapterDocumentSaveAllBusy(adapter));
    if (detached || cancelled) {
        REQUIRE(!umi_fs_exists(destination));
        REQUIRE(result.count == (detached ? 0U : 1U));
        if (cancelled) REQUIRE(result.progress.phase == UMI_DOCUMENT_SAVE_CANCELLED);
        UmiDocumentWorkingCopySnapshot active;
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK && active.dirty);
    } else {
        REQUIRE(result.count == 1U && result.progress.phase == UMI_DOCUMENT_SAVE_COMPLETE && result.progress.saved == 1U);
        REQUIRE(g_file_get_contents(destination, &disk, NULL, NULL));
        REQUIRE(strcmp(disk, "chosen draft\n") == 0);
        UmiDocumentWorkingCopySnapshot active;
        REQUIRE(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK && !active.dirty && active.has_path);
    }
cleanup:
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (pendingTask != NULL) Choose(NULL);
    if (adapter != NULL) umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    g_clear_object(&observedFolder);
    g_clear_object(&expectedFolder);
    g_clear_pointer(&observedName, g_free);
    if (changedDirectory) (void)g_chdir(original);
    if (!failed && madeRoot) (void)umi_fs_remove_tree(root);
    g_free(root); g_free(folder); g_free(destination); g_free(hint); g_free(disk); g_free(original);
    return failed;
}
