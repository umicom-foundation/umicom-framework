/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_completed_export.inc
 * PURPOSE: Save an immutable completed-order report without retaining broker or window ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Reuse the account report's byte writer and release routine. Its task data owns
 * the CSV and path; only the completion view differs for completed orders. */
static void CompletedWriteDone(GObject *source, GAsyncResult *result, gpointer context)
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
            "Completed-order report: %s\n%s\n%s", umi_status_text(write->status), write->path,
            write->status == UMI_STATUS_OK ? "Saved the captured scope, completeness and receipt times. This "
                                             "is not a complete trading statement."
                                           : "Existing files were not replaced. Inspect any partial new "
                                             "file; choose another path to retry.");
        gtk_label_set_text(GTK_LABEL(ui->completedExportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
        {
            ui->completedExportBusy = FALSE;
            gtk_widget_set_sensitive(ui->completedExportReport, TRUE);
        }
    }
    g_object_unref(window);
}
static void CompletedExportClicked(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    ConnectionUi *ui = Owner(window);
    if (ui == NULL || ui->completedExportBusy)
    {
        g_object_unref(window);
        return;
    }
    ui->completedExportBusy = TRUE;
    ObservationWrite *write = g_new0(ObservationWrite, 1);
    g_weak_ref_init(&write->window, window);
    write->status = umi_text_copy(write->path, sizeof write->path,
                                  gtk_editable_get_text(GTK_EDITABLE(ui->completedExportPath)));
    if (write->status == UMI_STATUS_OK)
        write->status = UmiOutputFileValidatePath(write->path);
    if (write->status == UMI_STATUS_OK)
        write->status = UmiIbkrCompletedOrdersExportCsv(ui->connection, UmiIbkrMonotonicMilliseconds(),
                                                        15000U, &write->document);
    if (write->status != UMI_STATUS_OK)
    {
        gchar *text = g_strdup_printf(
            "Report not written: %s. Request completed orders first and choose a new absolute CSV path.",
            umi_status_text(write->status));
        gtk_label_set_text(GTK_LABEL(ui->completedExportStatus), text);
        g_free(text);
        ui = Owner(window);
        if (ui != NULL)
            ui->completedExportBusy = FALSE;
        ObservationWriteFree(write);
        g_object_unref(window);
        return;
    }
    gtk_widget_set_sensitive(ui->completedExportReport, FALSE);
    ui = Owner(window);
    if (ui == NULL)
        goto abandoned;
    gtk_label_set_text(GTK_LABEL(ui->completedExportStatus),
                       "Writing the captured completed-order report. Closing this window does not undo the "
                       "requested write.");
    if (Owner(window) == NULL)
        goto abandoned;
    /* No live connection pointer crosses the worker boundary. Reconnection,
     * new UI selections and window destruction cannot alter the captured CSV. */
    GTask *task = g_task_new(NULL, NULL, CompletedWriteDone, NULL);
    g_task_set_task_data(task, write, ObservationWriteFree);
    g_task_run_in_thread(task, ObservationWriteWorker);
    g_object_unref(task);
    g_object_unref(window);
    return;
abandoned:
    ObservationWriteFree(write);
    g_object_unref(window);
}
static void CompletedExportControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *box)
{
    gtk_box_append(GTK_BOX(box), Text("Save a private completed-order CSV report. It includes all accounts "
                                      "in the captured scope, even when a different account is selected "
                                      "above. Partial and old evidence "
                                      "keeps its original flags. Existing files are never replaced."));
    ui->completedExportPath = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ui->completedExportPath), UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->completedExportPath), "New absolute CSV path");
    ui->completedExportReport = gtk_button_new_with_label("Write new completed-order report");
    ui->completedExportStatus = Text("No completed-order report has been written.");
    gtk_widget_set_name(ui->completedExportPath, "ibkr-completed-export-path");
    gtk_widget_set_name(ui->completedExportReport, "ibkr-completed-export-report");
    gtk_widget_set_name(ui->completedExportStatus, "ibkr-completed-export-status");
    gtk_box_append(GTK_BOX(box), ui->completedExportPath);
    gtk_box_append(GTK_BOX(box), ui->completedExportReport);
    gtk_box_append(GTK_BOX(box), ui->completedExportStatus);
    g_signal_connect_object(ui->completedExportReport, "clicked", G_CALLBACK(CompletedExportClicked), window,
                            0);
}
