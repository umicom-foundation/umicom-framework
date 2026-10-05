/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_observation_export.inc
 * PURPOSE: Write an explicit account report without retaining a window or live broker pointer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct ObservationWrite
{
    GWeakRef window;
    UmiCsvDocument *document;
    char path[UMI_PATH_CAPACITY];
    char account[64];
    UmiStatus status;
} ObservationWrite;
static void ObservationWriteFree(gpointer data)
{
    ObservationWrite *write = data;
    g_weak_ref_clear(&write->window);
    UmiCsvDocumentDestroy(write->document);
    g_free(write);
}
static void ObservationWriteWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    ObservationWrite *write = data;
    write->status = UmiIbkrObservationsWriteNew(write->document, write->path, NULL);
    g_task_return_boolean(task, TRUE);
}
/* Only report completion to the original still-owned window. The worker owns
 * the captured bytes, so reconnecting cannot change which account is exported. */
static void ObservationWriteDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    (void)context;
    ObservationWrite *write = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWindow *window = g_weak_ref_get(&write->window);
    if (window == NULL)
        return;
    ConnectionUi *ui = Owner(window);
    if (ui != NULL)
    {
        char *text = g_strdup_printf(
            "Account %s report: %s\n%s\n%s", write->account, umi_status_text(write->status), write->path,
            write->status == UMI_STATUS_OK
                ? "Saved captured observations. Check completeness and monotonic receipt times in the file."
                : "Existing files were not replaced. Inspect any partial new file; choose another path to "
                  "retry.");
        gtk_label_set_text(GTK_LABEL(ui->exportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
        {
            ui->exportBusy = FALSE;
            gtk_widget_set_sensitive(ui->exportReport, TRUE); /* Last mutation may reenter. */
        }
    }
    g_object_unref(window);
}
static void ObservationExportClicked(GtkButton *button, gpointer context)
{
    (void)button;
    GtkWindow *window = g_object_ref(GTK_WINDOW(context));
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || ui->exportBusy)
    {
        g_object_unref(window);
        return;
    }
    ui->exportBusy = TRUE;
    ObservationWrite *write = g_new0(ObservationWrite, 1);
    g_weak_ref_init(&write->window, window);
    const char *path = gtk_editable_get_text(GTK_EDITABLE(ui->exportPath));
    write->status = umi_text_copy(write->path, sizeof(write->path), path);
    if (write->status == UMI_STATUS_OK)
        write->status = UmiOutputFileValidatePath(write->path);
    /* Copy the latest owner state before formatting. No network pump or provider
     * request is hidden inside Export, and incomplete observations stay explicit. */
    if (write->status == UMI_STATUS_OK && ui->connection != NULL)
        write->status = UmiIbkrConnectionCopy(ui->connection, &ui->snapshot);
    if (write->status == UMI_STATUS_OK)
        write->status =
            UmiIbkrObservationsExportCsv(&ui->snapshot, UmiIbkrMonotonicMilliseconds(), &write->document);
    if (write->status == UMI_STATUS_OK)
        write->status = umi_text_copy(write->account, sizeof(write->account), ui->snapshot.selectedAccount);
    if (write->status != UMI_STATUS_OK)
    {
        char *text = g_strdup_printf(
            "Report not written: %s. Read a selected account first and choose a new absolute CSV path.",
            umi_status_text(write->status));
        gtk_label_set_text(GTK_LABEL(ui->exportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
            ui->exportBusy = FALSE;
        ObservationWriteFree(write);
        g_object_unref(window);
        return;
    }
    gtk_widget_set_sensitive(ui->exportReport, FALSE);
    ui = Owner(window);
    if (ui == NULL)
        goto abandoned;
    gtk_label_set_text(GTK_LABEL(ui->exportStatus), "Writing the captured selected-account report. Closing "
                                                    "this window does not undo the requested write.");
    if (Owner(window) == NULL)
        goto abandoned;
    GTask *task = g_task_new(NULL, NULL, ObservationWriteDone, NULL);
    g_task_set_task_data(task, write, ObservationWriteFree);
    g_task_run_in_thread(task, ObservationWriteWorker);
    g_object_unref(task);
    g_object_unref(window);
    return;
abandoned:
    ObservationWriteFree(write);
    g_object_unref(window);
}
static void ObservationExportControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *root)
{
    gtk_box_append(GTK_BOX(root), Text("Export received account observations"));
    gtk_box_append(GTK_BOX(root), Text("Choose a new private CSV file. Values and positions carry "
                                       "completeness flags and monotonic receipt times. These are separate "
                                       "provider responses, not a bank statement or atomic portfolio."));
    ui->exportPath = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ui->exportPath), UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->exportPath), "New absolute CSV path");
    gtk_widget_set_name(ui->exportPath, "ibkr-export-path");
    ui->exportReport = gtk_button_new_with_label("Write new account report");
    gtk_widget_set_name(ui->exportReport, "ibkr-export-report");
    ui->exportStatus = Text("No report has been written.");
    gtk_widget_set_name(ui->exportStatus, "ibkr-export-status");
    gtk_box_append(GTK_BOX(root), ui->exportPath);
    gtk_box_append(GTK_BOX(root), ui->exportReport);
    gtk_box_append(GTK_BOX(root), ui->exportStatus);
    g_signal_connect_object(ui->exportReport, "clicked", G_CALLBACK(ObservationExportClicked), window, 0);
}
