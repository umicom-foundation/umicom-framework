/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_tools_gtk4.inc
 * PURPOSE: Review captured developer tool file locations on a worker without launching them.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct DeveloperToolsRead
{
    UmiBuildProfile profile;
    UmiCancellationToken *cancel;
    UmiProjectToolCatalogue *catalogue;
    UmiStatus status;
} DeveloperToolsRead;
static void DeveloperToolsFree(gpointer data)
{
    DeveloperToolsRead *read = data;
    UmiProjectToolCatalogueDestroy(read->catalogue);
    umi_cancellation_token_destroy(read->cancel);
    g_free(read);
}
static void DeveloperToolsInvalidate(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    dialog->toolsInvalidated = 1;
    umi_cancellation_token_request(dialog->toolsCancel);
    if (!dialog->toolsLoading && !dialog->toolsChanging)
    {
        /* Label notifications can invoke application extensions. Remember every
         * edit, but avoid recursively publishing the same explanatory label. */
        dialog->toolsChanging = 1;
        gtk_label_set_text(GTK_LABEL(dialog->toolsDetail),
                           "Tool choices changed. Inspect the current files again.");
        dialog->toolsChanging = 0;
    }
    DeveloperRelease(dialog);
}
static void DeveloperToolsUnmapped(GtkWidget *widget, gpointer context)
{
    (void)widget;
    UmiGtk4DeveloperDialog *dialog = context;
    /* Signal ownership keeps the controller alive through this notification.
     * Hiding the form retires this capture even if it is shown again later. */
    dialog->toolsInvalidated = 1;
    umi_cancellation_token_request(dialog->toolsCancel);
}
static void DeveloperToolsWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    DeveloperToolsRead *read = data;
    read->status = UmiProjectToolCatalogueRead(&read->profile, read->cancel, &read->catalogue);
    g_task_return_boolean(task, TRUE);
}
static void DeveloperToolsDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context;
    DeveloperToolsRead *read = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    dialog->toolsCancel = NULL;
    if (DeveloperVisible(dialog))
    {
        GString *text = g_string_new(NULL);
        if (dialog->toolsInvalidated)
            g_string_append(
                text,
                "Tool choices or form visibility changed. Inspect again for current locations.");
        else if (read->status != UMI_STATUS_OK)
            g_string_append_printf(text, "Tool inspection: %s. No program was started.",
                                   umi_status_text(read->status));
        else
        {
            g_string_append(text,
                            "Captured tool files. No executable or project script was run.\n");
            for (size_t index = 0U; index < UmiProjectToolCatalogueCount(read->catalogue); ++index)
            {
                UmiProjectToolFile file;
                if (UmiProjectToolCatalogueAt(read->catalogue, index, &file) != UMI_STATUS_OK)
                    break;
                g_string_append_printf(text, "\n%s: ", UmiProjectToolName(file.kind));
                if (file.selection == UMI_PROJECT_TOOL_AUTOMATIC)
                    g_string_append(text, "CMake chooses the compiler during configuration.");
                else if (file.status == UMI_STATUS_OK)
                    g_string_append_printf(
                        text, "%s\n  File exists; executable compatibility is unverified.",
                        file.resolved);
                else
                    g_string_append_printf(text, "%s (%s)\n  Selection: %s.",
                                           umi_status_text(file.status), file.requested,
                                           file.selection == UMI_PROJECT_TOOL_FROM_DIRECTORY
                                               ? "project tools folder only"
                                               : "inherited PATH or explicit file");
            }
            g_string_append(
                text, "\n\nNot every project needs every tool. Review CMake's configured compiler, "
                      "generator and presets before building. Debugging and language support "
                      "require compatible adapters.");
        }
        gtk_label_set_text(GTK_LABEL(dialog->toolsDetail), text->str);
        g_string_free(text, TRUE);
        /* A label notification may edit inputs or dispose the dialog. Keep the
         * capture fenced until all publication notifications have returned. */
        if (DeveloperVisible(dialog) && dialog->toolsInvalidated)
            gtk_label_set_text(
                GTK_LABEL(dialog->toolsDetail),
                "Tool choices or form visibility changed. Inspect again for current locations.");
    }
    dialog->toolsLoading = 0;
    if (DeveloperVisible(dialog))
        gtk_widget_set_sensitive(dialog->toolsRead, TRUE);
    DeveloperRelease(dialog);
}
static void DeveloperToolsStart(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (dialog->toolsLoading || dialog->toolsChanging || dialog->accepting)
    {
        DeveloperRelease(dialog);
        return;
    }
    DeveloperToolsRead *read = g_try_new0(DeveloperToolsRead, 1U);
    if (read == NULL)
    {
        gtk_label_set_text(GTK_LABEL(dialog->toolsDetail),
                           "Not enough memory to inspect tool files.");
        DeveloperRelease(dialog);
        return;
    }
    read->profile = dialog->profile;
    read->status = umi_cancellation_token_create(&read->cancel);
    if (read->status == UMI_STATUS_OK)
        read->status =
            umi_text_copy(read->profile.tool_directory, sizeof read->profile.tool_directory,
                          gtk_editable_get_text(GTK_EDITABLE(dialog->fields[11])));
    if (read->status == UMI_STATUS_OK)
        read->status = umi_text_copy(read->profile.compiler, sizeof read->profile.compiler,
                                     gtk_editable_get_text(GTK_EDITABLE(dialog->fields[4])));
    if (read->status != UMI_STATUS_OK)
    {
        gtk_label_set_text(GTK_LABEL(dialog->toolsDetail), umi_status_text(read->status));
        DeveloperToolsFree(read);
        DeveloperRelease(dialog);
        return;
    }
    dialog->toolsLoading = 1;
    dialog->toolsInvalidated = 0;
    dialog->toolsCancel = read->cancel;
    gtk_widget_set_sensitive(dialog->toolsRead, FALSE);
    if (DeveloperVisible(dialog))
        gtk_label_set_text(GTK_LABEL(dialog->toolsDetail), "Inspecting selected tool files...");
    if (!DeveloperVisible(dialog))
    {
        dialog->toolsLoading = 0;
        dialog->toolsCancel = NULL;
        DeveloperToolsFree(read);
        DeveloperRelease(dialog);
        return;
    }
    GTask *task = g_task_new(NULL, NULL, DeveloperToolsDone, dialog);
    g_task_set_task_data(task, read, DeveloperToolsFree);
    g_task_run_in_thread(task, DeveloperToolsWorker);
    g_object_unref(task);
    /* Completion owns the acquired controller reference from this point. */
}
static void DeveloperToolsControls(UmiGtk4DeveloperDialog *dialog)
{
    dialog->toolsRead = gtk_button_new_with_label("Inspect developer tool files");
    dialog->toolsDetail = gtk_label_new(
        "Inspect the selected tool folder or inherited PATH without running any tools.");
    gtk_label_set_wrap(GTK_LABEL(dialog->toolsDetail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(dialog->toolsDetail), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(dialog->toolsDetail), TRUE);
    (void)umi_gtk4_automation_tag_widget(dialog->toolsRead, "developer.tools.inspect");
    (void)umi_gtk4_automation_tag_widget(dialog->toolsDetail, "developer.tools.detail");
    gtk_box_append(GTK_BOX(dialog->body), dialog->toolsRead);
    gtk_box_append(GTK_BOX(dialog->body), dialog->toolsDetail);
    DeveloperConnect(dialog, G_OBJECT(dialog->toolsRead), "clicked",
                     G_CALLBACK(DeveloperToolsStart));
    DeveloperConnect(dialog, G_OBJECT(dialog->fields[4]), "changed",
                     G_CALLBACK(DeveloperToolsInvalidate));
    DeveloperConnect(dialog, G_OBJECT(dialog->fields[11]), "changed",
                     G_CALLBACK(DeveloperToolsInvalidate));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "unmap", G_CALLBACK(DeveloperToolsUnmapped));
}
