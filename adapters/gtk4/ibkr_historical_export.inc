/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_historical_export.inc
 * PURPOSE: Save an owned historical-bar report through the existing non-overwriting worker.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Reuse the account report's byte writer and release routine. Its task data owns
 * the CSV and path; only the completion view differs for historical bars. */
static void HistoricalWriteDone(GObject *source, GAsyncResult *result, gpointer context)
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
            "Historical-bar report: %s\n%s\n%s", umi_status_text(write->status), write->path,
            write->status == UMI_STATUS_OK ? "Saved the captured scope, completeness and receipt times. This "
                                             "is historical data, not a live feed."
                                           : "Existing files were not replaced. Inspect any partial new "
                                             "file; choose another path to retry.");
        gtk_label_set_text(GTK_LABEL(ui->historyExportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
        {
            ui->historyExportBusy = FALSE;
            gtk_widget_set_sensitive(ui->historyExportReport, TRUE);
        }
    }
    g_object_unref(window);
}
static void HistoricalExportClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || ui->historyExportBusy)
    {
        g_object_unref(window);
        return;
    }
    ui->historyExportBusy = TRUE;
    ObservationWrite *write = g_new0(ObservationWrite, 1);
    g_weak_ref_init(&write->window, window);
    write->status = umi_text_copy(write->path, sizeof write->path,
                                  gtk_editable_get_text(GTK_EDITABLE(ui->historyExportPath)));
    if (write->status == UMI_STATUS_OK)
        write->status = UmiOutputFileValidatePath(write->path);
    if (write->status == UMI_STATUS_OK)
        write->status = UmiIbkrHistoricalExportCsv(ui->connection, ui->historyRequest,
                                                   UmiIbkrMonotonicMilliseconds(), 300000U, &write->document);
    if (write->status != UMI_STATUS_OK)
    {
        gchar *text = g_strdup_printf(
            "Report not written: %s. Request historical bars first and choose a new absolute CSV path.",
            umi_status_text(write->status));
        gtk_label_set_text(GTK_LABEL(ui->historyExportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
            ui->historyExportBusy = FALSE;
        ObservationWriteFree(write);
        g_object_unref(window);
        return;
    }
    gtk_widget_set_sensitive(ui->historyExportReport, FALSE);
    ui = Owner(window);
    if (ui == NULL)
        goto abandoned;
    gtk_label_set_text(GTK_LABEL(ui->historyExportStatus),
                       "Writing the captured historical-bar report. Closing this window does not undo the "
                       "requested write.");
    if (Owner(window) == NULL)
        goto abandoned;
    /* No live connection pointer crosses the worker boundary. Reconnection,
     * new UI selections and window destruction cannot alter the captured CSV. */
    GTask *task = g_task_new(NULL, NULL, HistoricalWriteDone, NULL);
    g_task_set_task_data(task, write, ObservationWriteFree);
    g_task_run_in_thread(task, ObservationWriteWorker);
    g_object_unref(task);
    g_object_unref(window);
    return;
abandoned:
    ObservationWriteFree(write);
    g_object_unref(window);
}
static void HistoricalExportControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *box)
{
    gtk_box_append(GTK_BOX(box), Text("Save the captured historical bars with their contract, interval and "
                                      "freshness flags. Existing files are never replaced."));
    ui->historyExportPath = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ui->historyExportPath), UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->historyExportPath), "New absolute CSV path");
    ui->historyExportReport = gtk_button_new_with_label("Write new historical-bar report");
    ui->historyExportStatus = Text("No historical-bar report has been written.");
    gtk_widget_set_name(ui->historyExportPath, "ibkr-history-export-path");
    gtk_widget_set_name(ui->historyExportReport, "ibkr-history-export-report");
    gtk_widget_set_name(ui->historyExportStatus, "ibkr-history-export-status");
    gtk_box_append(GTK_BOX(box), ui->historyExportPath);
    gtk_box_append(GTK_BOX(box), ui->historyExportReport);
    gtk_box_append(GTK_BOX(box), ui->historyExportStatus);
    g_signal_connect_object(ui->historyExportReport, "clicked", G_CALLBACK(HistoricalExportClicked), window,
                            0);
}
