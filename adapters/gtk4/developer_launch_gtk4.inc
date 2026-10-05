/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_launch_gtk4.inc
 * PURPOSE: Offer explicit launch-path selection and non-executing launch review.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct DeveloperLaunchChoice
{
    UmiGtk4DeveloperDialog *dialog;
    UmiBuildLaunchPathKind kind;
    char previous[UMI_BUILD_PATH_CAPACITY];
    char project[UMI_BUILD_PATH_CAPACITY];
} DeveloperLaunchChoice;

/* Read only the launch fields. Other unsaved build options are validated by
 * Apply Settings; reviewing a launch never adopts settings or execution trust. */
static UmiStatus DeveloperLaunchInputs(UmiGtk4DeveloperDialog *dialog, UmiBuildProfile *out)
{
    *out = dialog->profile;
    UmiStatus status = umi_text_copy(out->source_directory, sizeof(out->source_directory),
                                     gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(out->run_program, sizeof(out->run_program),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[8])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(out->run_argument, sizeof(out->run_argument),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[9])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(out->run_arguments, sizeof(out->run_arguments),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->arguments)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(out->run_working_directory, sizeof(out->run_working_directory),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->runDirectory)));
    return status;
}
static void DeveloperLaunchInvalidate(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    dialog->launchInvalidated = 1;
    if (!dialog->launchChanging)
    {
        dialog->launchChanging = 1;
        gtk_label_set_text(
            GTK_LABEL(dialog->launchDetail),
            "Launch inputs changed. Choose Review launch settings to refresh the description.");
        dialog->launchChanging = 0;
    }
    DeveloperRelease(dialog);
}
static void DeveloperLaunchReview(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    /* A discovery update must not recursively accept or replace visible settings. */
    /* Cross-form operations wait until install review finishes publishing its result. */
    if (dialog->installedLoading || dialog->installedChanging) { DeveloperRelease(dialog); return; }
    if (dialog->targetLoading || dialog->targetChanging) { DeveloperRelease(dialog); return; }
    if (dialog->launchChanging || dialog->launchChoosing || dialog->presetChanging || dialog->presetLoading ||
        dialog->accepting)
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->launchChanging = 1;
    dialog->launchInvalidated = 0;
    UmiBuildProfile profile;
    UmiBuildLaunchPlan plan;
    UmiStatus status = DeveloperLaunchInputs(dialog, &profile);
    if (status == UMI_STATUS_OK)
        status = UmiBuildLaunchPlanCreate(&profile, profile.source_directory, &plan);
    GString *text = g_string_new(NULL);
    if (status != UMI_STATUS_OK)
        g_string_append_printf(text,
                               "Cannot describe this launch: %s. Check the program, working folder and "
                               "argument fields. No process was started.",
                               umi_status_text(status));
    else
    {
        g_string_append_printf(
            text, "Run program: %s\nNative Debug program: %s\nWorking folder: %s\nArguments: %zu\n",
            plan.run_program, plan.debug_program, plan.working_directory, plan.argument_count);
        for (size_t i = 0U; i < plan.argument_count; ++i)
        {
            /* Escape control characters for display while retaining UTF-8.
             * This description is never used as a shell command. */
            g_string_append_printf(text, "  [%zu] \"", i);
            for (const unsigned char *p = (const unsigned char *)plan.arguments[i]; *p != 0U; ++p)
            {
                if (*p == '\n')
                    g_string_append(text, "\\n");
                else if (*p == '\r')
                    g_string_append(text, "\\r");
                else if (*p == '\t')
                    g_string_append(text, "\\t");
                else if (*p == '\\' || *p == '"')
                {
                    g_string_append_c(text, '\\');
                    g_string_append_c(text, (char)*p);
                }
                else if (*p < 0x20U || *p == 0x7fU)
                    g_string_append_printf(text, "\\x%02x", (unsigned)*p);
                else
                    g_string_append_c(text, (char)*p);
            }
            g_string_append(text, "\"\n");
        }
        if (plan.run_uses_program_lookup)
            g_string_append(text, "Run uses operating-system program lookup for this bare name; native Debug "
                                  "uses the project file above. Choose an absolute executable or a path "
                                  "beginning with ./ to make both explicit.\n");
        g_string_append(text, "Description only. Files, permissions, executable format and debugger "
                              "compatibility have not been checked. Apply Settings saves separately.");
    }
    gtk_label_set_text(GTK_LABEL(dialog->launchDetail), text->str);
    g_string_free(text, TRUE);
    if (DeveloperVisible(dialog) && dialog->launchInvalidated)
        gtk_label_set_text(
            GTK_LABEL(dialog->launchDetail),
            "Launch inputs changed during review. Review again before using this description.");
    dialog->launchChanging = 0;
    DeveloperRelease(dialog);
}
/* The chooser result owns a controller reference. The form may close while
 * the native dialog is visible; its callback must still finish and free data. */
static void DeveloperLaunchChosen(GObject *source, GAsyncResult *result, gpointer context)
{
    DeveloperLaunchChoice *choice = context;
    UmiGtk4DeveloperDialog *dialog = choice->dialog;
    GError *error = NULL;
    GFile *file = choice->kind == UMI_BUILD_LAUNCH_PROGRAM
                      ? gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error)
                      : gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &error);
    char *path = file != NULL ? g_file_get_path(file) : NULL;
    if (DeveloperVisible(dialog))
    {
        GtkWidget *entry =
            choice->kind == UMI_BUILD_LAUNCH_PROGRAM ? dialog->fields[8] : dialog->runDirectory;
        dialog->launchChanging = 1;
        if (file != NULL)
        {
            UmiBuildProfile selected;
            UmiStatus status = path != NULL && g_utf8_validate(path, -1, NULL)
                                   ? UmiBuildLaunchSelectPath(&dialog->profile, choice->kind, path, &selected)
                                   : UMI_STATUS_INVALID_ARGUMENT;
            if (status == UMI_STATUS_OK &&
                (strcmp(choice->previous, gtk_editable_get_text(GTK_EDITABLE(entry))) != 0 ||
                 strcmp(choice->project, gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1]))) != 0))
                status = UMI_STATUS_INVALID_STATE;
            if (status == UMI_STATUS_OK)
            {
                const char *value = choice->kind == UMI_BUILD_LAUNCH_PROGRAM ? selected.run_program
                                                                             : selected.run_working_directory;
                gtk_editable_set_text(GTK_EDITABLE(entry), value);
                if (DeveloperVisible(dialog))
                    gtk_label_set_text(GTK_LABEL(dialog->launchDetail),
                                       "Path selected. Review launch settings, then Apply Settings to save. "
                                       "Nothing has been launched.");
            }
            else
                ShowStatus(dialog, status,
                           "The path could not be applied, or the field changed while choosing. Keep your "
                           "edits and choose again using a local absolute path.");
        }
        else if (error != NULL && !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
                 !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
            ShowStatus(dialog, UMI_STATUS_IO_ERROR,
                       "The native path chooser failed. You can still enter a path manually.");
        dialog->launchChanging = 0;
    }
    g_free(path);
    g_clear_object(&file);
    g_clear_error(&error);
    g_clear_object(&dialog->launchChooserCancel);
    dialog->launchChoosing = 0;
    g_free(choice);
    DeveloperRelease(dialog);
}
static void DeveloperLaunchChoose(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    /* A discovery update must not recursively accept or replace visible settings. */
    /* Cross-form operations wait until install review finishes publishing its result. */
    if (dialog->installedLoading || dialog->installedChanging) { DeveloperRelease(dialog); return; }
    if (dialog->targetLoading || dialog->targetChanging) { DeveloperRelease(dialog); return; }
    if (dialog->launchChanging || dialog->launchChoosing || dialog->presetChanging || dialog->presetLoading ||
        dialog->accepting)
    {
        DeveloperRelease(dialog);
        return;
    }
    DeveloperLaunchChoice *choice = g_new0(DeveloperLaunchChoice, 1);
    choice->dialog = dialog;
    choice->kind =
        GTK_WIDGET(button) == dialog->chooseProgram ? UMI_BUILD_LAUNCH_PROGRAM : UMI_BUILD_LAUNCH_DIRECTORY;
    GtkWidget *entry = choice->kind == UMI_BUILD_LAUNCH_PROGRAM ? dialog->fields[8] : dialog->runDirectory;
    UmiStatus status =
        umi_text_copy(choice->previous, sizeof(choice->previous), gtk_editable_get_text(GTK_EDITABLE(entry)));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(choice->project, sizeof(choice->project),
                               gtk_editable_get_text(GTK_EDITABLE(dialog->fields[1])));
    if (status != UMI_STATUS_OK)
    {
        g_free(choice);
        DeveloperRelease(dialog);
        return;
    }
    dialog->launchChoosing = 1;
    dialog->launchChooserCancel = g_cancellable_new();
    GtkFileDialog *picker = gtk_file_dialog_new();
    gtk_file_dialog_set_title(picker, choice->kind == UMI_BUILD_LAUNCH_PROGRAM
                                          ? "Choose program to run"
                                          : "Choose program working folder");
    gtk_file_dialog_set_modal(picker, TRUE);
    if (umi_path_is_absolute(choice->project))
    {
        GFile *folder = g_file_new_for_path(choice->project);
        gtk_file_dialog_set_initial_folder(picker, folder);
        g_object_unref(folder);
    }
    if (choice->kind == UMI_BUILD_LAUNCH_PROGRAM)
        gtk_file_dialog_open(picker, dialog->window, dialog->launchChooserCancel, DeveloperLaunchChosen,
                             choice);
    else
        gtk_file_dialog_select_folder(picker, dialog->window, dialog->launchChooserCancel,
                                      DeveloperLaunchChosen, choice);
    g_object_unref(picker);
    /* The acquired controller reference now belongs to completion. */
}
static void DeveloperLaunchVisibility(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    UmiGtk4DeveloperDialog *dialog = context;
    /* Visibility notifications occur after hiding, so retain directly instead
     * of using the visible-form acquisition helper. Cancellation may notify. */
    ++dialog->references;
    if (!gtk_widget_get_visible(GTK_WIDGET(object)) && dialog->launchChooserCancel != NULL)
        g_cancellable_cancel(dialog->launchChooserCancel);
    DeveloperRelease(dialog);
}
static void DeveloperLaunchControls(UmiGtk4DeveloperDialog *dialog)
{
    dialog->chooseProgram = gtk_button_new_with_label("Choose program file…");
    dialog->chooseWorkingDirectory = gtk_button_new_with_label("Choose working folder…");
    dialog->reviewLaunch = gtk_button_new_with_label("Review launch settings");
    dialog->launchDetail = gtk_label_new("Choose a program and working folder, or enter paths manually. "
                                         "Review shows launch inputs without executing anything.");
    gtk_label_set_wrap(GTK_LABEL(dialog->launchDetail), TRUE);
    gtk_label_set_selectable(GTK_LABEL(dialog->launchDetail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(dialog->launchDetail), 0.0F);
    GtkWidget *controls[] = {dialog->chooseProgram, dialog->chooseWorkingDirectory, dialog->reviewLaunch,
                             dialog->launchDetail};
    const char *ids[] = {"developer.launch.program", "developer.launch.folder", "developer.launch.review",
                         "developer.launch.detail"};
    for (size_t i = 0U; i < 4U; ++i)
    {
        (void)umi_gtk4_automation_tag_widget(controls[i], ids[i]);
        gtk_box_append(GTK_BOX(dialog->body), controls[i]);
    }
    DeveloperConnect(dialog, G_OBJECT(dialog->chooseProgram), "clicked", G_CALLBACK(DeveloperLaunchChoose));
    DeveloperConnect(dialog, G_OBJECT(dialog->chooseWorkingDirectory), "clicked",
                     G_CALLBACK(DeveloperLaunchChoose));
    DeveloperConnect(dialog, G_OBJECT(dialog->reviewLaunch), "clicked", G_CALLBACK(DeveloperLaunchReview));
    GtkWidget *inputs[] = {dialog->fields[1], dialog->fields[8], dialog->fields[9], dialog->arguments,
                           dialog->runDirectory};
    for (size_t i = 0U; i < 5U; ++i)
        DeveloperConnect(dialog, G_OBJECT(inputs[i]), "changed", G_CALLBACK(DeveloperLaunchInvalidate));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::visible",
                     G_CALLBACK(DeveloperLaunchVisibility));
}
