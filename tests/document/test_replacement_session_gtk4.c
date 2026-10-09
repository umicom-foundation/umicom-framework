/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_replacement_session_gtk4.c
 * PURPOSE: Exercise native replacement decisions and teardown against the shared document service.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/document_commands.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%d: %s\n", __LINE__, #x); failed = 1; goto cleanup; } } while (0)

typedef struct Completion { unsigned count; UmiStatus status; UmiGtk4Adapter *detach; } Completion;
static void Complete(void *data, UmiStatus status)
{
    Completion *result = data; ++result->count; result->status = status;
    if (result->detach != NULL) (void)UmiGtk4AdapterBindDocumentEditing(result->detach, NULL, NULL, NULL);
}

/* Inspect semantic controls, never GTK implementation child positions. */
/* The rendered-only search missed controls in collapsed review panels. The Framework logical-tree helper replaces it; retain the former traversal for review. */
#if 0
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    if (root == NULL) return NULL;
    const char *actual = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (actual != NULL && strcmp(actual, id) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found != NULL) return found;
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
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index) {
        GtkWindow *window = g_list_model_get_item(windows, index);
        if (Find(GTK_WIDGET(window), "document.replacements.review") != NULL) return window;
        g_object_unref(window);
    }
    return NULL;
}
/* A toplevel observer can replace a binding during native construction. */
static void UnbindOnCreate(GListModel *model, guint position, guint removed, guint added, gpointer data)
{
    (void)model; (void)position; (void)removed;
    Completion *result = data;
    if (added != 0U && result->detach != NULL) {
        UmiGtk4Adapter *adapter = result->detach; result->detach = NULL;
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    }
}

static void Pump(void)
{
    /* Bound fixture work even if an unrelated source keeps the main loop busy. */
    for (unsigned iteration = 0U; iteration < 100U && g_main_context_pending(NULL); ++iteration)
        (void)g_main_context_iteration(NULL, FALSE);
}

int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    const char *name = argv[1];
    const char *cases[] = {"apply", "skip", "stop", "stale", "destroy", "unbind", "pending-close", "complete-unbind", "busy", "creation-unbind", "parent-close"};
    int known = 0;
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index) if (strcmp(name, cases[index]) == 0) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    GtkApplication *application = NULL; GtkWindow *dialog = NULL; GtkWidget *apply = NULL;
    UmiCommandRegistry *commands = NULL; UmiUiWorkbench *workbench = NULL; UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL; UmiDocumentCoordinator *documents = NULL; UmiGtk4Adapter *adapter = NULL;
    Completion result = {0}; UmiDocumentWorkingCopySnapshot active; UmiUiDocumentViewSnapshot view;
    UmiDocumentReplacementProgress progress;
    char *text = NULL; size_t length = 0U;
    application = gtk_application_new("org.umicom.replacements.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("test.replacements.native", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.replacements.test", "Review", workbench, &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "review.c", NULL, 0U) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "note note", 9U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &result) == UMI_STATUS_OK);
    if (strcmp(name, "creation-unbind") == 0) {
        result.detach = adapter;
        gulong observer = g_signal_connect(gtk_window_get_toplevels(), "items-changed", G_CALLBACK(UnbindOnCreate), &result);
        UmiStatus status = UmiGtk4AdapterReviewOpenReplacements(adapter, "note", "saved");
        g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
        CHECK(status == UMI_STATUS_CANCELLED && result.count == 0U && !UmiGtk4AdapterReplacementReviewBusy(adapter));
        dialog = Review(); CHECK(dialog == NULL); goto unchanged;
    }
    CHECK(UmiGtk4AdapterReviewOpenReplacements(adapter, "note", "saved") == UMI_STATUS_OK);
    dialog = Review(); CHECK(dialog != NULL && UmiGtk4AdapterReplacementReviewBusy(adapter));
    GtkWidget *button = Find(GTK_WIDGET(dialog), "document.replacements.apply"); CHECK(GTK_IS_BUTTON(button)); apply = g_object_ref(button);
    if (strcmp(name, "pending-close") == 0) {
        gtk_window_destroy(dialog); Pump();
        CHECK(!UmiGtk4AdapterReplacementReviewBusy(adapter) && result.count == 1U && result.status == UMI_STATUS_CANCELLED);
        g_signal_emit_by_name(apply, "clicked"); goto unchanged;
    }
    Pump();
    CHECK(UmiGtk4AdapterReplacementReviewProgress(adapter, &progress) == UMI_STATUS_OK && progress.phase == UMI_DOCUMENT_REPLACEMENT_REVIEW);
    CHECK(gtk_widget_get_sensitive(apply));
    GtkWidget *stop = Find(GTK_WIDGET(dialog), "document.replacements.stop"); CHECK(GTK_IS_BUTTON(stop));
    CHECK(gtk_window_get_default_widget(dialog) == stop);
    if (strcmp(name, "busy") == 0) {
        CHECK(UmiGtk4AdapterReviewOpenReplacements(adapter, "note", "other") == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterDocumentSaveAll(adapter, NULL, NULL) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterRequestDocumentClose(adapter, NULL) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterPromptDocumentLocation(adapter) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterCloseDocuments(adapter, UMI_DOCUMENT_CLOSE_ALL, NULL, NULL) == UMI_STATUS_BUSY);
        goto unchanged;
    }
    if (strcmp(name, "unbind") == 0) {
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        CHECK(!UmiGtk4AdapterReplacementReviewBusy(adapter) && result.count == 0U);
        g_signal_emit_by_name(apply, "clicked"); goto unchanged;
    }
    if (strcmp(name, "parent-close") == 0) {
        GtkWidget *parent = g_object_ref(umi_gtk4_adapter_native_window(adapter));
        gtk_window_destroy(GTK_WINDOW(parent)); Pump(); g_object_unref(parent);
        CHECK(!UmiGtk4AdapterReplacementReviewBusy(adapter) && result.count == 0U);
        g_signal_emit_by_name(apply, "clicked"); goto unchanged;
    }
    if (strcmp(name, "destroy") == 0) {
        gtk_window_destroy(dialog); g_signal_emit_by_name(apply, "clicked");
        CHECK(!UmiGtk4AdapterReplacementReviewBusy(adapter) && result.count == 1U); goto unchanged;
    }
    if (strcmp(name, "stale") == 0) CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "later typing", 12U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    if (strcmp(name, "skip") == 0) {
        GtkWidget *skip = Find(GTK_WIDGET(dialog), "document.replacements.skip"); CHECK(GTK_IS_BUTTON(skip));
        g_signal_emit_by_name(skip, "clicked");
    } else if (strcmp(name, "stop") == 0) g_signal_emit_by_name(stop, "clicked");
    else g_signal_emit_by_name(apply, "clicked");
    CHECK(UmiGtk4AdapterReplacementReviewProgress(adapter, &progress) == UMI_STATUS_OK);
    CHECK(progress.phase == (strcmp(name, "stop") == 0 ? UMI_DOCUMENT_REPLACEMENT_CANCELLED :
        strcmp(name, "stale") == 0 ? UMI_DOCUMENT_REPLACEMENT_FAILED : UMI_DOCUMENT_REPLACEMENT_COMPLETE));
    CHECK(result.count == 0U);
    if (strcmp(name, "complete-unbind") == 0) result.detach = adapter;
    g_signal_emit_by_name(stop, "clicked");
    CHECK(result.count == 1U && !UmiGtk4AdapterReplacementReviewBusy(adapter));
    g_signal_emit_by_name(apply, "clicked");
unchanged:
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &text, &length) == UMI_STATUS_OK);
    const char *expected = strcmp(name, "apply") == 0 || strcmp(name, "complete-unbind") == 0 ? "saved saved" :
        strcmp(name, "stale") == 0 ? "later typing" : "note note";
    CHECK(length == strlen(expected) && strcmp(text, expected) == 0);
cleanup:
    UmiUiDocumentViewModelFreeText(text);
    if (adapter != NULL) (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL) gtk_window_destroy(dialog);
    g_clear_object(&dialog); g_clear_object(&apply);
    umi_gtk4_adapter_destroy(adapter); umi_document_coordinator_destroy(documents); umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell); umi_ui_workbench_destroy(workbench); umi_command_registry_destroy(commands);
    g_clear_object(&application); return failed;
}
