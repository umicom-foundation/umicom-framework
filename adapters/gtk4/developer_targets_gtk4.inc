/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_targets_gtk4.inc
 * PURPOSE: Offer configured targets without starting a build or accepting settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct DeveloperTargetRead
{
    char source[UMI_BUILD_PATH_CAPACITY];
    char build[UMI_BUILD_PATH_CAPACITY];
    char configuration[UMI_BUILD_NAME_CAPACITY];
    UmiProjectTargetCatalogue *catalogue;
    UmiStatus status;
    bool request;
    bool inspect;
    UmiProjectBuildCache cache;
    UmiCancellationToken *cancel;
} DeveloperTargetRead;
static bool DeveloperTargetBusy(const UmiGtk4DeveloperDialog *dialog)
{
    size_t source = 0U;
    if (dialog->targetFilter != NULL && UmiGtk4FilteredChoicesSelectedSource(dialog->targetFilter, &source) == UMI_STATUS_BUSY) return true;
    /* Installed-file filtering publishes a model and source mapping together.
     * Other discovery actions must wait until that publication has finished. */
    if (dialog->installedFilter != NULL &&
        UmiGtk4FilteredChoicesSelectedSource(dialog->installedFilter, &source) == UMI_STATUS_BUSY)
        return true;
    /* A program choice must not race a second discovery or profile acceptance. */
    if (dialog->installedLoading || dialog->installedChanging) return true;
    return dialog->targetLoading || dialog->targetChanging || dialog->presetLoading ||
           dialog->presetChanging || dialog->launchChoosing || dialog->launchChanging || dialog->accepting;
}
/* Changing configuration inputs retires the visible discovery snapshot, even
 * when the user subsequently restores the old spelling. A fresh read is needed.
 * No GTK mutation here: entry notifications may arrive during model publication. */
static void DeveloperTargetInvalidate(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    dialog->targetInputsChanged = 1;
    umi_cancellation_token_request(dialog->targetCancel);
    DeveloperRelease(dialog);
}
static void DeveloperTargetReadFree(gpointer data)
{
    DeveloperTargetRead *read = data;
    UmiProjectTargetCatalogueDestroy(read->catalogue);
    umi_cancellation_token_destroy(read->cancel);
    g_free(read);
}
/* GIO supplies native UTF-8 directory handling for this GTK presenter. Query
 * creation is explicit and exclusive: an existing empty regular query is reused,
 * other content is left untouched. CMake owns all replies; none are modified.
 * The owner must choose a build folder they control; parent aliases are followed.
 * A close hides the result but does not roll back a requested filesystem write. */
static UmiStatus DeveloperTargetRequestQuery(const char *build)
{
    char path[UMI_BUILD_PATH_CAPACITY];
    UmiStatus status = UmiOutputFileValidatePath(build);
    if (status != UMI_STATUS_OK)
        return status;
    GFile *root = g_file_new_for_path(build);
    GFileType type = g_file_query_file_type(root, G_FILE_QUERY_INFO_NONE, NULL);
    g_object_unref(root);
    if (type != G_FILE_TYPE_DIRECTORY)
        return UMI_STATUS_NOT_FOUND;
    status =
        umi_path_join(build, ".cmake/api/v1/query/client-umicom-studio/codemodel-v2", path, sizeof(path));
    if (status != UMI_STATUS_OK)
        return status;
    GFile *query = g_file_new_for_path(path), *parent = g_file_get_parent(query);
    GError *error = NULL;
    if (!g_file_make_directory_with_parents(parent, NULL, &error) &&
        !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_EXISTS))
    {
        status = UMI_STATUS_IO_ERROR;
        goto done;
    }
    g_clear_error(&error);
    GFileOutputStream *stream = g_file_create(query, G_FILE_CREATE_PRIVATE, NULL, &error);
    if (stream != NULL)
    {
        status = g_output_stream_close(G_OUTPUT_STREAM(stream), NULL, &error) ? UMI_STATUS_OK
                                                                              : UMI_STATUS_IO_ERROR;
        g_object_unref(stream);
    }
    else if (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_EXISTS))
    {
        g_clear_error(&error);
        GFileInfo *info =
            g_file_query_info(query, G_FILE_ATTRIBUTE_STANDARD_TYPE "," G_FILE_ATTRIBUTE_STANDARD_SIZE,
                              G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &error);
        status = info != NULL && g_file_info_get_file_type(info) == G_FILE_TYPE_REGULAR &&
                         g_file_info_get_size(info) == 0
                     ? UMI_STATUS_OK
                     : UMI_STATUS_INVALID_STATE;
        g_clear_object(&info);
    }
    else
        status = UMI_STATUS_IO_ERROR;
done:
    g_clear_error(&error);
    g_object_unref(parent);
    g_object_unref(query);
    return status;
}
/* Cache inspection shares the asynchronous task owner with discovery so the
 * form cannot accept settings from inside a read-completion notification. The
 * earlier target-only worker is retained for engineering review. */
#if 0
static void DeveloperTargetWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    DeveloperTargetRead *read = data;
    if (read->status == UMI_STATUS_OK)
        read->status = read->request ? DeveloperTargetRequestQuery(read->build)
                                     : UmiProjectTargetCatalogueRead(read->source, read->build,
                                                                     read->configuration, &read->catalogue);
    g_task_return_boolean(task, TRUE);
}
#endif
static void DeveloperTargetWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source;
    (void)cancel;
    DeveloperTargetRead *read = data;
    if (read->status == UMI_STATUS_OK && read->inspect)
        read->status = UmiProjectBuildCacheRead(read->source, read->build, &read->cache);
    else if (read->status == UMI_STATUS_OK)
        read->status = read->request ? DeveloperTargetRequestQuery(read->build)
                                     : UmiProjectTargetCatalogueReadCancellable(read->source, read->build,
                                                                     read->configuration, read->cancel, &read->catalogue);
    g_task_return_boolean(task, TRUE);
}
static void DeveloperTargetDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context; /* Completion owns the callback's reference. */
    DeveloperTargetRead *read = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    dialog->targetCancel = NULL; /* The worker has returned; task data still owns the token. */
    if (!DeveloperVisible(dialog))
        goto done;
    if (read->inspect)
    {
        /* This display intentionally changes no profile fields. Presets can
         * override manual fields; the recorded cache lets the user compare
         * their actual generator and toolchain before applying other edits. */
        char *text;
        if (dialog->targetInputsChanged)
            text = g_strdup("Build inputs changed during inspection. Inspect the build folder again.");
        else if (read->status != UMI_STATUS_OK)
            text = g_strdup_printf("Build folder inspection failed: %s. Check the selected folder and its CMakeCache.txt. No settings were changed.", umi_status_text(read->status));
        else
            text = g_strdup_printf(
                "Recorded project: %s\nRecorded build folder: %s\nGenerator: %s\nC compiler: %s\nBuild type: %s\nConfigurations: %s\nInstall prefix: %s\nToolchain: %s\n%s\nThis cache is a snapshot; it does not prove Configure or Build succeeded. No settings were changed.",
                read->cache.source_directory, read->cache.build_directory, read->cache.generator,
                read->cache.compiler[0] ? read->cache.compiler : "not recorded",
                read->cache.configuration[0] ? read->cache.configuration : "empty or not recorded",
                read->cache.configurations[0] ? read->cache.configurations : "not recorded",
                read->cache.install_directory[0] ? read->cache.install_directory : "not recorded",
                read->cache.toolchain_file[0] ? read->cache.toolchain_file : "not recorded",
                !read->cache.source_matches ? "PROJECT MISMATCH: this cache names another source folder. Choose the matching build folder before continuing."
                    : !read->cache.build_matches ? "BUILD FOLDER MISMATCH: this cache was created in another folder. Select the original folder or configure a fresh one."
                    : "Project and build-folder paths match. Compare the recorded options with your intended settings, especially when using presets.");
        gtk_label_set_text(GTK_LABEL(dialog->targetDetail), text);
        g_free(text);
    }
    else if (read->request)
    {
        char *text =
            g_strdup_printf("Target-data request in %s: %s. %s", read->build, umi_status_text(read->status),
                            read->status == UMI_STATUS_OK
                                ? "Apply the matching build settings, run Configure successfully, then "
                                  "reopen settings and Read configured targets. No build tool was started."
                                : "Choose an existing build folder you control. Existing query content was "
                                  "not replaced. Created directories or a partial new query may remain.");
        gtk_label_set_text(GTK_LABEL(dialog->targetDetail), text);
        g_free(text);
    }
    else if (dialog->targetInputsChanged)
    {
        gtk_label_set_text(GTK_LABEL(dialog->targetDetail),
                           "Build inputs changed during discovery. Read configured targets again.");
    }
    else if (read->status != UMI_STATUS_OK)
    {
        char *text =
            g_strdup_printf("Targets were not loaded: %s. The build folder must contain a successful CMake "
                            "file-API codemodel reply for this project and configuration. See Discover "
                            "configured targets in the learning guide. Manual fields remain available.",
                            umi_status_text(read->status));
        gtk_label_set_text(GTK_LABEL(dialog->targetDetail), text);
        g_free(text);
    }
    else
    {
        UmiProjectTargetSummary summary;
        (void)UmiProjectTargetCatalogueSummary(read->catalogue, &summary);
        GtkStringList *names = gtk_string_list_new(NULL);
        for (size_t i = 0U; i < summary.count; ++i)
        {
            UmiProjectTargetChoice choice;
            (void)UmiProjectTargetCatalogueAt(read->catalogue, i, &choice);
            char *name = g_strdup_printf("%s | %s", choice.name, choice.type);
            gtk_string_list_append(names, name);
            g_free(name);
        }
/* Visible sorting must not change the configured target that a selection refers to. The previous implementation is retained for engineering review. */
#if 0
        dialog->targetCatalogue = read->catalogue;
        read->catalogue = NULL;
        gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->targetPicker), G_LIST_MODEL(names));
        g_object_unref(names); /* set_model retains, rather than consumes, this model. */
#endif
        /* The projection owns copied labels; the catalogue continues to own
         * build metadata. Publish both only after text validation succeeds. */
        const char **rows = g_new(const char *, summary.count + 1U);
        for (size_t i = 0U; i < summary.count; ++i)
            rows[i] = gtk_string_list_get_string(names, (guint)i);
        UmiStatus mapped = UmiGtk4FilteredChoicesSetRows(dialog->targetFilter, rows, summary.count);
        g_free(rows);
        g_object_unref(names);
        if (mapped == UMI_STATUS_OK)
        {
            dialog->targetCatalogue = read->catalogue;
            read->catalogue = NULL;
        }
        else
        {
            if (DeveloperVisible(dialog))
                gtk_label_set_text(GTK_LABEL(dialog->targetDetail),
                    "Targets could not be displayed within the text limits. Manual settings remain available.");
            goto publish_done;
        }
        if (!DeveloperVisible(dialog))
            goto done;
        gtk_drop_down_set_selected(GTK_DROP_DOWN(dialog->targetPicker), GTK_INVALID_LIST_POSITION);
        if (!DeveloperVisible(dialog))
            goto done;
        char *text =
            g_strdup_printf("Read %zu configured targets from %s. Configuration: %s. Select a row to inspect "
                            "its output, then copy a target or program. No settings have been applied.",
                            summary.count, summary.build_directory,
                            summary.configuration[0] != '\0' ? summary.configuration : "unnamed");
        gtk_label_set_text(GTK_LABEL(dialog->targetDetail), text);
        g_free(text);
    }
publish_done:
    if (!DeveloperVisible(dialog))
        goto done;
    if (!read->request && dialog->targetInputsChanged)
        gtk_label_set_text(
            GTK_LABEL(dialog->targetDetail),
            "The discovery snapshot is stale because build inputs changed. Read configured targets again.");
    if (!DeveloperVisible(dialog))
        goto done;
    dialog->targetLoading = 0;
    gtk_widget_set_sensitive(dialog->targetRead, TRUE); /* Last mutation permits a new request. */
done:
    DeveloperRelease(dialog);
}
static void DeveloperTargetReadClicked(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (DeveloperTargetBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->targetLoading = 1;
    dialog->targetInputsChanged = 0;
    UmiProjectTargetCatalogueDestroy(dialog->targetCatalogue);
    dialog->targetCatalogue = NULL;
    /* Capture inputs before any notification can reenter the form. The worker
     * owns strings and a catalogue only; it never dereferences a GTK widget. */
    DeveloperTargetRead *read = g_new0(DeveloperTargetRead, 1);
    read->request = GTK_WIDGET(button) == dialog->targetRequest;
    read->inspect = GTK_WIDGET(button) == dialog->cacheRead;
    read->status = umi_text_copy(read->source, sizeof(read->source),
                                 gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1])));
    if (read->status == UMI_STATUS_OK && !umi_path_is_absolute(read->source))
        read->status = UMI_STATUS_INVALID_ARGUMENT;
    const char *buildInput = gtk_editable_get_text(GTK_EDITABLE(dialog->fields[2]));
    /* A blank folder is not permission to write a query in the project root. */
    if (buildInput[0] == '\0')
        read->status = UMI_STATUS_INVALID_ARGUMENT;
#ifdef _WIN32
    if (!umi_path_is_absolute(buildInput) &&
        (buildInput[0] == '/' || buildInput[0] == '\\' || strchr(buildInput, ':') != NULL))
        read->status = UMI_STATUS_INVALID_ARGUMENT;
#endif
    if (read->status == UMI_STATUS_OK)
        read->status = umi_path_absolute(gtk_editable_get_text(GTK_EDITABLE(dialog->fields[2])), read->source,
                                         read->build, sizeof(read->build));
    if (read->status == UMI_STATUS_OK)
        read->status = umi_text_copy(read->configuration, sizeof(read->configuration),
                                     gtk_editable_get_text(GTK_EDITABLE(dialog->fields[5])));
    if (read->status == UMI_STATUS_OK)
        read->status = umi_cancellation_token_create(&read->cancel);
    dialog->targetCancel = read->cancel;
    gtk_widget_set_sensitive(dialog->targetRead, FALSE);
    if (!DeveloperVisible(dialog))
        goto abandoned;
/* Retiring discovery clears the owned mapping as well as its visible rows. The previous implementation is retained for engineering review. */
#if 0
    gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->targetPicker), NULL);
#endif
    (void)UmiGtk4FilteredChoicesSetRows(dialog->targetFilter, NULL, 0U);
    if (!DeveloperVisible(dialog))
        goto abandoned;
    /* Inspection now shares the worker. Keep the earlier two-operation message
     * for review; the active text tells the user which read they requested. */
#if 0
    gtk_label_set_text(GTK_LABEL(dialog->targetDetail),
                       read->request ? "Creating an empty CMake query in the selected build folder. Closing "
                                       "this form does not undo the request…"
                                     : "Reading the configured target snapshot…");
#endif
    gtk_label_set_text(GTK_LABEL(dialog->targetDetail), read->inspect ? "Inspecting the build folder's CMake cache…"
        : read->request ? "Creating an empty CMake query in the selected build folder. Closing this form does not undo the request…"
                        : "Reading the configured target snapshot…");
    if (!DeveloperVisible(dialog))
        goto abandoned;
    GTask *task = g_task_new(NULL, NULL, DeveloperTargetDone, dialog);
    g_task_set_task_data(task, read, DeveloperTargetReadFree);
    g_task_run_in_thread(task, DeveloperTargetWorker);
    g_object_unref(task);
    return;
abandoned:
    dialog->targetCancel = NULL;
    DeveloperTargetReadFree(read);
    dialog->targetLoading = 0;
    DeveloperRelease(dialog);
}
static void DeveloperTargetDetails(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object;
    (void)property;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (DeveloperTargetBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->targetChanging = 1;
    UmiProjectTargetChoice choice;
/* A filtered position is resolved through the captured mapping before metadata lookup. The previous implementation is retained for engineering review. */
#if 0
    guint index = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->targetPicker));
    if (UmiProjectTargetCatalogueAt(dialog->targetCatalogue, (size_t)index, &choice) == UMI_STATUS_OK)
#endif
    size_t index = 0U;
    if (UmiGtk4FilteredChoicesSelectedSource(dialog->targetFilter, &index) == UMI_STATUS_OK &&
        UmiProjectTargetCatalogueAt(dialog->targetCatalogue, index, &choice) == UMI_STATUS_OK)
    {
        char *text = g_strdup_printf(
            "%s | %s\nProgram: %s\n%s", choice.name, choice.type,
            choice.program[0] != '\0' ? choice.program : "No unambiguous executable output is available.",
            dialog->targetInputsChanged
                ? "Build inputs changed; read again before selecting."
                : "This is configured metadata, not proof that an output was built. Copying changes one "
                  "field only; review and Apply Settings separately.");
        gtk_label_set_text(GTK_LABEL(dialog->targetDetail), text);
        g_free(text);
        if (DeveloperVisible(dialog) && dialog->targetInputsChanged)
            gtk_label_set_text(GTK_LABEL(dialog->targetDetail),
                               "Build inputs changed; read configured targets again before selecting.");
    }
    else if (DeveloperVisible(dialog))
        gtk_label_set_text(GTK_LABEL(dialog->targetDetail),
            "No configured target is selected. Select a visible row, or clear the filter and apply it to see all captured targets.");
    dialog->targetChanging = 0;
    DeveloperRelease(dialog);
}
static void DeveloperTargetUse(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (DeveloperTargetBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->targetChanging = 1;
    if (dialog->targetInputsChanged)
    {
        ShowStatus(dialog, UMI_STATUS_INVALID_STATE,
                   "Build inputs changed. Read configured targets again before copying a value.");
        goto done;
    }
    UmiBuildProfile current = dialog->profile, selected;
    UmiStatus status = umi_text_copy(current.source_directory, sizeof(current.source_directory),
                                     gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(current.build_directory, sizeof(current.build_directory),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[2])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(current.configuration, sizeof(current.configuration),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[5])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(current.build_preset, sizeof(current.build_preset),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->buildPreset)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(current.preset, sizeof(current.preset),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[6])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(current.configure_preset, sizeof(current.configure_preset),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->configurePreset)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(current.test_preset, sizeof(current.test_preset),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->testPreset)));
    UmiProjectTargetSelection kind =
        GTK_WIDGET(button) == dialog->targetProgram ? UMI_PROJECT_TARGET_PROGRAM : UMI_PROJECT_TARGET_BUILD;
/* Build and program selection share the original catalogue index after filtering or sorting. The previous implementation is retained for engineering review. */
#if 0
    if (status == UMI_STATUS_OK)
        status = UmiProjectTargetCatalogueSelect(
            dialog->targetCatalogue, (size_t)gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->targetPicker)),
            kind, &current, &selected);
#endif
    size_t sourceIndex = 0U;
    if (status == UMI_STATUS_OK)
        status = UmiGtk4FilteredChoicesSelectedSource(dialog->targetFilter, &sourceIndex);
    if (status == UMI_STATUS_OK)
        status = UmiProjectTargetCatalogueSelect(dialog->targetCatalogue, sourceIndex, kind, &current, &selected);
    if (status != UMI_STATUS_OK)
    {
        ShowStatus(dialog, status,
                   "Choose a valid row from this build folder. Program selection needs an executable output; "
                   "build target selection needs a blank Build preset field.");
        goto done;
    }
    gtk_editable_set_text(GTK_EDITABLE(dialog->fields[kind == UMI_PROJECT_TARGET_PROGRAM ? 8 : 7]),
                          kind == UMI_PROJECT_TARGET_PROGRAM ? selected.run_program : selected.build_target);
    if (DeveloperVisible(dialog))
        ShowStatus(dialog, UMI_STATUS_OK,
                   "Value copied. Review the target, program and configuration, then Apply Settings to save. "
                   "Nothing was launched.");
done:
    dialog->targetChanging = 0;
    DeveloperRelease(dialog);
}
/* Parent destruction and a hidden form both retire discovery work. This
 * notification performs no GTK mutation; registered signal ownership keeps
 * the controller alive until these handlers have been disconnected. */
static void DeveloperTargetVisibility(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    UmiGtk4DeveloperDialog *dialog = context;
    if (!DeveloperVisible(dialog)) umi_cancellation_token_request(dialog->targetCancel);
}
static void DeveloperTargetControls(UmiGtk4DeveloperDialog *dialog)
{
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::visible", G_CALLBACK(DeveloperTargetVisibility));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::child", G_CALLBACK(DeveloperTargetVisibility));
    dialog->cacheRead = gtk_button_new_with_label("Inspect build folder");
    gtk_widget_set_tooltip_text(dialog->cacheRead,
        "Read CMakeCache.txt to review project identity, generator, compiler and configuration. Does not run tools or change settings. A new inspection clears the previous target list.");
    (void)umi_gtk4_automation_tag_widget(dialog->cacheRead, "developer.build.inspect-cache");
    gtk_box_append(GTK_BOX(dialog->body), dialog->cacheRead);
    DeveloperConnect(dialog, G_OBJECT(dialog->cacheRead), "clicked", G_CALLBACK(DeveloperTargetReadClicked));
    dialog->targetRequest = gtk_button_new_with_label("Request target data in this build folder");
    gtk_widget_set_tooltip_text(dialog->targetRequest,
                                "Creates an empty CMake file-API query in the existing Build folder. Does "
                                "not run Configure, overwrite existing content, or apply settings.");
    (void)umi_gtk4_automation_tag_widget(dialog->targetRequest, "developer.targets.request");
    gtk_box_append(GTK_BOX(dialog->body), dialog->targetRequest);
    DeveloperConnect(dialog, G_OBJECT(dialog->targetRequest), "clicked",
                     G_CALLBACK(DeveloperTargetReadClicked));
    dialog->targetRead = gtk_button_new_with_label("Read configured targets");
    dialog->targetPicker = gtk_drop_down_new(NULL, NULL);
    gtk_drop_down_set_enable_search(GTK_DROP_DOWN(dialog->targetPicker), TRUE);
    dialog->targetFilter = UmiGtk4FilteredChoicesCreate(GTK_DROP_DOWN(dialog->targetPicker));
    (void)umi_gtk4_automation_tag_widget(dialog->targetFilter, "developer.targets.filter");
    gtk_widget_set_tooltip_text(dialog->targetFilter,
        "Filter target names and types, then apply the filter. Original target identities are preserved. Sorting is textual, with ASCII case matching.");
    gtk_box_append(GTK_BOX(dialog->body), dialog->targetFilter);
    dialog->targetBuild = gtk_button_new_with_label("Use selected build target");
    dialog->targetProgram = gtk_button_new_with_label("Use selected program");
    dialog->targetDetail =
        gtk_label_new("Discover targets from existing CMake file-API replies in the Build folder. Nothing is "
                      "read until requested. Request target data creates an empty query; Configure must then "
                      "succeed in that same folder. Manual settings remain available.");
    gtk_label_set_wrap(GTK_LABEL(dialog->targetDetail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(dialog->targetDetail), 0.0F);
    GtkWidget *controls[] = {dialog->targetRead, dialog->targetPicker, dialog->targetBuild,
                             dialog->targetProgram, dialog->targetDetail};
    const char *ids[] = {"developer.targets.read", "developer.targets.choices", "developer.targets.build",
                         "developer.targets.program", "developer.targets.detail"};
    for (size_t i = 0U; i < 5U; ++i)
    {
        (void)umi_gtk4_automation_tag_widget(controls[i], ids[i]);
        gtk_box_append(GTK_BOX(dialog->body), controls[i]);
    }
    DeveloperConnect(dialog, G_OBJECT(dialog->targetRead), "clicked", G_CALLBACK(DeveloperTargetReadClicked));
    DeveloperConnect(dialog, G_OBJECT(dialog->targetPicker), "notify::selected",
                     G_CALLBACK(DeveloperTargetDetails));
    DeveloperConnect(dialog, G_OBJECT(dialog->targetBuild), "clicked", G_CALLBACK(DeveloperTargetUse));
    DeveloperConnect(dialog, G_OBJECT(dialog->targetProgram), "clicked", G_CALLBACK(DeveloperTargetUse));
    const size_t inputs[] = {1U, 2U, 3U, 4U, 5U, 6U};
    for (size_t i = 0U; i < sizeof(inputs) / sizeof(inputs[0]); ++i)
        DeveloperConnect(dialog, G_OBJECT(dialog->fields[inputs[i]]), "changed",
                         G_CALLBACK(DeveloperTargetInvalidate));
    DeveloperConnect(dialog, G_OBJECT(dialog->configurePreset), "changed",
                     G_CALLBACK(DeveloperTargetInvalidate));
    DeveloperConnect(dialog, G_OBJECT(dialog->buildPreset), "changed", G_CALLBACK(DeveloperTargetInvalidate));
}
