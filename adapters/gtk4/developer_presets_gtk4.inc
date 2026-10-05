/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_presets_gtk4.inc
 * PURPOSE: Present declared project presets with asynchronous reads and explicit selection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct DeveloperPresetRead
{
    char root[UMI_BUILD_PATH_CAPACITY];
    UmiProjectPresetCatalogue *catalogue;
    UmiStatus status;
    bool expanded;
    UmiProjectPresetReadReport report;
    UmiCancellationToken *cancel;
} DeveloperPresetRead;
static const char *DeveloperPresetStage(UmiProjectPresetStage stage)
{
    return stage == UMI_PROJECT_PRESET_CONFIGURE ? "Configure"
           : stage == UMI_PROJECT_PRESET_BUILD   ? "Build"
                                                 : "Test";
}
static void DeveloperPresetReadFree(gpointer data)
{
    DeveloperPresetRead *read = data;
    UmiProjectPresetCatalogueDestroy(read->catalogue);
    umi_cancellation_token_destroy(read->cancel);
    g_free(read);
}
/* The worker owns only a copied path and its result. GTK objects and the
 * application's callback context are never accessed from this thread. */
static void DeveloperPresetReadWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    DeveloperPresetRead *read = data;
    /* The explicit expanded action follows supported includes on this worker.
     * Retain direct-only dispatch for review; the ordinary Read action still
     * uses that public API below. Neither action starts CMake or applies a name. */
#if 0
    if (read->status == UMI_STATUS_OK)
        read->status = UmiProjectPresetCatalogueRead(read->root, &read->catalogue);
#endif
    if (read->status == UMI_STATUS_OK) {
        read->status = read->expanded
            ? UmiProjectPresetCatalogueReadExpanded(read->root, read->cancel, &read->catalogue, &read->report)
            : UmiProjectPresetCatalogueRead(read->root, &read->catalogue);
        if (umi_cancellation_token_is_requested(read->cancel)) read->status = UMI_STATUS_CANCELLED;
    }
    g_task_return_boolean(task, TRUE);
}
static void DeveloperPresetDetails(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object;
    (void)property;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    /* A discovery update must not recursively accept or replace visible settings. */
    /* Cross-form operations wait until install review finishes publishing its result. */
    if (dialog->installedLoading || dialog->installedChanging) { DeveloperRelease(dialog); return; }
    if (dialog->targetLoading || dialog->targetChanging) { DeveloperRelease(dialog); return; }
    if (dialog->launchChanging || dialog->launchChoosing) { DeveloperRelease(dialog); return; }
    if (!dialog->presetLoading && !dialog->presetChanging)
    {
        /* Ignore recursive selection changes while publishing these details. */
        dialog->presetChanging = 1;
        UmiProjectPresetChoice choice;
        guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->presetPicker));
        if (UmiProjectPresetCatalogueAt(dialog->presetCatalogue, (size_t)selected, &choice) == UMI_STATUS_OK)
        {
            /* Expanded rows carry their actual defining file and inherited
             * review metadata. Retain the direct-only label for review. */
#if 0
            char *text = g_strdup_printf(
                "%s: %s\nSource: %s\n%s\nConfigure reference: %s\nDeclared build folder: %s\n%s%s%s",
                DeveloperPresetStage(choice.stage), choice.name,
                choice.from_user_file ? "CMakeUserPresets.json" : "CMakePresets.json", choice.description,
                choice.configure_preset[0] != '\0' ? choice.configure_preset : "not declared here",
                choice.binary_directory[0] != '\0' ? choice.binary_directory : "not declared here",
                choice.hidden ? "Hidden base preset; cannot select. " : "",
                choice.condition_false ? "Condition is false; cannot select. " : "",
                choice.inherits || choice.condition_deferred
                    ? "Inheritance and conditions are evaluated by CMake when run."
                    : "CMake validates this choice when run.");
#endif
            UmiProjectPresetOrigin origin = {0};
            bool expanded = UmiProjectPresetCatalogueOriginAt(dialog->presetCatalogue,
                (size_t)selected, &origin) == UMI_STATUS_OK;
            char *text = g_strdup_printf(
                "%s: %s\nSource: %s\n%s\nConfigure reference: %s\nBuild folder expression: %s\n%s%s%s%s",
                DeveloperPresetStage(choice.stage), choice.name,
                expanded ? origin.file_path : choice.from_user_file ? "CMakeUserPresets.json" : "CMakePresets.json",
                choice.description, choice.configure_preset[0] != '\0' ? choice.configure_preset : "not provided",
                choice.binary_directory[0] != '\0' ? choice.binary_directory : "not provided",
                choice.hidden ? "Hidden base preset; cannot select. " : "",
                choice.condition_false ? "Condition is false; cannot select. " : "",
                expanded && origin.inherited_metadata ? "Displayed metadata includes inherited values. " : "",
                "CMake validates all settings, expressions and conditions when run.");
            /* Copy before notifying GTK: an extension may close the form or
             * start another read while a label-change notification is delivered. */
            gtk_label_set_text(GTK_LABEL(dialog->presetDetail), text);
            g_free(text);
        }
        dialog->presetChanging = 0;
    }
    DeveloperRelease(dialog);
}
static void DeveloperPresetReadDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context; /* The task holds a controller reference. */
    GTask *task = G_TASK(result);
    DeveloperPresetRead *read = g_task_get_task_data(task);
    (void)g_task_propagate_boolean(task, NULL);
    dialog->presetCancel = NULL; /* The finished worker no longer uses the borrowed token. */
    if (dialog->presetInputsChanged && read->status == UMI_STATUS_OK) read->status = UMI_STATUS_CANCELLED;
    if (!DeveloperVisible(dialog))
        goto done;
    if (read->status != UMI_STATUS_OK)
    {
        /* Keep the failing file visible when an include or inherited name
         * cannot be resolved. The former generic message remains for review. */
#if 0
        char *message = g_strdup_printf("Preset files were not loaded: %s. Check the two files in the "
                                        "project folder; manual entry remains available.",
                                        umi_status_text(read->status));
#endif
        char *message = g_strdup_printf(
            "Preset files were not loaded: %s. %s%s%sManual entry remains available. "
            "Expanded discovery supports literal includes and sourceDir/fileDir expressions; "
            "environment expressions remain with CMake.",
            umi_status_text(read->status), read->report.problem_file[0] != '\0' ? "File: " : "",
            read->report.problem_file, read->report.problem_file[0] != '\0' ? ". " : "");
        gtk_label_set_text(GTK_LABEL(dialog->presetDetail), message);
        g_free(message);
    }
    else
    {
        UmiProjectPresetSummary summary;
        (void)UmiProjectPresetCatalogueSummary(read->catalogue, &summary);
        GtkStringList *names = gtk_string_list_new(NULL);
        for (size_t i = 0U; i < summary.count; ++i)
        {
            UmiProjectPresetChoice choice;
            (void)UmiProjectPresetCatalogueAt(read->catalogue, i, &choice);
            char *name = g_strdup_printf("%s | %s%s%s [%s]%s", DeveloperPresetStage(choice.stage),
                                         choice.name, choice.display_name[0] != '\0' ? " — " : "",
                                         choice.display_name, choice.from_user_file ? "user" : "project",
                                         choice.hidden || choice.condition_false ? " (unavailable)" : "");
            gtk_string_list_append(names, name);
            g_free(name);
        }
        dialog->presetCatalogue = read->catalogue;
        read->catalogue = NULL;
        memcpy(dialog->presetRoot, read->root, strlen(read->root) + 1U);
        gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->presetPicker), G_LIST_MODEL(names));
        g_object_unref(names); /* set_model borrows its input and retains its own reference. */
        if (!DeveloperVisible(dialog))
            goto done;
        gtk_drop_down_set_selected(GTK_DROP_DOWN(dialog->presetPicker), GTK_INVALID_LIST_POSITION);
        if (!DeveloperVisible(dialog))
            goto done;
        /* Report the scope actually read; a direct-only list must not look
         * equivalent to a successfully traversed include graph. Keep the
         * previous label text for engineering review. */
#if 0
        char *message = g_strdup_printf(
            "Read %zu declarations. Choose a row, then Use selected name. %s Names are a snapshot; refresh "
            "after editing preset files. No build settings have been applied.",
            summary.count,
            summary.includes_not_read ? "Included files were not read; this list is incomplete."
                                      : "Only direct declarations in the two standard files are shown.");
#endif
        char *message = read->expanded
            ? g_strdup_printf("Read %zu declarations from %zu files. Choose a row, then Use selected name. "
                "Inherited review metadata is shown; CMake evaluates other settings and expressions when run. "
                "No build settings have been applied.", summary.count, read->report.file_count)
            : g_strdup_printf("Read %zu direct declarations. Choose a row, then Use selected name. %s "
                "No build settings have been applied.", summary.count, summary.includes_not_read
                ? "This direct-only list is incomplete; use Read included presets for supported shared definitions."
                : "Inherited metadata remains unresolved in this direct-only read.");
        gtk_label_set_text(GTK_LABEL(dialog->presetDetail), message);
        g_free(message);
        if (!DeveloperVisible(dialog))
            goto done;
        gtk_widget_set_sensitive(dialog->presetUse, summary.count != 0U);
    }
    if (!DeveloperVisible(dialog))
        goto done;
    /* Keep the busy guard during notifications above. Enabling Read is the
     * last mutation, so a reentrant new request cannot be overwritten here. */
    gtk_widget_set_sensitive(dialog->presetReadIncludes, TRUE);
    if (!DeveloperVisible(dialog)) goto done;
    dialog->presetLoading = 0;
    gtk_widget_set_sensitive(dialog->presetRead, TRUE);
done:
    DeveloperRelease(dialog);
}
static void DeveloperPresetReadClicked(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    /* A discovery update must not recursively accept or replace visible settings. */
    /* Cross-form operations wait until install review finishes publishing its result. */
    if (dialog->installedLoading || dialog->installedChanging) { DeveloperRelease(dialog); return; }
    if (dialog->targetLoading || dialog->targetChanging) { DeveloperRelease(dialog); return; }
    if (dialog->launchChanging || dialog->launchChoosing) { DeveloperRelease(dialog); return; }
    if (dialog->presetLoading || dialog->presetChanging || dialog->accepting)
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->presetLoading = 1;
    dialog->presetInputsChanged = 0;
    gtk_widget_set_sensitive(dialog->presetReadIncludes, FALSE);
    if (!DeveloperVisible(dialog)) goto abandoned;
    UmiProjectPresetCatalogueDestroy(dialog->presetCatalogue);
    dialog->presetCatalogue = NULL;
    gtk_widget_set_sensitive(dialog->presetRead, FALSE);
    if (!DeveloperVisible(dialog))
        goto abandoned;
    gtk_widget_set_sensitive(dialog->presetUse, FALSE);
    if (!DeveloperVisible(dialog))
        goto abandoned;
    gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->presetPicker), NULL);
    if (!DeveloperVisible(dialog))
        goto abandoned;
    gtk_label_set_text(GTK_LABEL(dialog->presetDetail), "Reading the two project preset files…");
    if (!DeveloperVisible(dialog))
        goto abandoned;
    DeveloperPresetRead *read = g_new0(DeveloperPresetRead, 1);
    /* Capture the displayed folder, including an unsaved edit. Later selection
     * checks this path again so a result for a previous project is not reused. */
    const char *root = gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1]));
    read->status = g_strlcpy(read->root, root, sizeof(read->root)) < sizeof(read->root)
                       ? UMI_STATUS_OK
                       : UMI_STATUS_CAPACITY_EXCEEDED;
    read->expanded = GTK_WIDGET(button) == dialog->presetReadIncludes;
    if (read->status == UMI_STATUS_OK) read->status = umi_cancellation_token_create(&read->cancel);
    dialog->presetCancel = read->cancel;
    GTask *task = g_task_new(NULL, NULL, DeveloperPresetReadDone, dialog);
    g_task_set_task_data(task, read, DeveloperPresetReadFree);
    /* Transfer this callback's controller reference to completion. Closing the
     * form drops the result when I/O finishes; it does not block on the worker. */
    g_task_run_in_thread(task, DeveloperPresetReadWorker);
    g_object_unref(task);
    return;
abandoned:
    dialog->presetLoading = 0;
    DeveloperRelease(dialog);
}
static void DeveloperPresetUseClicked(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    /* A discovery update must not recursively accept or replace visible settings. */
    /* Cross-form operations wait until install review finishes publishing its result. */
    if (dialog->installedLoading || dialog->installedChanging) { DeveloperRelease(dialog); return; }
    if (dialog->targetLoading || dialog->targetChanging) { DeveloperRelease(dialog); return; }
    if (dialog->launchChanging || dialog->launchChoosing) { DeveloperRelease(dialog); return; }
    if (dialog->presetLoading || dialog->presetChanging || dialog->accepting)
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->presetChanging = 1;
    if (gtk_editable_get_text(GTK_EDITABLE(dialog->fields[6]))[0] != '\0')
    {
        ShowStatus(dialog, UMI_STATUS_INVALID_ARGUMENT,
                   "Clear the shared CMake preset before choosing a separate stage name.");
        goto done;
    }
    /* Even a change followed by restoring the old text retires the snapshot;
     * require a fresh read before applying a name after project input edits. */
    if (dialog->presetInputsChanged ||
        strcmp(dialog->presetRoot, gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1]))) != 0)
    {
        ShowStatus(dialog, UMI_STATUS_INVALID_STATE,
                   "The project folder changed. Read its preset files before choosing a name.");
        goto done;
    }
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->presetPicker));
    UmiProjectPresetChoice choice;
    UmiBuildProfile current = dialog->profile, selected;
    current.preset[0] = '\0'; /* The visible legacy field was explicitly cleared above. */
    UmiStatus status =
        UmiProjectPresetCatalogueSelect(dialog->presetCatalogue, (size_t)index, &current, &selected);
    if (status == UMI_STATUS_OK)
        status = UmiProjectPresetCatalogueAt(dialog->presetCatalogue, (size_t)index, &choice);
    if (status != UMI_STATUS_OK)
    {
        ShowStatus(
            dialog, status,
            "Choose a visible preset whose condition is not false. Read the files again if they changed.");
        goto done;
    }
    GtkWidget *entry = choice.stage == UMI_PROJECT_PRESET_CONFIGURE ? dialog->configurePreset
                       : choice.stage == UMI_PROJECT_PRESET_BUILD   ? dialog->buildPreset
                                                                    : dialog->testPreset;
    const char *name = choice.stage == UMI_PROJECT_PRESET_CONFIGURE ? selected.configure_preset
                       : choice.stage == UMI_PROJECT_PRESET_BUILD   ? selected.build_preset
                                                                    : selected.test_preset;
    gtk_editable_set_text(GTK_EDITABLE(entry), name);
    if (DeveloperVisible(dialog))
        ShowStatus(dialog, UMI_STATUS_OK,
                   "Name copied to its stage field. Review the build folder and other settings, then Apply "
                   "Settings to save.");
done:
    dialog->presetChanging = 0;
    DeveloperRelease(dialog);
}
/* Project changes and form teardown retire a discovery snapshot. These
 * callbacks only record state/request cancellation; they never mutate widgets
 * during another GTK notification. The task owns the token until completion. */
static void DeveloperPresetInvalidate(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    dialog->presetInputsChanged = 1;
    umi_cancellation_token_request(dialog->presetCancel);
    DeveloperRelease(dialog);
}
static void DeveloperPresetVisibility(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    UmiGtk4DeveloperDialog *dialog = context;
    if (!DeveloperVisible(dialog)) umi_cancellation_token_request(dialog->presetCancel);
}
static void DeveloperPresetControls(UmiGtk4DeveloperDialog *dialog)
{
    dialog->presetRead = gtk_button_new_with_label("Read project preset files");
    dialog->presetReadIncludes = gtk_button_new_with_label("Read included presets");
    gtk_widget_set_tooltip_text(dialog->presetReadIncludes,
        "Read supported local include files, including declared paths outside this project, and review inherited metadata. "
        "Use only projects whose preset files you trust. Does not run CMake or apply settings.");
    (void)umi_gtk4_automation_tag_widget(dialog->presetReadIncludes, "developer.presets.read-includes");
    DeveloperConnect(dialog, G_OBJECT(dialog->presetReadIncludes), "clicked", G_CALLBACK(DeveloperPresetReadClicked));
    DeveloperConnect(dialog, G_OBJECT(dialog->fields[1]), "changed", G_CALLBACK(DeveloperPresetInvalidate));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::visible", G_CALLBACK(DeveloperPresetVisibility));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::child", G_CALLBACK(DeveloperPresetVisibility));
    dialog->presetPicker = gtk_drop_down_new(NULL, NULL);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dialog->presetPicker), TRUE);
    dialog->presetUse = gtk_button_new_with_label("Use selected name");
    dialog->presetDetail =
        gtk_label_new("Read CMakePresets.json and CMakeUserPresets.json to choose a declared name. Included "
                      "files and inherited values are not resolved. Manual stage fields remain available.");
    gtk_label_set_wrap(GTK_LABEL(dialog->presetDetail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(dialog->presetDetail), 0.0F);
    gtk_widget_set_sensitive(dialog->presetUse, FALSE);
    (void)umi_gtk4_automation_tag_widget(dialog->presetRead, "developer.presets.read");
    (void)umi_gtk4_automation_tag_widget(dialog->presetPicker, "developer.presets.choices");
    (void)umi_gtk4_automation_tag_widget(dialog->presetUse, "developer.presets.use");
    (void)umi_gtk4_automation_tag_widget(dialog->presetDetail, "developer.presets.detail");
    gtk_box_append(GTK_BOX(dialog->body), dialog->presetRead);
    gtk_box_append(GTK_BOX(dialog->body), dialog->presetReadIncludes);
    gtk_box_append(GTK_BOX(dialog->body), dialog->presetPicker);
    gtk_box_append(GTK_BOX(dialog->body), dialog->presetUse);
    gtk_box_append(GTK_BOX(dialog->body), dialog->presetDetail);
    DeveloperConnect(dialog, G_OBJECT(dialog->presetRead), "clicked", G_CALLBACK(DeveloperPresetReadClicked));
    DeveloperConnect(dialog, G_OBJECT(dialog->presetPicker), "notify::selected",
                     G_CALLBACK(DeveloperPresetDetails));
    DeveloperConnect(dialog, G_OBJECT(dialog->presetUse), "clicked", G_CALLBACK(DeveloperPresetUseClicked));
}
