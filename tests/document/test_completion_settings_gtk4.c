/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_completion_settings_gtk4.c
 * PURPOSE: Check saved language-server choices through real controls without launching a server.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/document_commands.h"
#include "umicom/ui/gtk4/interaction_recording.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(c)                                                                                             \
    do                                                                                                       \
    {                                                                                                        \
        if (!(c))                                                                                            \
        {                                                                                                    \
            fprintf(stderr, "line %d: %s\n", __LINE__, #c);                                                  \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
typedef struct Completion
{
    unsigned count;
    UmiStatus status;
    UmiGtk4Adapter *detach;
} Completion;
static void Complete(void *data, UmiStatus status)
{
    Completion *completion = data;
    ++completion->count;
    completion->status = status;
    if (completion->detach != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(completion->detach, NULL, NULL, NULL);
}
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *actual = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (actual != NULL && strcmp(actual, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static GtkWindow *Review(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (Find(GTK_WIDGET(window), "document.completion.review") != NULL)
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static void UnbindOnCreate(GListModel *model, guint position, guint removed, guint added, gpointer data)
{
    (void)model;
    (void)position;
    (void)removed;
    Completion *result = data;
    if (added != 0U && result->detach != NULL)
    {
        UmiGtk4Adapter *adapter = result->detach;
        result->detach = NULL;
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    }
}

static int Wait(GtkWidget *window)
{
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (g_object_get_data(G_OBJECT(window), "umicom-completion-settings-pending") != NULL &&
           g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return g_object_get_data(G_OBJECT(window), "umicom-completion-settings-pending") == NULL;
}

#include "umicom/language_runtime/server_preferences.h"
#include "umicom/document/source_request.h"
#include "umicom/platform/local_replace.h"
typedef struct Interruption
{
    GtkWidget *arguments;
    int once;
} Interruption;
static void EditDuringLoad(GtkEditable *program, gpointer data)
{
    (void)program;
    Interruption *interrupt = data;
    if (!interrupt->once)
    {
        interrupt->once = 1;
        gtk_editable_set_text(GTK_EDITABLE(interrupt->arguments), "--user-choice");
    }
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {"save-load",       "missing",       "invalid-file",     "language-mismatch",
                           "invalid-save",    "changed-back",  "edit-during-load", "load-publication",
                           "save-close",      "worker-unbind", "retained",         "private",
                           "transient",       "invalid-root",  "creation-unbind",  "busy",
                           "save-newer-draft"};
    int known = 0;
    for (size_t i = 0U; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    GtkApplication *application = NULL;
    GtkWindow *dialog = NULL;
    GtkWidget *retained = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    Completion result = {0};
    UmiDocumentWorkingCopySnapshot active;
    UmiUiDocumentViewSnapshot view;
    char *text = NULL, *directory = g_get_current_dir(),
         *root = g_dir_make_tmp("umicom-language-settings-XXXXXX", NULL);
    size_t length = 0U;
    CHECK(root != NULL);
    application = gtk_application_new("org.umicom.settings.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("completion.settings", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.settings.test", "Settings", workbench, &shell) ==
          UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "main.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    view.cursor_offset = 0U;
    view.selection_length = 2U;
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "pu", 2U) == UMI_STATUS_OK);
    if (strcmp(mode, "invalid-root") == 0)
    {
        CHECK(UmiGtk4AdapterReviewCompletionWithSettings(adapter, directory, "Studio", "relative") !=
              UMI_STATUS_OK);
        CHECK(!UmiGtk4AdapterCompletionReviewBusy(adapter));
        goto inspect;
    }
    if (strcmp(mode, "creation-unbind") == 0)
    {
        result.detach = adapter;
        gulong observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                           G_CALLBACK(UnbindOnCreate), &result);
        UmiStatus status = UmiGtk4AdapterReviewCompletionWithSettings(adapter, directory, "Studio", root);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
        CHECK(status == UMI_STATUS_CANCELLED && !UmiGtk4AdapterCompletionReviewBusy(adapter));
        goto inspect;
    }
    CHECK((strcmp(mode, "transient") == 0 ? UmiGtk4AdapterReviewCompletion(adapter, directory)
                                          : UmiGtk4AdapterReviewCompletionWithSettings(
                                                adapter, directory, "Studio", root)) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL);
    GtkWidget *save = Find(GTK_WIDGET(dialog), "document.completion.settings.save");
    GtkWidget *load = Find(GTK_WIDGET(dialog), "document.completion.settings.load");
    if (strcmp(mode, "transient") == 0)
    {
        CHECK(save == NULL && load == NULL);
        goto inspect;
    }
    GtkWidget *program = Find(GTK_WIDGET(dialog), "document.completion.program");
    GtkWidget *arguments = Find(GTK_WIDGET(dialog), "document.completion.arguments");
    GtkWidget *request = Find(GTK_WIDGET(dialog), "document.completion.request");
    CHECK(save != NULL && load != NULL && program != NULL && arguments != NULL && request != NULL);
    retained = g_object_ref(load);
    if (strcmp(mode, "private") == 0)
    {
        CHECK(UmiGtk4RecordingIsPrivate(program) && UmiGtk4RecordingIsPrivate(arguments));
        goto inspect;
    }
    if (strcmp(mode, "retained") == 0)
    {
        gtk_window_destroy(dialog);
        g_signal_emit_by_name(retained, "clicked");
        CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-completion-settings-pending") == NULL);
        goto inspect;
    }
#ifdef _WIN32
    const char *executable = "C:/not-installed/caf\xc3\xa9/clangd.exe";
#else
    const char *executable = "/not-installed/caf\xc3\xa9/clangd";
#endif
    gtk_editable_set_text(GTK_EDITABLE(program), executable);
    gtk_editable_set_text(GTK_EDITABLE(arguments), "--background-index");
    if (strcmp(mode, "missing") == 0)
    {
        g_signal_emit_by_name(load, "clicked");
        CHECK(Wait(GTK_WIDGET(dialog)));
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), executable) == 0);
        goto inspect;
    }
    if (strcmp(mode, "invalid-save") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(program), "relative");
        g_signal_emit_by_name(save, "clicked");
        CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-completion-settings-pending") == NULL);
        goto inspect;
    }
    g_signal_emit_by_name(save, "clicked");
    if (strcmp(mode, "save-close") == 0)
        gtk_window_destroy(dialog);
    if (strcmp(mode, "worker-unbind") == 0)
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    if (strcmp(mode, "save-newer-draft") == 0)
        gtk_editable_set_text(GTK_EDITABLE(arguments), "--newer");
    CHECK(Wait(GTK_WIDGET(dialog)));
    char path[UMI_PATH_CAPACITY];
    UmiDocumentSourceRequest *captured = NULL;
    UmiDocumentSourceRequestSummary summary;
    CHECK(UmiDocumentSourceRequestCreate(documents, active.document_id, &captured) == UMI_STATUS_OK);
    UmiStatus inspected = UmiDocumentSourceRequestInspect(captured, &summary);
    UmiDocumentSourceRequestDestroy(captured);
    CHECK(inspected == UMI_STATUS_OK);
    CHECK(UmiLanguageServerPreferencesPath("Studio", root, summary.language_id, path, sizeof(path)) ==
          UMI_STATUS_OK);
    UmiLanguageServerPreferences saved = {0};
    CHECK(UmiLanguageServerPreferencesLoad(path, summary.language_id, &saved) == UMI_STATUS_OK);
    CHECK(strcmp(saved.executable, executable) == 0 && strcmp(saved.arguments, "--background-index") == 0);
    if (strcmp(mode, "save-close") == 0 || strcmp(mode, "worker-unbind") == 0)
        goto inspect;
    if (strcmp(mode, "save-newer-draft") == 0)
    {
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(arguments)), "--newer") == 0);
        goto inspect;
    }
    if (strcmp(mode, "invalid-file") == 0)
        CHECK(UmiLocalFileReplace(path, "{}", 2U) == UMI_STATUS_OK);
    if (strcmp(mode, "language-mismatch") == 0)
    {
        strcpy(saved.language_id, "another-language");
        CHECK(UmiLanguageServerPreferencesSave(path, &saved) == UMI_STATUS_OK);
    }
    gtk_editable_set_text(GTK_EDITABLE(program), "");
    gtk_editable_set_text(GTK_EDITABLE(arguments), "--draft");
    Interruption interrupt = {arguments, 0};
    if (strcmp(mode, "load-publication") == 0)
        g_signal_connect(program, "changed", G_CALLBACK(EditDuringLoad), &interrupt);
    g_signal_emit_by_name(load, "clicked");
    if (strcmp(mode, "edit-during-load") == 0 || strcmp(mode, "changed-back") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(arguments), "--intervening");
        if (strcmp(mode, "changed-back") == 0)
            gtk_editable_set_text(GTK_EDITABLE(arguments), "--draft");
    }
    if (strcmp(mode, "busy") == 0)
    {
        g_signal_emit_by_name(load, "clicked");
        g_signal_emit_by_name(save, "clicked");
        g_signal_emit_by_name(request, "clicked");
        CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-completion-pending") == NULL);
    }
    CHECK(Wait(GTK_WIDGET(dialog)));
    if (strcmp(mode, "load-publication") == 0)
    {
        g_signal_handlers_disconnect_by_data(program, &interrupt);
        CHECK(interrupt.once && strcmp(gtk_editable_get_text(GTK_EDITABLE(arguments)), "--user-choice") == 0);
    }
    else if (strcmp(mode, "invalid-file") == 0 || strcmp(mode, "language-mismatch") == 0 ||
             strcmp(mode, "changed-back") == 0 || strcmp(mode, "edit-during-load") == 0)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "") == 0);
    else
    {
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), executable) == 0);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(arguments)), "--background-index") == 0);
    }
    CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-completion-pending") == NULL);
inspect:
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &text, &length) == UMI_STATUS_OK);
    CHECK(length == 2U && strcmp(text, "pu") == 0);
cleanup:
    UmiUiDocumentViewModelFreeText(text);
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
    {
        gtk_window_destroy(dialog);
        (void)Wait(GTK_WIDGET(dialog));
    }
    g_clear_object(&dialog);
    g_clear_object(&retained);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    g_free(directory);
    g_free(root);
    return failed;
}
