/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_recovery.inc
 * PURPOSE: Keep recovery I/O on workers and restore only the explicitly previewed independent draft.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/document_recovery.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include "umicom/document/recovery_coordinator.h"
#include "umicom/document/recovery_storage.h"
#include <inttypes.h>
typedef enum RecoveryOperation
{
    RECOVERY_SAVE,
    RECOVERY_LIST,
    RECOVERY_LOAD
} RecoveryOperation;
typedef struct RecoveryWork
{
    GWeakRef window;
    UmiCancellationToken *cancel;
    UmiDocumentRecoveryDraft *draft;
    UmiDocumentRecoveryCatalogue *catalogue;
    char directory[UMI_PATH_CAPACITY], key[UMI_DOCUMENT_RECOVERY_KEY_CAPACITY];
    UmiStatus status;
    RecoveryOperation operation;
    uint64_t generation;
} RecoveryWork;
typedef struct RecoveryPanel
{
    GObject *lifetime;
    GPtrArray *controls;
    GtkWidget *choice, *preview, *restore, *message, *save, *refresh, *load;
    GtkWidget *automatic, *automatic_status;
    gchar *application_directory, *base_override;
    UmiDocumentRecoveryCatalogue *catalogue;
    UmiDocumentRecoveryDraft *draft;
    RecoveryWork *pending;
    char directory[UMI_PATH_CAPACITY];
    uint64_t generation;
    int closed, busy, publishing_preview;
} RecoveryPanel;
static RecoveryPanel *RecoveryState(GtkWidget *window)
{
    return g_object_get_data(G_OBJECT(window), "umicom-document-recovery");
}
static UmiGtk4Adapter *RecoveryOwner(GtkWidget *window, RecoveryPanel *panel)
{
    if (panel == NULL || panel->closed || g_object_get_data(panel->lifetime, "recovery-review") != window)
        return NULL;
    UmiGtk4Adapter *owner = g_object_get_data(panel->lifetime, "adapter");
    return owner != NULL && owner->edit_coordinator != NULL && owner->window != NULL &&
                   UmiGtk4WindowIsOpen(owner->window)
               ? owner
               : NULL;
}
static void RecoveryFree(gpointer data)
{
    RecoveryPanel *panel = data;
    UmiDocumentRecoveryCatalogueDestroy(panel->catalogue);
    UmiDocumentRecoveryDraftDestroy(panel->draft);
    g_free(panel->application_directory);
    g_free(panel->base_override);
    g_ptr_array_unref(panel->controls);
    g_object_unref(panel->lifetime);
    g_free(panel);
}
static void RecoveryWorkFree(RecoveryWork *work)
{
    g_weak_ref_clear(&work->window);
    umi_cancellation_token_destroy(work->cancel);
    UmiDocumentRecoveryDraftDestroy(work->draft);
    UmiDocumentRecoveryCatalogueDestroy(work->catalogue);
    g_free(work);
}
static void RecoveryReviewClose(GtkWidget *window)
{
    if (window == NULL)
        return;
    g_object_ref(window);
    RecoveryPanel *panel = RecoveryState(window);
    if (panel != NULL && !panel->closed)
    {
        panel->closed = 1;
        if (panel->pending != NULL)
            umi_cancellation_token_request(panel->pending->cancel);
        if (g_object_get_data(panel->lifetime, "recovery-review") == window)
            g_object_set_data(panel->lifetime, "recovery-review", NULL);
        gtk_window_destroy(GTK_WINDOW(window));
    }
    g_object_unref(window);
}
static void RecoveryReviewDetach(GObject *lifetime)
{
    RecoveryReviewClose(g_object_get_data(lifetime, "recovery-review"));
}
static gboolean RecoveryClosing(GtkWindow *window, gpointer data)
{
    (void)data;
    RecoveryReviewClose(GTK_WIDGET(window));
    return TRUE;
}
static void RecoveryRemoved(GtkWidget *window, gpointer data)
{
    (void)data;
    RecoveryReviewClose(window);
}
static void RecoveryParentRemoved(GtkWidget *parent, gpointer window)
{
    (void)parent;
    RecoveryReviewClose(window);
}
static void RecoveryCancel(GtkButton *button, gpointer window)
{
    (void)button;
    RecoveryReviewClose(window);
}
static GtkWidget *RecoveryControl(RecoveryPanel *panel, GtkWidget *box, GtkWidget *control, const char *id)
{
    g_ptr_array_add(panel->controls, g_object_ref_sink(control));
    (void)umi_gtk4_automation_tag_widget(control, id);
    gtk_box_append(GTK_BOX(box), control);
    return control;
}
/* Status belongs to the binding, so closing and reopening the form does not
 * accidentally disable an opted-in timer or hide a paused storage failure. */
static void RecoveryPanelMonitorChanged(GObject *lifetime)
{
    GtkWidget *window = g_object_get_data(lifetime, "recovery-review");
    if (window == NULL)
        return;
    g_object_ref(window);
    RecoveryPanel *panel = RecoveryState(window);
    UmiGtk4Adapter *owner = RecoveryOwner(window, panel);
    UmiDocumentRecoveryScheduleInfo info;
    if (owner != NULL && UmiGtk4AdapterRecoveryState(owner, &info) == UMI_STATUS_OK)
    {
        const char *action = !info.enabled               ? "Enable automatic snapshots (every minute)"
                             : info.paused_after_failure ? "Resume automatic snapshots"
                                                         : "Pause automatic snapshots";
        gtk_button_set_label(GTK_BUTTON(panel->automatic), action);
        if (RecoveryOwner(window, panel) != NULL)
        {
            gchar *message;
            if (info.paused_after_failure)
                message =
                    g_strdup_printf("Automatic recovery paused: %s. Keep your drafts open, resolve the "
                                    "storage problem, then choose Resume. Earlier snapshots remain intact.",
                                    umi_status_text(info.last_status));
            else if (info.enabled)
                message = g_strdup_printf(
                    "Automatic recovery is on for this editing session: %" PRIu64
                    " snapshots saved. Changed dirty drafts are captured after at least %" PRIu64
                    " seconds each. Close this form to continue periodic capture.",
                    info.saved_snapshots, info.interval_ms / 1000U);
            else
                message = g_strdup(
                    "Automatic recovery is off. Enabling it captures local plaintext copies while this "
                    "editing session is open. Saved copies remain after closure. Source files are not "
                    "saved and old snapshots are not pruned.");
            gtk_label_set_text(GTK_LABEL(panel->automatic_status), message);
            g_free(message);
        }
    }
    g_object_unref(window);
}
static void RecoveryAutomaticClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    RecoveryPanel *panel = RecoveryState(window);
    UmiGtk4Adapter *owner = RecoveryOwner(window, panel);
    UmiDocumentRecoveryScheduleInfo info;
    if (owner != NULL && UmiGtk4AdapterRecoveryState(owner, &info) == UMI_STATUS_OK)
    {
        int enabled = !info.enabled || info.paused_after_failure;
        UmiStatus status = UmiGtk4AdapterConfigureRecovery(owner, panel->application_directory,
                                                           panel->base_override, enabled, 60000U);
        if (status != UMI_STATUS_OK && RecoveryOwner(window, panel) != NULL)
        {
            gchar *message =
                g_strdup_printf("Could not change automatic recovery: %s", umi_status_text(status));
            gtk_label_set_text(GTK_LABEL(panel->automatic_status), message);
            g_free(message);
        }
    }
    g_object_unref(window);
}
static void RecoveryMessage(GtkWidget *window, RecoveryPanel *panel, const char *message)
{
    if (RecoveryOwner(window, panel) != NULL)
        gtk_label_set_text(GTK_LABEL(panel->message), message);
}
static void RecoveryAvailability(GtkWidget *window, RecoveryPanel *panel)
{
    GtkWidget *controls[] = {panel->save, panel->refresh, panel->load, panel->choice};
    for (size_t i = 0U; i < sizeof(controls) / sizeof(controls[0]); ++i)
        if (RecoveryOwner(window, panel) != NULL)
            gtk_widget_set_sensitive(controls[i], !panel->busy);
    if (RecoveryOwner(window, panel) != NULL)
        gtk_widget_set_sensitive(panel->restore, !panel->busy && panel->draft != NULL);
}
static void RecoveryChoiceChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object;
    (void)property;
    GtkWidget *window = g_object_ref(data);
    RecoveryPanel *panel = RecoveryState(window);
    if (RecoveryOwner(window, panel) != NULL)
    {
        if (panel->generation != UINT64_MAX)
            ++panel->generation;
        UmiDocumentRecoveryDraftDestroy(panel->draft);
        panel->draft = NULL;
        gtk_widget_set_sensitive(panel->restore, FALSE);
        if (RecoveryOwner(window, panel) != NULL)
            gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->preview)), "", 0);
    }
    g_object_unref(window);
}
/* Restore must always describe the visible preview. A contributed native
 * observer can change even a read-only text buffer, so retire its authority
 * when that happens instead of restoring unseen source. */
static void RecoveryPreviewChanged(GtkTextBuffer *buffer, gpointer data)
{
    (void)buffer;
    GtkWidget *window = g_object_ref(data);
    RecoveryPanel *panel = RecoveryState(window);
    if (RecoveryOwner(window, panel) != NULL && !panel->publishing_preview)
    {
        if (panel->generation != UINT64_MAX)
            ++panel->generation;
        UmiDocumentRecoveryDraftDestroy(panel->draft);
        panel->draft = NULL;
        gtk_widget_set_sensitive(panel->restore, FALSE);
    }
    g_object_unref(window);
}
static void RecoveryWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancellable)
{
    (void)source;
    (void)cancellable;
    RecoveryWork *work = data;
    work->status = UMI_STATUS_OK;
    if (umi_cancellation_token_is_requested(work->cancel))
        work->status = UMI_STATUS_CANCELLED;
    if (work->status == UMI_STATUS_OK && work->operation == RECOVERY_SAVE)
    {
        /* Only an explicit capture creates the private recovery directory.
         * No GUI object, live document or product runtime crosses this worker. */
        GFile *directory = g_file_new_for_path(work->directory);
        GError *error = NULL;
        if (!g_file_make_directory_with_parents(directory, NULL, &error) &&
            !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_EXISTS))
            work->status = UMI_STATUS_IO_ERROR;
        g_clear_error(&error);
        g_object_unref(directory);
        if (work->status == UMI_STATUS_OK)
            work->status = UmiDocumentRecoverySave(work->directory, work->draft, work->cancel);
    }
    else if (work->status == UMI_STATUS_OK && work->operation == RECOVERY_LIST)
        work->status = UmiDocumentRecoveryList(work->directory, work->cancel, &work->catalogue);
    else if (work->status == UMI_STATUS_OK)
        work->status = UmiDocumentRecoveryLoad(work->directory, work->key, work->cancel, &work->draft);
    g_task_return_boolean(task, TRUE);
}
static void RecoveryDone(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    (void)data;
    GTask *task = G_TASK(result);
    (void)g_task_propagate_boolean(task, NULL);
    RecoveryWork *work = g_task_get_task_data(task);
    g_task_set_task_data(task, NULL, NULL);
    GtkWidget *window = g_weak_ref_get(&work->window);
    RecoveryPanel *panel = window == NULL ? NULL : RecoveryState(window);
    if (RecoveryOwner(window, panel) != NULL && panel->pending == work)
    {
        UmiStatus status = work->status;
        if (status == UMI_STATUS_OK && work->operation == RECOVERY_LOAD &&
            work->generation != panel->generation)
            status = UMI_STATUS_CANCELLED;
        if (status == UMI_STATUS_OK && work->operation == RECOVERY_LIST)
        {
            UmiDocumentRecoveryCatalogueDestroy(panel->catalogue);
            panel->catalogue = work->catalogue;
            work->catalogue = NULL;
            GtkStringList *names = gtk_string_list_new(NULL);
            for (size_t i = 0U; i < UmiDocumentRecoveryCatalogueCount(panel->catalogue); ++i)
            {
                UmiDocumentRecoveryEntry entry;
                (void)UmiDocumentRecoveryCatalogueAt(panel->catalogue, i, &entry);
                GDateTime *time = g_date_time_new_from_unix_utc(
                    (gint64)(entry.modified_nanoseconds / UINT64_C(1000000000)));
                gchar *when = time == NULL ? g_strdup("Unknown time")
                                           : g_date_time_format(time, "%Y-%m-%d %H:%M:%S UTC");
                gchar *label =
                    g_strdup_printf("%s | %" PRIu64 " bytes | %.8s%s", when, entry.file_bytes, entry.key,
                                    entry.availability == UMI_STATUS_OK ? "" : " | unavailable");
                gtk_string_list_append(names, label);
                g_free(label);
                g_free(when);
                if (time != NULL)
                    g_date_time_unref(time);
            }
            /* The setter borrows the model and retains its own reference. */
            gtk_drop_down_set_model(GTK_DROP_DOWN(panel->choice), G_LIST_MODEL(names));
            g_object_unref(names);
            RecoveryMessage(window, panel,
                            UmiDocumentRecoveryCatalogueCount(panel->catalogue) == 0U
                                ? "No stored snapshots. Save a snapshot to protect the active draft."
                                : "Select a snapshot and choose Preview. No source file has been changed.");
        }
        else if (status == UMI_STATUS_OK && work->operation == RECOVERY_LOAD)
        {
            UmiDocumentRecoveryInfo info;
            const char *text = NULL;
            size_t bytes = 0U;
            (void)UmiDocumentRecoveryDraftInspect(work->draft, &info);
            (void)UmiDocumentRecoveryDraftRead(work->draft, &text, &bytes);
            /* The worker keeps source alive while native changed observers run.
             * Recheck generation after publication before enabling Restore. */
            panel->publishing_preview = 1;
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->preview));
            gtk_text_buffer_set_text(buffer, text, (gint)bytes);
            panel->publishing_preview = 0;
            int same = 0;
            if (RecoveryOwner(window, panel) != NULL)
            {
                GtkTextIter begin, end;
                gtk_text_buffer_get_bounds(buffer, &begin, &end);
                gchar *actual = gtk_text_buffer_get_text(buffer, &begin, &end, FALSE);
                same = strlen(actual) == bytes && memcmp(actual, text, bytes) == 0;
                g_free(actual);
            }
            if (RecoveryOwner(window, panel) != NULL && work->generation == panel->generation && same)
            {
                UmiDocumentRecoveryDraftDestroy(panel->draft);
                panel->draft = work->draft;
                work->draft = NULL;
                gchar *message =
                    g_strdup_printf("%s | %zu source bytes\nOriginal location: %s\nRestore opens a new "
                                    "unsaved tab. Choose Save As after reviewing it.",
                                    info.display_name, info.text_bytes,
                                    info.source_path[0] == '\0' ? "Unsaved document" : info.source_path);
                RecoveryMessage(window, panel, message);
                g_free(message);
            }
        }
        else if (status == UMI_STATUS_OK)
        {
            gchar *message = g_strdup_printf(
                "Snapshot saved locally in %s. Refresh to find it. Later edits need another snapshot.",
                work->directory);
            RecoveryMessage(window, panel, message);
            g_free(message);
        }
        else
        {
            gchar *message =
                g_strdup_printf("Recovery: %s. Existing source and snapshots were retained. Location: %s",
                                umi_status_text(status), work->directory);
            RecoveryMessage(window, panel, message);
            g_free(message);
        }
        panel->pending = NULL;
        panel->busy = 0;
        g_object_set_data(G_OBJECT(window), "umicom-recovery-pending", NULL);
        RecoveryAvailability(window, panel);
    }
    /* A retained, already closed window can still own panel state. Clear its
     * bookkeeping before freeing work; no control regains document authority. */
    if (panel != NULL && panel->pending == work)
    {
        panel->pending = NULL;
        g_object_set_data(G_OBJECT(window), "umicom-recovery-pending", NULL);
    }
    g_clear_object(&window);
    RecoveryWorkFree(work);
}
static void RecoveryStart(GtkWidget *window, RecoveryOperation operation)
{
    g_object_ref(window);
    RecoveryPanel *panel = RecoveryState(window);
    UmiGtk4Adapter *owner = RecoveryOwner(window, panel);
    if (owner == NULL || panel->busy || panel->generation == UINT64_MAX)
    {
        g_object_unref(window);
        return;
    }
    panel->busy = 1;
    RecoveryWork *work = g_try_new0(RecoveryWork, 1);
    UmiStatus status = work == NULL ? UMI_STATUS_OUT_OF_MEMORY : UMI_STATUS_OK;
    if (work != NULL)
    {
        g_weak_ref_init(&work->window, window);
        work->operation = operation;
        work->generation = panel->generation;
        memcpy(work->directory, panel->directory, sizeof(work->directory));
        status = umi_cancellation_token_create(&work->cancel);
    }
    if (status == UMI_STATUS_OK && operation == RECOVERY_SAVE)
    {
        UmiDocumentId document = 0U;
        status = EditFindDocument(owner, NULL, &document);
        if (status == UMI_STATUS_OK)
            status = UmiDocumentRecoveryKeyCreate(work->key, sizeof(work->key));
        if (status == UMI_STATUS_OK)
            status = UmiDocumentCoordinatorCaptureRecovery(owner->edit_coordinator, document, work->key,
                                                           work->cancel, &work->draft);
    }
    else if (status == UMI_STATUS_OK && operation == RECOVERY_LOAD)
    {
        UmiDocumentRecoveryEntry entry;
        guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(panel->choice));
        status = UmiDocumentRecoveryCatalogueAt(panel->catalogue, (size_t)selected, &entry);
        if (status == UMI_STATUS_OK)
            status = entry.availability;
        if (status == UMI_STATUS_OK)
            memcpy(work->key, entry.key, sizeof(work->key));
    }
    if (operation != RECOVERY_SAVE)
    {
        UmiDocumentRecoveryDraftDestroy(panel->draft);
        panel->draft = NULL;
    }
    if (status == UMI_STATUS_OK)
    {
        panel->pending = work;
        g_object_set_data(G_OBJECT(window), "umicom-recovery-pending", work);
        RecoveryAvailability(window, panel);
        if (RecoveryOwner(window, panel) != NULL)
        {
            GTask *task = g_task_new(NULL, NULL, RecoveryDone, NULL);
            g_task_set_task_data(task, work, NULL);
            g_task_run_in_thread(task, RecoveryWorker);
            g_object_unref(task);
            work = NULL;
        }
        else
            status = UMI_STATUS_CANCELLED;
    }
    if (status != UMI_STATUS_OK)
    {
        panel->pending = NULL;
        panel->busy = 0;
        g_object_set_data(G_OBJECT(window), "umicom-recovery-pending", NULL);
        gchar *message = g_strdup_printf(
            "Recovery: %s. Select an available snapshot to preview, or open a document to capture.",
            umi_status_text(status));
        RecoveryMessage(window, panel, message);
        g_free(message);
        RecoveryAvailability(window, panel);
    }
    if (work != NULL)
        RecoveryWorkFree(work);
    g_object_unref(window);
}
static void RecoverySaveClicked(GtkButton *button, gpointer window)
{
    (void)button;
    RecoveryStart(window, RECOVERY_SAVE);
}
static void RecoveryRefreshClicked(GtkButton *button, gpointer window)
{
    (void)button;
    RecoveryStart(window, RECOVERY_LIST);
}
static void RecoveryLoadClicked(GtkButton *button, gpointer window)
{
    (void)button;
    RecoveryStart(window, RECOVERY_LOAD);
}
static void RecoveryRestoreClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *window = g_object_ref(data);
    RecoveryPanel *panel = RecoveryState(window);
    UmiGtk4Adapter *owner = RecoveryOwner(window, panel);
    if (owner != NULL && !panel->busy && panel->draft != NULL)
    {
        panel->busy = 1;
        UmiDocumentId document = 0U;
        UmiStatus status =
            UmiDocumentCoordinatorRestoreRecovery(owner->edit_coordinator, panel->draft, &document);
        if (status == UMI_STATUS_OK)
        {
            GObject *lifetime = g_object_ref(panel->lifetime);
            RecoveryReviewClose(window);
            owner = g_object_get_data(lifetime, "adapter");
            if (owner != NULL && owner->window != NULL && UmiGtk4WindowIsOpen(owner->window))
                (void)EditFinished(owner, status);
            g_object_unref(lifetime);
        }
        else
        {
            panel->busy = 0;
            gchar *message = g_strdup_printf("Could not restore: %s. The snapshot remains available.",
                                             umi_status_text(status));
            RecoveryMessage(window, panel, message);
            g_free(message);
            RecoveryAvailability(window, panel);
        }
    }
    g_object_unref(window);
}
UmiStatus UmiGtk4AdapterReviewRecovery(UmiGtk4Adapter *adapter, const char *application, const char *base)
{
    if (adapter == NULL || application == NULL || application[0] == '\0')
        return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_lifetime == NULL || adapter->edit_coordinator == NULL || adapter->window == NULL)
        return UMI_STATUS_UNAVAILABLE;
    if (!UmiGtk4WindowIsOpen(adapter->window))
        return UMI_STATUS_CANCELLED;
    if (UmiGtk4AdapterCompletionReviewBusy(adapter) || UmiGtk4AdapterReplacementReviewBusy(adapter) ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) || UmiGtk4AdapterDocumentSaveAllBusy(adapter) ||
        adapter->edit_cancel != NULL)
        return UMI_STATUS_BUSY;
    UmiDocumentRecoveryScheduleInfo monitor_info;
    if (UmiGtk4AdapterRecoveryState(adapter, &monitor_info) == UMI_STATUS_OK && monitor_info.pending)
        return UMI_STATUS_BUSY;
    char directory[UMI_PATH_CAPACITY];
    UmiStatus status = UmiDocumentRecoveryDirectory(application, base, directory, sizeof(directory));
    if (status != UMI_STATUS_OK)
        return status;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkWindow *parent = g_object_ref(adapter->window);
    /* Reserve before creating a toplevel: creation observers can reenter the
     * application. Keep lifetime identities locally rather than trusting the
     * adapter after a native callback which might detach or destroy it. */
    g_object_set_data(lifetime, "recovery-creating", lifetime);
    GtkWidget *window = gtk_window_new();
    g_object_ref_sink(window);
    RecoveryPanel *panel = g_new0(RecoveryPanel, 1);
    panel->lifetime = g_object_ref(lifetime);
    panel->controls = g_ptr_array_new_with_free_func(g_object_unref);
    panel->generation = 1U;
    panel->application_directory = g_strdup(application);
    panel->base_override = g_strdup(base);
    memcpy(panel->directory, directory, sizeof(directory));
    g_object_set_data_full(G_OBJECT(window), "umicom-document-recovery", panel, RecoveryFree);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_window_set_child(GTK_WINDOW(window), box);
    GtkWidget *help =
        RecoveryControl(panel, box,
                        gtk_label_new("Recovery snapshots contain a local copy of your source. Save snapshot "
                                      "captures the active draft; it does not save the source file. Preview "
                                      "a stored snapshot before restoring it as a new unsaved tab."),
                        "document.recovery.help");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    panel->automatic =
        RecoveryControl(panel, box, gtk_button_new_with_label("Enable automatic snapshots (every minute)"),
                        "document.recovery.automatic");
    panel->automatic_status = RecoveryControl(panel, box, gtk_label_new("Automatic recovery is off."),
                                              "document.recovery.automatic-status");
    gtk_label_set_wrap(GTK_LABEL(panel->automatic_status), TRUE);
    g_signal_connect_object(panel->automatic, "clicked", G_CALLBACK(RecoveryAutomaticClicked), window, 0);
    panel->save = RecoveryControl(panel, box, gtk_button_new_with_label("Save snapshot of active draft"),
                                  "document.recovery.save");
    panel->refresh = RecoveryControl(panel, box, gtk_button_new_with_label("Refresh stored snapshots"),
                                     "document.recovery.refresh");
    panel->choice = RecoveryControl(panel, box, gtk_drop_down_new(NULL, NULL), "document.recovery.choice");
    panel->load = RecoveryControl(panel, box, gtk_button_new_with_label("Preview selected snapshot"),
                                  "document.recovery.load");
    GtkWidget *scroll = RecoveryControl(panel, box, gtk_scrolled_window_new(), "document.recovery.scroll");
    gtk_widget_set_vexpand(scroll, TRUE);
    panel->preview = gtk_text_view_new();
    g_ptr_array_add(panel->controls, g_object_ref_sink(panel->preview));
    (void)umi_gtk4_automation_tag_widget(panel->preview, "document.recovery.preview");
    gtk_text_view_set_editable(GTK_TEXT_VIEW(panel->preview), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(panel->preview), TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), panel->preview);
    panel->restore =
        RecoveryControl(panel, box, gtk_button_new_with_label("Restore preview as new unsaved tab"),
                        "document.recovery.restore");
    gtk_widget_set_sensitive(panel->restore, FALSE);
    GtkWidget *close =
        RecoveryControl(panel, box, gtk_button_new_with_label("Close"), "document.recovery.close");
    panel->message = RecoveryControl(panel, box, gtk_label_new(directory), "document.recovery.message");
    gtk_label_set_wrap(GTK_LABEL(panel->message), TRUE);
    g_signal_connect_object(panel->save, "clicked", G_CALLBACK(RecoverySaveClicked), window, 0);
    g_signal_connect_object(panel->refresh, "clicked", G_CALLBACK(RecoveryRefreshClicked), window, 0);
    g_signal_connect_object(panel->load, "clicked", G_CALLBACK(RecoveryLoadClicked), window, 0);
    g_signal_connect_object(panel->restore, "clicked", G_CALLBACK(RecoveryRestoreClicked), window, 0);
    g_signal_connect_object(panel->choice, "notify::selected", G_CALLBACK(RecoveryChoiceChanged), window, 0);
    g_signal_connect_object(gtk_text_view_get_buffer(GTK_TEXT_VIEW(panel->preview)), "changed",
                            G_CALLBACK(RecoveryPreviewChanged), window, 0);
    g_signal_connect_object(close, "clicked", G_CALLBACK(RecoveryCancel), window, 0);
    g_signal_connect(window, "close-request", G_CALLBACK(RecoveryClosing), NULL);
    g_signal_connect(window, "unmap", G_CALLBACK(RecoveryRemoved), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(RecoveryRemoved), NULL);
    (void)umi_gtk4_automation_tag_widget(window, "document.recovery.review");
    (void)UmiGtk4RecordingSetPrivate(window, 1);
    gtk_window_set_title(GTK_WINDOW(window), "Local draft recovery");
    gtk_window_set_default_size(GTK_WINDOW(window), 880, 680);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_default_widget(GTK_WINDOW(window), close);
    gtk_window_set_transient_for(GTK_WINDOW(window), parent);
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    g_object_set_data(lifetime, "recovery-creating", NULL);
    if (g_object_get_data(lifetime, "adapter") != adapter || !UmiGtk4WindowIsOpen(parent) ||
        !UmiGtk4WindowIsOpen(GTK_WINDOW(window)))
        status = UMI_STATUS_CANCELLED;
    else
    {
        g_object_set_data(lifetime, "recovery-review", window);
        RecoveryPanelMonitorChanged(lifetime);
        (void)UmiGtk4WatchWindowClosed(GTK_WINDOW(window), G_OBJECT(window), RecoveryRemoved, NULL);
        (void)UmiGtk4WatchWindowClosed(parent, G_OBJECT(window), RecoveryParentRemoved, window);
        g_signal_connect_object(parent, "destroy", G_CALLBACK(RecoveryParentRemoved), window, 0);
        if (RecoveryOwner(window, panel) != NULL)
            gtk_window_present(GTK_WINDOW(window));
        if (RecoveryOwner(window, panel) == NULL)
            status = UMI_STATUS_CANCELLED;
    }
    if (status != UMI_STATUS_OK)
        RecoveryReviewClose(window);
    g_object_unref(window);
    g_object_unref(parent);
    g_object_unref(lifetime);
    return status;
}
