/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_formatting_review_gtk4.c
 * PURPOSE: Check native formatting review, explicit approval and worker lifetime through actual GTK controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/automation.h"
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
/* The rendered-only search missed controls in collapsed review panels. The Framework logical-tree helper replaces it; retain the former traversal for review. */
#if 0
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
#endif
/* Inspect logical ownership as well as rendered children. Finding a control
 * does not grant permission to edit it or make a collapsed panel visible. */
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(root, id);
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
    while (g_object_get_data(G_OBJECT(window), "umicom-completion-pending") != NULL &&
           g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return g_object_get_data(G_OBJECT(window), "umicom-completion-pending") == NULL;
}
static int ProgramMain(int argc, char **argv)
{
    if (argc != 3)
        return 2;
    const char *mode = argv[1];
    const char *cases[] = {
        "open",         "apply",          "undo",     "no-preview",   "no-approval",     "stale",
        "cancel",       "retained",       "unbind",   "parent-close", "creation-unbind", "worker-unbind",
        "input-change", "changed-back",   "private",  "busy",         "invalid-path",    "completion-unbind",
        "tabs",         "options-change", "multiline"};
    int known = 0;
    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
        if (strcmp(mode, cases[i]) == 0)
            known = 1;
    if (!known)
        return 2;
    if (!gtk_init_check())
        return 77;
    int failed = 0;
    GtkApplication *application = NULL;
    GtkWindow *dialog = NULL;
    GtkWidget *apply = NULL;
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    Completion result = {0};
    UmiDocumentWorkingCopySnapshot active;
    UmiUiDocumentViewSnapshot view;
    char *text = NULL, *directory = g_get_current_dir();
    size_t length = 0U;
    application = gtk_application_new("org.umicom.completion.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("completion.native", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.completion.test", "Completion", workbench, &shell) ==
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
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    if (strcmp(mode, "multiline") == 0)
    {
        view.selection_length = 3U;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "pu\n", 3U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "creation-unbind") == 0)
    {
        result.detach = adapter;
        gulong observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed",
                                           G_CALLBACK(UnbindOnCreate), &result);
        UmiStatus status = UmiGtk4AdapterReviewFormatting(adapter, directory);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
        CHECK(status == UMI_STATUS_CANCELLED && !UmiGtk4AdapterCompletionReviewBusy(adapter));
        CHECK(Review() == NULL);
        goto inspect;
    }
    CHECK(UmiGtk4AdapterReviewFormatting(adapter, directory) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL);
    GtkWidget *program = Find(GTK_WIDGET(dialog), "document.completion.program");
    GtkWidget *arguments = Find(GTK_WIDGET(dialog), "document.completion.arguments");
    GtkWidget *request = Find(GTK_WIDGET(dialog), "document.completion.request");
    GtkWidget *preview = Find(GTK_WIDGET(dialog), "document.completion.preview");
    GtkWidget *approve = Find(GTK_WIDGET(dialog), "document.completion.approve");
    GtkWidget *cancel = Find(GTK_WIDGET(dialog), "document.completion.cancel");
    GtkWidget *found_apply = Find(GTK_WIDGET(dialog), "document.completion.apply");
    CHECK(program != NULL && arguments != NULL && request != NULL && preview != NULL && approve != NULL &&
          cancel != NULL && found_apply != NULL);
    apply = g_object_ref(found_apply);
    CHECK(gtk_window_get_default_widget(dialog) == cancel && !gtk_widget_get_sensitive(apply));
    CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-completion-pending") == NULL);
    if (strcmp(mode, "open") == 0)
        goto inspect;
    if (strcmp(mode, "busy") == 0)
    {
        CHECK(UmiGtk4AdapterReviewFormatting(adapter, directory) == UMI_STATUS_BUSY);
        goto inspect;
    }
    if (strcmp(mode, "cancel") == 0)
    {
        g_signal_emit_by_name(cancel, "clicked");
        CHECK(result.status == UMI_STATUS_CANCELLED);
        goto inspect;
    }
    if (strcmp(mode, "retained") == 0 || strcmp(mode, "unbind") == 0 || strcmp(mode, "parent-close") == 0)
    {
        if (strcmp(mode, "retained") == 0)
            gtk_window_destroy(dialog);
        else if (strcmp(mode, "unbind") == 0)
            CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        else
            gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(adapter)));
        CHECK(!UmiGtk4AdapterCompletionReviewBusy(adapter));
        unsigned count = result.count;
        g_signal_emit_by_name(apply, "clicked");
        CHECK(result.count == count);
        goto inspect;
    }
    gtk_editable_set_text(GTK_EDITABLE(program),
                          strcmp(mode, "invalid-path") == 0 ? "relative-program" : argv[2]);
    gtk_editable_set_text(GTK_EDITABLE(arguments), "formatting-valid");
    if (strcmp(mode, "tabs") == 0)
    {
        GtkWidget *tab = Find(GTK_WIDGET(dialog), "document.formatting.tab-size");
        GtkWidget *spaces = Find(GTK_WIDGET(dialog), "document.formatting.insert-spaces");
        CHECK(tab != NULL && spaces != NULL);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(tab), 2.0);
        gtk_check_button_set_active(GTK_CHECK_BUTTON(spaces), FALSE);
        gtk_editable_set_text(GTK_EDITABLE(arguments), "formatting-tabs");
    }
    g_signal_emit_by_name(request, "clicked");
    if (strcmp(mode, "invalid-path") == 0)
    {
        CHECK(g_object_get_data(G_OBJECT(dialog), "umicom-completion-pending") == NULL &&
              !gtk_widget_get_sensitive(preview));
        goto inspect;
    }
    if (strcmp(mode, "worker-unbind") == 0)
    {
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        CHECK(Wait(GTK_WIDGET(dialog)) && result.count == 0U);
        g_signal_emit_by_name(apply, "clicked");
        CHECK(result.count == 0U);
        goto inspect;
    }
    if (strcmp(mode, "input-change") == 0 || strcmp(mode, "changed-back") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(arguments), "formatting-error");
        if (strcmp(mode, "changed-back") == 0)
            gtk_editable_set_text(GTK_EDITABLE(arguments), "formatting-valid");
        CHECK(Wait(GTK_WIDGET(dialog)) && !gtk_widget_get_sensitive(preview));
        goto inspect;
    }
    CHECK(Wait(GTK_WIDGET(dialog)) && gtk_widget_get_sensitive(preview));
    if (strcmp(mode, "options-change") == 0)
    {
        GtkWidget *tab = Find(GTK_WIDGET(dialog), "document.formatting.tab-size");
        CHECK(tab != NULL);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(tab), 8.0);
        CHECK(!gtk_widget_get_sensitive(preview));
        goto inspect;
    }
    if (strcmp(mode, "no-preview") != 0)
        g_signal_emit_by_name(preview, "clicked");
    if (strcmp(mode, "private") == 0)
    {
        GtkWidget *left = Find(GTK_WIDGET(dialog), "umicom.comparison.left");
        CHECK(left != NULL && UmiGtk4RecordingIsPrivate(left) && UmiGtk4RecordingIsPrivate(program));
        goto inspect;
    }
    if (strcmp(mode, "no-approval") != 0)
        gtk_check_button_set_active(GTK_CHECK_BUTTON(approve), TRUE);
    if (strcmp(mode, "stale") == 0)
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "push", 4U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    if (strcmp(mode, "completion-unbind") == 0)
        result.detach = adapter;
    g_signal_emit_by_name(apply, "clicked");
    if (strcmp(mode, "apply") == 0 || strcmp(mode, "undo") == 0 || strcmp(mode, "completion-unbind") == 0 ||
        strcmp(mode, "tabs") == 0 || strcmp(mode, "multiline") == 0)
    {
        CHECK(result.count == 1U && result.status == UMI_STATUS_OK &&
              !UmiGtk4AdapterCompletionReviewBusy(adapter));
        if (strcmp(mode, "undo") == 0)
            CHECK(UmiDocumentCoordinatorUndo(documents, active.document_id) == UMI_STATUS_OK);
    }
    else
        CHECK(result.count == 0U);
inspect:
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &text, &length) == UMI_STATUS_OK);
    const char *expected =
        strcmp(mode, "multiline") == 0 ? "puts\n"
        : (strcmp(mode, "apply") == 0 || strcmp(mode, "completion-unbind") == 0 || strcmp(mode, "tabs") == 0)
            ? "puts"
        : strcmp(mode, "stale") == 0 ? "push"
                                     : "pu";
    CHECK(length == strlen(expected) && strcmp(text, expected) == 0);
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
    g_clear_object(&apply);
    umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    g_free(directory);
    return failed;
}
#include "../native_process/utf8_entry.inc"
