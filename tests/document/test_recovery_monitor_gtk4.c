/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_recovery_monitor_gtk4.c
 * PURPOSE: Exercise opted-in timer capture, storage failures and binding retirement with isolated native storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/document_recovery.h"
#include "umicom/document/recovery_storage.h"
#include "umicom/platform/output_file.h"
#include <gtk/gtk.h>
#include <glib/gstdio.h>
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
typedef struct Event
{
    UmiDocumentRecoveryScheduleInfo last;
    unsigned count;
    UmiGtk4Adapter *detach;
} Event;
static void Changed(void *data, const UmiDocumentRecoveryScheduleInfo *info)
{
    Event *event = data;
    event->last = *info;
    ++event->count;
    if (event->detach != NULL && info->saved_snapshots != 0U)
    {
        UmiGtk4Adapter *owner = event->detach;
        event->detach = NULL;
        (void)UmiGtk4AdapterBindDocumentEditing(owner, NULL, NULL, NULL);
    }
}
static int WaitEvent(Event *event, uint64_t saved, int paused)
{
    gint64 deadline = g_get_monotonic_time() + 12 * G_TIME_SPAN_SECOND;
    while (g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        if (event->last.saved_snapshots >= saved && event->last.paused_after_failure == paused)
            return 1;
        g_usleep(1000U);
    }
    return 0;
}
static void PumpFor(gint64 milliseconds)
{
    gint64 deadline = g_get_monotonic_time() + milliseconds * 1000;
    while (g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
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
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1],
               *cases[] = {"disabled",        "periodic",      "unchanged", "newer",        "read-only",
                           "multiple",        "failure-pause", "disable",   "parent-close", "unbind",
                           "observer-detach", "form-enable",   "form-pause"};
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
    UmiDocumentRecoveryCatalogue *catalogue = NULL;
    gchar *base = g_dir_make_tmp("umicom-periodic-recovery-XXXXXX", NULL);
    Event event = {0};
    char directory[UMI_PATH_CAPACITY], view_id[UMI_UI_ID_CAPACITY];
    CHECK(base != NULL);
    application = gtk_application_new("org.umicom.recovery.monitor.test", G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, NULL));
    CHECK(umi_command_registry_create(&commands) == UMI_STATUS_OK);
    CHECK(umi_ui_workbench_create("recovery.monitor", commands, &workbench) == UMI_STATUS_OK);
    CHECK(umi_ui_application_shell_create("org.umicom.recovery.monitor.test", "Recovery monitor", workbench,
                                          &shell) == UMI_STATUS_OK);
    CHECK(umi_document_store_create(&store) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_create(store, workbench, NULL, &documents) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_create(application, &adapter) == UMI_STATUS_OK);
    CHECK(umi_gtk4_adapter_prepare(adapter, shell) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, documents, NULL, NULL) == UMI_STATUS_OK);
    CHECK(UmiGtk4AdapterObserveRecovery(adapter, Changed, &event) == UMI_STATUS_OK);
    CHECK(umi_document_coordinator_new(documents, "draft.c", view_id, sizeof(view_id)) == UMI_STATUS_OK);
    UmiUiDocumentViewModel *views = umi_ui_workbench_documents(workbench);
    UmiUiDocumentViewSnapshot view;
    CHECK(umi_ui_document_view_model_find(views, view_id, &view) == UMI_STATUS_OK);
    view.dirty = 1;
    view.read_only = strcmp(mode, "read-only") == 0;
    CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "draft source", 12U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
    CHECK(UmiDocumentRecoveryDirectory("MonitorTest", base, directory, sizeof(directory)) == UMI_STATUS_OK);
    if (strcmp(mode, "form-enable") == 0 || strcmp(mode, "form-pause") == 0)
    {
        CHECK(UmiGtk4AdapterReviewRecovery(adapter, "MonitorTest", base) == UMI_STATUS_OK);
        dialog = Review();
        CHECK(dialog != NULL);
        GtkWidget *automatic = Find(GTK_WIDGET(dialog), "document.recovery.automatic");
        CHECK(automatic);
        g_signal_emit_by_name(automatic, "clicked");
        UmiDocumentRecoveryScheduleInfo info;
        CHECK(UmiGtk4AdapterRecoveryState(adapter, &info) == UMI_STATUS_OK && info.enabled &&
              info.interval_ms == 60000U);
        if (strcmp(mode, "form-pause") == 0)
        {
            g_signal_emit_by_name(automatic, "clicked");
            CHECK(UmiGtk4AdapterRecoveryState(adapter, &info) == UMI_STATUS_OK && !info.enabled);
        }
        goto cleanup;
    }
    if (strcmp(mode, "multiple") == 0)
    {
        char other[UMI_UI_ID_CAPACITY];
        CHECK(umi_document_coordinator_new(documents, "other.c", other, sizeof(other)) == UMI_STATUS_OK);
        UmiUiDocumentViewSnapshot copy;
        CHECK(umi_ui_document_view_model_find(views, other, &copy) == UMI_STATUS_OK);
        copy.dirty = 1;
        CHECK(UmiUiDocumentViewModelUpsertText(views, &copy, "other", 5U) == UMI_STATUS_OK);
    }
    if (strcmp(mode, "failure-pause") == 0)
    {
        char parent[UMI_PATH_CAPACITY];
        CHECK(umi_path_parent(directory, parent, sizeof(parent)) == UMI_STATUS_OK &&
              g_mkdir_with_parents(parent, 0700) == 0);
        UmiOutputFile *file = NULL;
        CHECK(UmiOutputFileCreate(directory, &file) == UMI_STATUS_OK);
        CHECK(UmiOutputFileWrite(file, "blocked", 7U) == UMI_STATUS_OK);
        CHECK(UmiOutputFileClose(file) == UMI_STATUS_OK);
        UmiOutputFileDestroy(file);
    }
    if (strcmp(mode, "observer-detach") == 0)
        event.detach = adapter;
    CHECK(UmiGtk4AdapterConfigureRecovery(adapter, "MonitorTest", base, strcmp(mode, "disabled") == 0 ? 0 : 1,
                                          1000U) == UMI_STATUS_OK);
    if (strcmp(mode, "disabled") == 0)
    {
        PumpFor(2200);
        CHECK(event.last.saved_snapshots == 0U);
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK &&
              UmiDocumentRecoveryCatalogueCount(catalogue) == 0U);
        goto cleanup;
    }
    if (strcmp(mode, "parent-close") == 0 || strcmp(mode, "unbind") == 0)
    {
        unsigned notifications = event.count;
        if (strcmp(mode, "parent-close") == 0)
            gtk_window_destroy(GTK_WINDOW(umi_gtk4_adapter_native_window(adapter)));
        else
            CHECK(UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL) == UMI_STATUS_OK);
        PumpFor(2200);
        CHECK(event.count == notifications);
        CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK &&
              UmiDocumentRecoveryCatalogueCount(catalogue) == 0U);
        goto cleanup;
    }
    if (strcmp(mode, "failure-pause") == 0)
    {
        CHECK(WaitEvent(&event, 0U, 1));
        unsigned notifications = event.count;
        CHECK(event.last.last_status != UMI_STATUS_OK);
        PumpFor(2200);
        CHECK(event.count == notifications && event.last.saved_snapshots == 0U);
        goto cleanup;
    }
    uint64_t wanted = strcmp(mode, "multiple") == 0 ? 2U : 1U;
    CHECK(WaitEvent(&event, wanted, 0));
    if (strcmp(mode, "newer") == 0)
    {
        CHECK(umi_ui_document_view_model_find(views, view_id, &view) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "newer source", 12U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
        wanted = 2U;
        CHECK(WaitEvent(&event, wanted, 0));
    }
    if (strcmp(mode, "unchanged") == 0 || strcmp(mode, "observer-detach") == 0)
    {
        unsigned notifications = event.count;
        PumpFor(2200);
        CHECK(event.count == notifications && event.last.saved_snapshots == wanted);
    }
    if (strcmp(mode, "disable") == 0)
    {
        CHECK(UmiGtk4AdapterConfigureRecovery(adapter, "MonitorTest", base, 0, 1000U) == UMI_STATUS_OK);
        CHECK(umi_ui_document_view_model_find(views, view_id, &view) == UMI_STATUS_OK);
        CHECK(UmiUiDocumentViewModelUpsertText(views, &view, "later", 5U) == UMI_STATUS_OK);
    /* UpsertText copies its input; reload the owned preview before later
     * metadata edits so this fixture cannot restore the previous draft. */
    CHECK(umi_ui_document_view_model_find(views, view.view_id, &view) == UMI_STATUS_OK);
        PumpFor(2200);
        CHECK(!event.last.enabled && event.last.saved_snapshots == 1U);
    }
    CHECK(UmiDocumentRecoveryList(directory, NULL, &catalogue) == UMI_STATUS_OK &&
          UmiDocumentRecoveryCatalogueCount(catalogue) == wanted);
cleanup:
    if (adapter != NULL)
        (void)UmiGtk4AdapterBindDocumentEditing(adapter, NULL, NULL, NULL);
    if (dialog != NULL)
    {
        gtk_window_destroy(dialog);
        g_object_unref(dialog);
    }
    UmiDocumentRecoveryCatalogueDestroy(catalogue);
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
