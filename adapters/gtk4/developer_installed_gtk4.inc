/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_installed_gtk4.inc
 * PURPOSE: Review installed files asynchronously before copying an explicit program choice.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct DeveloperInstalledRead
{
    UmiBuildProfile profile;
    UmiProjectInstalledFiles *catalogue;
    const UmiProjectInstalledFiles *selection;
    size_t index;
    UmiStatus status;
    UmiCancellationToken *cancel;
} DeveloperInstalledRead;
static void DeveloperInstalledFree(gpointer data)
{
    DeveloperInstalledRead *read = data;
    UmiProjectInstalledFilesDestroy(read->catalogue);
    umi_cancellation_token_destroy(read->cancel);
    g_free(read);
}
/* Invalidation records user edits even if the original text is restored before
 * the worker returns. It changes no GTK state, so model notifications cannot
 * recursively publish a second catalogue or overwrite an edited program. */
static void DeveloperInstalledInvalidate(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    dialog->installedInvalidated = 1;
    umi_cancellation_token_request(dialog->installedCancel);
    DeveloperRelease(dialog);
}
static void DeveloperInstalledWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    DeveloperInstalledRead *read = data;
    if (read->status == UMI_STATUS_OK)
    {
        if (read->selection != NULL)
            read->status = UmiProjectInstalledFilesSelect(read->selection, read->index, &read->profile,
                                                          read->cancel, &read->profile);
        else
            read->status = UmiProjectInstalledFilesRead(&read->profile, read->cancel, &read->catalogue);
    }
    g_task_return_boolean(task, TRUE);
}
/* A worker captures a catalogue index, not a visible row number. Sorting may
 * move that row while validation runs; hiding it or choosing another file must
 * refuse the late result. Keep all native-to-domain index translation here. */
static bool DeveloperInstalledSelectionCurrent(UmiGtk4DeveloperDialog *dialog,
                                                const DeveloperInstalledRead *read)
{
    size_t source = SIZE_MAX;
    return UmiGtk4FilteredChoicesSelectedSource(dialog->installedFilter, &source) == UMI_STATUS_OK &&
           source == read->index;
}
/* The request owns a controller reference until completion. Closing or destroying
 * the form hides late results; retained buttons cannot revive a retired owner.
 * Keep the busy flag set through every GTK notification during publication. */
static void DeveloperInstalledDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context;
    DeveloperInstalledRead *read = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    dialog->installedCancel = NULL;
    if (!DeveloperVisible(dialog))
        goto done;
    if (dialog->installedInvalidated)
    {
        gtk_label_set_text(GTK_LABEL(dialog->installedDetail),
                           "The folders or program changed during review. Read installed files again. No "
                           "program was copied.");
    }
    else if (read->status != UMI_STATUS_OK)
    {
        char *text = g_strdup_printf(
            "Install review: %s. Choose the actual build and install folders, "
            "complete a full installation successfully, then read again. The previous program is unchanged.",
            umi_status_text(read->status));
        gtk_label_set_text(GTK_LABEL(dialog->installedDetail), text);
        g_free(text);
    }
/* Resolve the selected source identity after asynchronous validation; visible order may have changed.
 * The former implementation remains below for engineering review. */
#if 0
    else if (read->selection != NULL &&
             (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->installedPicker)) != read->index)
#endif
    else if (read->selection != NULL && !DeveloperInstalledSelectionCurrent(dialog, read))
    {
        gtk_label_set_text(
            GTK_LABEL(dialog->installedDetail),
            "The selected row changed during review. Select the intended program again; nothing was copied.");
    }
    else if (read->selection != NULL)
    {
        /* One field changes, after the worker has checked the recorded file.
         * Acceptance remains a separate action and does not inherit trust. */
        gtk_editable_set_text(GTK_EDITABLE(dialog->fields[8]), read->profile.run_program);
        if (!DeveloperVisible(dialog))
            goto done;
        if (strcmp(gtk_editable_get_text(GTK_EDITABLE(dialog->fields[8])), read->profile.run_program) != 0)
        {
            gtk_label_set_text(GTK_LABEL(dialog->installedDetail),
                               "The program field changed while copying the selection. Review its current "
                               "value before applying settings.");
            goto done;
        }
        gtk_label_set_text(
            GTK_LABEL(dialog->installedDetail),
            "Copied the installed program into Program to run. Review its arguments and working folder, "
            "then Apply Settings separately. Nothing was launched. Read again before another selection.");
    }
    else
    {
        UmiProjectInstalledSummary summary;
        (void)UmiProjectInstalledFilesSummary(read->catalogue, &summary);
        GtkStringList *names = gtk_string_list_new(NULL);
        for (size_t i = 0U; i < summary.count; ++i)
        {
            UmiProjectInstalledFile file;
            (void)UmiProjectInstalledFilesAt(read->catalogue, i, &file);
            char *text = g_strdup_printf("%s | %s", file.path,
                                         file.inspection_status != UMI_STATUS_OK
                                             ? umi_status_text(file.inspection_status)
                                         : file.program_candidate ? "program candidate"
                                                                  : "installed entry");
            gtk_string_list_append(names, text);
            g_free(text);
        }
/* Shared filtered choices replace direct list publication so display order cannot change file identity.
 * The former implementation remains below for engineering review. */
#if 0
        dialog->installedCatalogue = read->catalogue;
        read->catalogue = NULL;
        gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->installedPicker), G_LIST_MODEL(names));
        g_object_unref(names); /* The setter retains its model. */
#endif
        /* Publish copied labels through Framework's common projection before
         * transferring file evidence. Busy guards prevent callbacks from using
         * the new mapping until the matching catalogue belongs to this form. */
        const char **rows = g_new(const char *, summary.count + 1U);
        for (size_t i = 0U; i < summary.count; ++i)
            rows[i] = gtk_string_list_get_string(names, (guint)i);
        UmiStatus mapped = UmiGtk4FilteredChoicesSetRows(dialog->installedFilter, rows, summary.count);
        g_free(rows);
        g_object_unref(names);
        if (mapped != UMI_STATUS_OK)
        {
            if (DeveloperVisible(dialog))
                gtk_label_set_text(GTK_LABEL(dialog->installedDetail),
                    "Installed paths could not be displayed within the text limits. Manual program settings remain available.");
            goto done;
        }
        dialog->installedCatalogue = read->catalogue;
        read->catalogue = NULL;
        if (!DeveloperVisible(dialog))
            goto done;
        gtk_drop_down_set_selected(GTK_DROP_DOWN(dialog->installedPicker), GTK_INVALID_LIST_POSITION);
        if (!DeveloperVisible(dialog))
            goto done;
        char *text = g_strdup_printf(
            "Read %zu installed paths under %s. Select a row to inspect it. "
            "This manifest describes a full install; it does not prove the application can launch.",
            summary.count, summary.install_directory);
        gtk_label_set_text(GTK_LABEL(dialog->installedDetail), text);
        g_free(text);
        if (!DeveloperVisible(dialog))
            goto done;
        if (dialog->installedInvalidated)
            gtk_label_set_text(GTK_LABEL(dialog->installedDetail),
                               "Inputs changed while publishing the catalogue. Read installed files again.");
    }
done:
    dialog->installedLoading = 0;
    if (DeveloperVisible(dialog))
    {
        /* Re-enabling is the last mutation: a sensitivity callback may begin
         * another request. The completed request no longer touches its state. */
        gtk_widget_set_sensitive(dialog->installedRead, TRUE);
    }
    DeveloperRelease(dialog);
}
static UmiStatus DeveloperInstalledInputs(UmiGtk4DeveloperDialog *dialog, UmiBuildProfile *out)
{
    *out = dialog->profile;
    UmiStatus status = umi_text_copy(out->source_directory, sizeof(out->source_directory),
                                     gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(out->build_directory, sizeof(out->build_directory),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[2])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(out->install_directory, sizeof(out->install_directory),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[10])));
    return status;
}
static void DeveloperInstalledStart(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (dialog->installedLoading || dialog->installedChanging || DeveloperTargetBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    bool selecting = GTK_WIDGET(button) == dialog->installedProgram;
    if (selecting && (dialog->installedInvalidated || dialog->installedCatalogue == NULL))
    {
        dialog->installedChanging = 1;
        gtk_label_set_text(GTK_LABEL(dialog->installedDetail),
                           "Read installed files before selecting a program.");
        dialog->installedChanging = 0;
        DeveloperRelease(dialog);
        return;
    }
    DeveloperInstalledRead *read = g_new0(DeveloperInstalledRead, 1);
    read->status = DeveloperInstalledInputs(dialog, &read->profile);
    if (read->status == UMI_STATUS_OK)
        read->status = umi_cancellation_token_create(&read->cancel);
    if (selecting)
    {
        read->selection = dialog->installedCatalogue;
/* Resolve the selected file before starting validation; an absent or externally replaced model cannot select a program.
 * The former implementation remains below for engineering review. */
#if 0
        read->index = (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->installedPicker));
#endif
        if (read->status == UMI_STATUS_OK)
            read->status = UmiGtk4FilteredChoicesSelectedSource(dialog->installedFilter, &read->index);
    }
    else
    {
        UmiProjectInstalledFilesDestroy(dialog->installedCatalogue);
        dialog->installedCatalogue = NULL;
    }
    dialog->installedInvalidated = 0;
    dialog->installedLoading = 1;
    dialog->installedCancel = read->cancel;
    gtk_widget_set_sensitive(dialog->installedRead, FALSE);
    if (!DeveloperVisible(dialog))
        goto abandoned;
/* A fresh manifest read retires both visible rows and their source mapping.
 * The former implementation remains below for engineering review. */
#if 0
    if (!selecting)
        gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->installedPicker), NULL);
#endif
    if (!selecting)
        (void)UmiGtk4FilteredChoicesSetRows(dialog->installedFilter, NULL, 0U);
    if (!DeveloperVisible(dialog))
        goto abandoned;
    gtk_label_set_text(GTK_LABEL(dialog->installedDetail),
                       selecting ? "Checking the manifest and selected program before copying its path…"
                                 : "Reading the full-install manifest and inspecting installed files…");
    if (!DeveloperVisible(dialog))
        goto abandoned;
    GTask *task = g_task_new(NULL, NULL, DeveloperInstalledDone, dialog);
    g_task_set_task_data(task, read, DeveloperInstalledFree);
    g_task_run_in_thread(task, DeveloperInstalledWorker);
    g_object_unref(task);
    return;
abandoned:
    dialog->installedCancel = NULL;
    dialog->installedLoading = 0;
    DeveloperInstalledFree(read);
    DeveloperRelease(dialog);
}
static void DeveloperInstalledDetails(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object;
    (void)property;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (dialog->installedLoading || dialog->installedChanging || DeveloperTargetBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->installedChanging = 1;
    UmiProjectInstalledFile file;
/* Details and program selection must read the same captured file after filtering or sorting.
 * The former implementation remains below for engineering review. */
#if 0
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->installedPicker));
    if (UmiProjectInstalledFilesAt(dialog->installedCatalogue, (size_t)index, &file) == UMI_STATUS_OK)
#endif
    size_t index = 0U;
    if (UmiGtk4FilteredChoicesSelectedSource(dialog->installedFilter, &index) == UMI_STATUS_OK &&
        UmiProjectInstalledFilesAt(dialog->installedCatalogue, index, &file) == UMI_STATUS_OK)
    {
        char *text = g_strdup_printf(
            "%s\nInspection: %s | Bytes: %" G_GUINT64_FORMAT "\n%s", file.path,
            umi_status_text(file.inspection_status), (guint64)file.size,
            dialog->installedInvalidated ? "Inputs changed. Read installed files again."
            : file.program_candidate ? "Use installed program rechecks the file, then copies its path only."
                                     : "This entry is not an executable regular-file candidate. It cannot be "
                                       "selected as a program.");
        gtk_label_set_text(GTK_LABEL(dialog->installedDetail), text);
        g_free(text);
    }
    else if (DeveloperVisible(dialog))
        gtk_label_set_text(GTK_LABEL(dialog->installedDetail),
            "No installed file is selected. Choose a visible row, or clear and apply the filter to see all captured files.");
    dialog->installedChanging = 0;
    DeveloperRelease(dialog);
}
static void DeveloperInstalledControls(UmiGtk4DeveloperDialog *dialog)
{
    dialog->installedRead = gtk_button_new_with_label("Read installed files");
    dialog->installedPicker = gtk_drop_down_new(NULL, NULL);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dialog->installedPicker), TRUE);
    dialog->installedFilter = UmiGtk4FilteredChoicesCreate(GTK_DROP_DOWN(dialog->installedPicker));
    (void)umi_gtk4_automation_tag_widget(dialog->installedFilter, "developer.installed.filter");
    gtk_widget_set_tooltip_text(dialog->installedFilter,
        "Find text in installed paths or inspection labels, then apply the filter. Sorting changes display order only.");
    gtk_box_append(GTK_BOX(dialog->body), dialog->installedFilter);
    dialog->installedProgram = gtk_button_new_with_label("Use installed program");
    dialog->installedDetail = gtk_label_new(
        "After Install succeeds, review its files here. "
        "Choose the actual build and install folders above; presets and staging folders are not inferred.");
    gtk_label_set_wrap(GTK_LABEL(dialog->installedDetail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(dialog->installedDetail), 0.0F);
    GtkWidget *widgets[] = {dialog->installedRead, dialog->installedPicker, dialog->installedProgram,
                            dialog->installedDetail};
    const char *ids[] = {"developer.installed.read", "developer.installed.choices",
                         "developer.installed.program", "developer.installed.detail"};
    for (size_t i = 0U; i < sizeof(widgets) / sizeof(widgets[0]); ++i)
    {
        (void)umi_gtk4_automation_tag_widget(widgets[i], ids[i]);
        gtk_box_append(GTK_BOX(dialog->body), widgets[i]);
    }
    DeveloperConnect(dialog, G_OBJECT(dialog->installedRead), "clicked", G_CALLBACK(DeveloperInstalledStart));
    DeveloperConnect(dialog, G_OBJECT(dialog->installedProgram), "clicked",
                     G_CALLBACK(DeveloperInstalledStart));
    DeveloperConnect(dialog, G_OBJECT(dialog->installedPicker), "notify::selected",
                     G_CALLBACK(DeveloperInstalledDetails));
    const size_t fields[] = {1U, 2U, 8U, 10U};
    for (size_t i = 0U; i < sizeof(fields) / sizeof(fields[0]); ++i)
        DeveloperConnect(dialog, G_OBJECT(dialog->fields[fields[i]]), "changed",
                         G_CALLBACK(DeveloperInstalledInvalidate));
}
