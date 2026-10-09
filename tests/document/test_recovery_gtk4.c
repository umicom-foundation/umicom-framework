/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_gtk4.c
 * PURPOSE: Exercise native recovery controls, independent drafts and worker retirement with temporary private storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/document_recovery.h"
#include "umicom/document/recovery_storage.h"
#include "umicom/platform/rooted_files.h"
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
    Completion *done = data;
    ++done->count;
    done->status = status;
    if (done->detach != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(done->detach, NULL, NULL, NULL);
}
/* The rendered-only search missed controls in collapsed review panels. The Framework logical-tree helper replaces it; retain the former traversal for review. */
#if 0
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
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
        if (Find(GTK_WIDGET(window), "document.recovery.review") != NULL)
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static int Wait(GtkWindow *window)
{
    gint64 deadline = g_get_monotonic_time() + 20 * G_TIME_SPAN_SECOND;
    while (g_object_get_data(G_OBJECT(window), "umicom-recovery-pending") != NULL &&
           g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return g_object_get_data(G_OBJECT(window), "umicom-recovery-pending") == NULL;
}
static void SubstitutePreview(GtkTextBuffer *buffer, gpointer data)
{
    int *once = data;
    if (*once)
    {
        *once = 0;
        gtk_text_buffer_set_text(buffer, "observer changed source", -1);
    }
}
static void OnCreate(GListModel *list, guint position, guint removed, guint added, gpointer data)
{
    (void)list;
    (void)position;
    (void)removed;
    if (added != 0U)
        (void)UmiGtk4AdapterBindDocumentEditing(data, NULL, NULL, NULL);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1], *cases[] = {"open",
                                            "empty-list",
                                            "capture",
                                            "preview",
                                            "restore",
                                            "pending-source",
                                            "selection",
                                            "unicode",
                                            "no-document",
                                            "read-only",
                                            "malformed",
                                            "missing",
                                            "change-selection",
                                            "worker-close",
                                            "worker-unbind",
                                            "parent-close",
                                            "retained-control",
                                            "restore-detach",
                                            "creation-unbind",
                                            "busy",
                                            "invalid-base",
                                            "preview-changed",
                                            "preview-substituted"};
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
    UmiCommandRegistry *commands = NULL;
    UmiUiWorkbench *workbench = NULL;
    UmiUiApplicationShell *shell = NULL;
    UmiDocumentStore *store = NULL;
    UmiDocumentCoordinator *documents = NULL;
    UmiGtk4Adapter *adapter = NULL;
    GtkWindow *dialog = NULL;
    GtkWidget *retained = NULL;
    UmiDocumentRecoveryCatalogue *catalogue = NULL;
    UmiDocumentRecoveryDraft *loaded = NULL;
    gchar *base = g_dir_make_tmp("umicom-recovery-XXXXXX", NULL), *shown = NULL;
    char *text = NULL;
    Completion completion = {0};
    CHECK(base != NULL);
    application = gtk_application_new("org.umicom.recovery.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("recovery", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.recovery.test", "Recovery", workbench, &shell) ==
          UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, Complete, &completion) == UMI_STATUS_OK);
    const char *source = strcmp(mode, "unicode") == 0 ? "a\xe9\x9b\xaa" : "unsaved\nsource";
    UmiDocumentId original = 0U;
    UmiDocumentWorkingCopySnapshot active;
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    if (strcmp(mode, "no-document") != 0)
    {
        CHECK(umi_document_coordinator_new(documents, "draft.c", NULL, 0U) == UMI_STATUS_OK);
        CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK);
        original = active.document_id;
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK);
        view.dirty = 1;
        view.read_only = strcmp(mode, "read-only") == 0;
        view.cursor_offset = strcmp(mode, "selection") == 0 ? 8U : 0U;
        view.selection_length = strcmp(mode, "selection") == 0 ? 6U : 0U;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, source, strlen(source)) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "capture") == 0)
        CHECK(umi_document_coordinator_sync_active(documents) == UMI_STATUS_OK);
    if (strcmp(mode, "invalid-base") == 0)
    {
        CHECK(UmiGtk4AdapterReviewRecovery(adapter, "RecoveryTest", "relative") != UMI_STATUS_OK &&
              !UmiGtk4AdapterCompletionReviewBusy(adapter));
        goto cleanup;
    }
    if (strcmp(mode, "creation-unbind") == 0)
    {
        gulong observer =
            g_signal_connect(gtk_window_get_toplevels(), "items-changed", G_CALLBACK(OnCreate), adapter);
        UmiStatus status = UmiGtk4AdapterReviewRecovery(adapter, "RecoveryTest", base);
        g_signal_handler_disconnect(gtk_window_get_toplevels(), observer);
        CHECK(status == UMI_STATUS_CANCELLED && !UmiGtk4AdapterCompletionReviewBusy(adapter));
        dialog = Review();
        CHECK(dialog == NULL);
        goto cleanup;
    }
    CHECK(UmiGtk4AdapterReviewRecovery(adapter, "RecoveryTest", base) == UMI_STATUS_OK);
    dialog = Review();
    CHECK(dialog != NULL);
    GtkWidget *save = Find(GTK_WIDGET(dialog), "document.recovery.save"),
              *refresh = Find(GTK_WIDGET(dialog), "document.recovery.refresh"),
              *load = Find(GTK_WIDGET(dialog), "document.recovery.load"),
              *restore = Find(GTK_WIDGET(dialog), "document.recovery.restore"),
              *choice = Find(GTK_WIDGET(dialog), "document.recovery.choice"),
              *preview = Find(GTK_WIDGET(dialog), "document.recovery.preview");
    CHECK(save && refresh && load && restore && choice && preview && !gtk_widget_get_sensitive(restore));
    char directory[UMI_PATH_CAPACITY];
    CHECK(UmiDocumentRecoveryDirectory("RecoveryTest", base, directory, sizeof(directory)) == UMI_STATUS_OK);
    if (strcmp(mode, "open") == 0)
    {
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK &&
              UmiDocumentRecoveryCatalogueCount(catalogue) == 0U);
        goto cleanup;
    }
    if (strcmp(mode, "busy") == 0)
    {
        CHECK(UmiGtk4AdapterReviewRecovery(adapter, "RecoveryTest", base) == UMI_STATUS_BUSY);
        CHECK(UmiGtk4AdapterReviewSnippet(adapter) == UMI_STATUS_BUSY);
        goto cleanup;
    }
    if (strcmp(mode, "empty-list") == 0)
    {
        g_signal_emit_by_name(refresh, "clicked");
        CHECK(Wait(dialog));
        CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(choice))) == 0U &&
              !gtk_widget_get_sensitive(restore));
        goto cleanup;
    }
    g_signal_emit_by_name(save, "clicked");
    CHECK(Wait(dialog));
    CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK);
    if (strcmp(mode, "no-document") == 0)
    {
        CHECK(UmiDocumentRecoveryCatalogueCount(catalogue) == 0U);
        goto cleanup;
    }
    CHECK(UmiDocumentRecoveryCatalogueCount(catalogue) == 1U);
    UmiDocumentRecoveryEntry entry;
    CHECK(UmiDocumentRecoveryCatalogueAt(catalogue, 0U, &entry) == UMI_STATUS_OK);
    CHECK(UmiDocumentRecoveryLoad(directory, entry.key, NULL, &loaded) == UMI_STATUS_OK);
    const char *saved = NULL;
    size_t saved_bytes = 0U;
    CHECK(UmiDocumentRecoveryDraftRead(loaded, &saved, &saved_bytes) == UMI_STATUS_OK &&
          strcmp(saved, source) == 0);
    if (strcmp(mode, "capture") == 0 || strcmp(mode, "pending-source") == 0 || strcmp(mode, "read-only") == 0)
    {
        UmiDocumentSnapshot snapshot;
        CHECK(umi_document_store_snapshot(store, original, &snapshot) == UMI_STATUS_OK &&
              snapshot.length == (strcmp(mode, "capture") == 0 ? strlen(source) : 0U));
        goto cleanup;
    }
    if (strcmp(mode, "malformed") == 0 || strcmp(mode, "missing") == 0)
    {
        char leaf[39];
        (void)snprintf(leaf, sizeof(leaf), "%s.draft", entry.key);
        if (strcmp(mode, "malformed") == 0)
            CHECK(UmiRootedFileWrite(directory, leaf, "broken", 6U) == UMI_STATUS_OK);
    }
    g_signal_emit_by_name(refresh, "clicked");
    CHECK(Wait(dialog));
    gtk_drop_down_set_selected(GTK_DROP_DOWN(choice), 0U);
    if (strcmp(mode, "missing") == 0)
    {
        char leaf[39];
        (void)snprintf(leaf, sizeof(leaf), "%s.draft", entry.key);
        CHECK(UmiRootedFileRemove(directory, leaf) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "retained-control") == 0)
    {
        retained = g_object_ref(restore);
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        g_signal_emit_by_name(retained, "clicked");
        CHECK(umi_document_coordinator_count(documents) == 1U);
        goto cleanup;
    }
    int substitute_once = 1;
    gulong substitution = 0U;
    if (strcmp(mode, "preview-substituted") == 0)
        substitution = g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(preview)), "changed",
                                        G_CALLBACK(SubstitutePreview), &substitute_once);
    g_signal_emit_by_name(load, "clicked");
    if (strcmp(mode, "worker-close") == 0)
        gtk_window_destroy(dialog);
    if (strcmp(mode, "worker-unbind") == 0)
        CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
    if (strcmp(mode, "parent-close") == 0)
        gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(adapter)));
    if (strcmp(mode, "change-selection") == 0)
        /* GTK may ignore an invalid index on a nonempty list. An empty model
         * exercises the no-selection path. Retain the former setup for review. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(choice), GTK_INVALID_LIST_POSITION);
#endif
        gtk_drop_down_set_model(GTK_DROP_DOWN(choice), NULL);
    CHECK(Wait(dialog));
    if (substitution != 0U)
        g_signal_handler_disconnect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(preview)), substitution);
    if (strcmp(mode, "preview-changed") == 0)
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(preview)), "changed after load", -1);
    if (strcmp(mode, "preview-changed") == 0 || strcmp(mode, "preview-substituted") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(restore));
        g_signal_emit_by_name(restore, "clicked");
        CHECK(umi_document_coordinator_count(documents) == 1U);
        goto cleanup;
    }
    if (strcmp(mode, "worker-close") == 0 || strcmp(mode, "worker-unbind") == 0 ||
        strcmp(mode, "parent-close") == 0)
    {
        g_signal_emit_by_name(restore, "clicked");
        CHECK(umi_document_coordinator_count(documents) == 1U);
        goto cleanup;
    }
    if (strcmp(mode, "malformed") == 0 || strcmp(mode, "missing") == 0 ||
        strcmp(mode, "change-selection") == 0)
    {
        CHECK(!gtk_widget_get_sensitive(restore));
        g_signal_emit_by_name(restore, "clicked");
        CHECK(umi_document_coordinator_count(documents) == 1U);
        goto cleanup;
    }
    GtkTextIter begin, end;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(preview));
    gtk_text_buffer_get_bounds(buffer, &begin, &end);
    shown = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
    CHECK(strcmp(shown, source) == 0 && gtk_widget_get_sensitive(restore));
    if (strcmp(mode, "preview") == 0)
        goto cleanup;
    if (strcmp(mode, "restore-detach") == 0)
        completion.detach = adapter;
    g_signal_emit_by_name(restore, "clicked");
    CHECK(umi_document_coordinator_count(documents) == 2U);
    CHECK(umi_document_coordinator_active_snapshot(documents, &active) == UMI_STATUS_OK &&
          active.document_id != original && !active.has_path && active.dirty && active.undo_count == 0U);
    size_t bytes = 0U;
    CHECK(UmiUiDocumentViewModelCopyText(views, active.view_id, &text, &bytes) == UMI_STATUS_OK &&
          strcmp(text, source) == 0);
    if (strcmp(mode, "selection") == 0)
    {
        UmiUiDocumentViewSnapshot view;
        CHECK(umi_ui_document_view_model_find(views, active.view_id, &view) == UMI_STATUS_OK &&
              view.cursor_offset == 8U && view.selection_length == 6U);
    }
    CHECK(completion.count == 1U && completion.status == UMI_STATUS_OK);
cleanup:
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
    {
        if (!Wait(dialog))
            failed = 1;
        gtk_window_destroy(dialog);
        g_object_unref(dialog);
    }
    g_clear_object(&retained);
    UmiDocumentRecoveryCatalogueDestroy(catalogue);
    UmiDocumentRecoveryDraftDestroy(loaded);
    UmiUiDocumentViewModelFreeText(text);
    g_free(shown);
    if (adapter != NULL)
        umi_gtk4_adapter_destroy(adapter);
    umi_document_coordinator_destroy(documents);
    umi_document_store_destroy(store);
    umi_ui_application_shell_destroy(shell);
    umi_ui_workbench_destroy(workbench);
    umi_command_registry_destroy(commands);
    g_clear_object(&application);
    g_free(base);
    return failed;
}
