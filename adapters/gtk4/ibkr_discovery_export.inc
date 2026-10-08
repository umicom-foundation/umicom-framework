/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ibkr_discovery_export.inc
 * PURPOSE: Save frozen discovery evidence through the shared new-file writer.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void DiscoveryWriteDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    guint slot = GPOINTER_TO_UINT(context);
    ObservationWrite *write = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    GtkWindow *window = g_weak_ref_get(&write->window);
    if (!window)
        return;
    ConnectionUi *ui = Owner(window);
    if (ui)
    {
        gchar *message = g_strdup_printf(
            "Discovery report: %s\n%s\n%s", umi_status_text(write->status), write->path,
            write->status == UMI_STATUS_OK
                ? "Saved the frozen discovery records and completeness flags."
                : "Existing files were not replaced. Inspect any partial new file before retrying.");
        gtk_label_set_text(GTK_LABEL(ui->discoveryExportStatus[slot]), message);
        g_free(message);
        ui = Owner(window);
        if (ui)
        {
            ui->discoveryExportBusy[slot] = FALSE;
            gtk_widget_set_sensitive(ui->discoveryExportReport[slot], TRUE);
        }
    }
    g_object_unref(window);
}
static void DiscoveryExportClicked(GtkButton *button, gpointer data)
{
    GtkWindow *window = g_object_ref(GTK_WINDOW(data));
    ConnectionUi *ui = Owner(window);
    if (!ui)
    {
        g_object_unref(window);
        return;
    }
    guint slot = GTK_WIDGET(button) == ui->discoveryExportReport[1] ? 1U : 0U;
    if (ui->discoveryExportBusy[slot] || ui->fillBusy)
    {
        g_object_unref(window);
        return;
    }
    ui->discoveryExportBusy[slot] = TRUE;
    ObservationWrite *write = g_new0(ObservationWrite, 1);
    g_weak_ref_init(&write->window, window);
    write->status = umi_text_copy(write->path, sizeof write->path,
                                  gtk_editable_get_text(GTK_EDITABLE(ui->discoveryExportPath[slot])));
    if (write->status == UMI_STATUS_OK)
        write->status = UmiOutputFileValidatePath(write->path);
    if (write->status == UMI_STATUS_OK)
    {
        uint64_t now = UmiIbkrMonotonicMilliseconds();
        write->status =
            slot ? UmiIbkrOptionChainExportCsv(ui->connection, ui->optionRequest, now, &write->document)
                 : UmiIbkrScannerExportCsv(ui->connection, ui->scannerRequest, now, 60000U, &write->document);
    }
    if (write->status != UMI_STATUS_OK)
    {
        gchar *message = g_strdup_printf(
            "Report not written: %s. Request discovery first and choose a new absolute CSV path.",
            umi_status_text(write->status));
        gtk_label_set_text(GTK_LABEL(ui->discoveryExportStatus[slot]), message);
        g_free(message);
        ui = Owner(window);
        if (ui)
            ui->discoveryExportBusy[slot] = FALSE;
        ObservationWriteFree(write);
        g_object_unref(window);
        return;
    }
    gtk_widget_set_sensitive(ui->discoveryExportReport[slot], FALSE);
    ui = Owner(window);
    if (!ui)
        goto abandoned;
    gtk_label_set_text(
        GTK_LABEL(ui->discoveryExportStatus[slot]),
        "Writing captured discovery records. Closing this window does not undo the requested write.");
    if (!Owner(window))
        goto abandoned;
    /* The worker owns only the document and path. It cannot poll the broker,
     * alter subscriptions or observe a later selection in this window. */
    GTask *task = g_task_new(NULL, NULL, DiscoveryWriteDone, GUINT_TO_POINTER(slot));
    g_task_set_task_data(task, write, ObservationWriteFree);
    g_task_run_in_thread(task, ObservationWriteWorker);
    g_object_unref(task);
    g_object_unref(window);
    return;
abandoned:
    ObservationWriteFree(write);
    g_object_unref(window);
}
static void DiscoveryExportControls(GtkWindow *window, ConnectionUi *ui, GtkWidget *box, guint slot)
{
    const char *prefix = slot ? "ibkr-option" : "ibkr-scanner";
    ui->discoveryExportPath[slot] = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(ui->discoveryExportPath[slot]), UMI_PATH_CAPACITY - 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(ui->discoveryExportPath[slot]), "New absolute CSV path");
    ui->discoveryExportReport[slot] =
        gtk_button_new_with_label(slot ? "Write option definition report" : "Write scanner report");
    ui->discoveryExportStatus[slot] = Text("No discovery report has been written.");
    gchar *name = g_strdup_printf("%s-export-path", prefix);
    gtk_widget_set_name(ui->discoveryExportPath[slot], name);
    g_free(name);
    name = g_strdup_printf("%s-export-report", prefix);
    gtk_widget_set_name(ui->discoveryExportReport[slot], name);
    g_free(name);
    name = g_strdup_printf("%s-export-status", prefix);
    gtk_widget_set_name(ui->discoveryExportStatus[slot], name);
    g_free(name);
    gtk_box_append(GTK_BOX(box), Text("Export the retained request and result state to a new CSV file. "
                                      "Incomplete and stale evidence is labelled in the report."));
    gtk_box_append(GTK_BOX(box), ui->discoveryExportPath[slot]);
    gtk_box_append(GTK_BOX(box), ui->discoveryExportReport[slot]);
    gtk_box_append(GTK_BOX(box), ui->discoveryExportStatus[slot]);
    g_signal_connect_object(ui->discoveryExportReport[slot], "clicked", G_CALLBACK(DiscoveryExportClicked),
                            window, 0);
}
