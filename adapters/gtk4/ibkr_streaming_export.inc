/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_streaming_export.inc
 * PURPOSE: Save a frozen rolling-window report without borrowing a live broker on a worker thread.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Reuse the account report's byte writer and release routine. Its task data owns
 * the CSV and path; only the completion view differs for streaming bars. */
static void StreamingWriteDone(GObject *source, GAsyncResult *result, gpointer context)
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
        gchar *text = g_strdup_printf(
            "Streaming-bar report: %s\n%s\n%s", umi_status_text(write->status), write->path,
            write->status == UMI_STATUS_OK ? "Saved the rolling window, gaps, receipt times and stale flags."
                                           : "Existing files were not replaced. Inspect any partial new "
                                             "file; choose another path to retry.");
        gtk_label_set_text(GTK_LABEL(ui->streamExportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
        {
            ui->streamExportBusy = FALSE;
            gtk_widget_set_sensitive(ui->streamExportReport, TRUE);
        }
    }
    g_object_unref(window);
}
static void StreamingExportClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || ui->streamExportBusy)
    {
        g_object_unref(window);
        return;
    }
    ui->streamExportBusy = TRUE;
    ObservationWrite *write = g_new0(ObservationWrite, 1);
    g_weak_ref_init(&write->window, window);
    write->status = umi_text_copy(write->path, sizeof write->path,
                                  gtk_editable_get_text(GTK_EDITABLE(ui->streamExportPath)));
    if (write->status == UMI_STATUS_OK)
        write->status = UmiOutputFileValidatePath(write->path);
    if (write->status == UMI_STATUS_OK)
        write->status = UmiIbkrRealtimeExportCsv(ui->connection, ui->streamRequest,
                                                 UmiIbkrMonotonicMilliseconds(), 15000U, &write->document);
    if (write->status != UMI_STATUS_OK)
    {
        gchar *text = g_strdup_printf(
            "Report not written: %s. Request streaming bars first and choose a new absolute CSV path.",
            umi_status_text(write->status));
        gtk_label_set_text(GTK_LABEL(ui->streamExportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
            ui->streamExportBusy = FALSE;
        ObservationWriteFree(write);
        g_object_unref(window);
        return;
    }
    gtk_widget_set_sensitive(ui->streamExportReport, FALSE);
    ui = Owner(window);
    if (ui == NULL)
        goto abandoned;
    gtk_label_set_text(GTK_LABEL(ui->streamExportStatus),
                       "Writing the captured streaming-bar report. Closing this window does not undo the "
                       "requested write.");
    if (Owner(window) == NULL)
        goto abandoned;
    /* No live connection pointer crosses the worker boundary. Reconnection,
     * new UI selections and window destruction cannot alter the captured CSV. */
    GTask *task = g_task_new(NULL, NULL, StreamingWriteDone, NULL);
    g_task_set_task_data(task, write, ObservationWriteFree);
    g_task_run_in_thread(task, ObservationWriteWorker);
    g_object_unref(task);
    g_object_unref(window);
    return;
abandoned:
    ObservationWriteFree(write);
    g_object_unref(window);
}
static void StreamingExportControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *box)
{
    gtk_box_append(GTK_BOX(box), Text("Save the captured streaming bars with their contract, interval and "
                                      "freshness flags. Existing files are never replaced."));
    ui->streamExportPath = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ui->streamExportPath), UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->streamExportPath), "New absolute CSV path");
    ui->streamExportReport = gtk_button_new_with_label("Write new streaming-bar report");
    ui->streamExportStatus = Text("No streaming-bar report has been written.");
    gtk_widget_set_name(ui->streamExportPath, "ibkr-stream-export-path");
    gtk_widget_set_name(ui->streamExportReport, "ibkr-stream-export-report");
    gtk_widget_set_name(ui->streamExportStatus, "ibkr-stream-export-status");
    gtk_box_append(GTK_BOX(box), ui->streamExportPath);
    gtk_box_append(GTK_BOX(box), ui->streamExportReport);
    gtk_box_append(GTK_BOX(box), ui->streamExportStatus);
    g_signal_connect_object(ui->streamExportReport, "clicked", G_CALLBACK(StreamingExportClicked), window, 0);
}
