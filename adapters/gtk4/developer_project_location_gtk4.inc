/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_project_location_gtk4.inc
 * PURPOSE: Let a project author choose its parent folder without creating files.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct DeveloperProjectLocationChoice
{
    UmiGtk4DeveloperDialog *dialog;
    char previous[UMI_PATH_CAPACITY];
    char folder[UMI_PATH_CAPACITY];
} DeveloperProjectLocationChoice;

/* Completion holds the controller, but its permission to edit the form comes
 * from the still-visible native owner and unchanged draft fields. A chooser
 * opened earlier must never replace edits made while it was on screen. */
static void DeveloperProjectLocationChosen(GObject *source, GAsyncResult *result, gpointer context)
{
    DeveloperProjectLocationChoice *choice = context;
    UmiGtk4DeveloperDialog *dialog = choice->dialog;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &error);
    char *parent = file != NULL ? g_file_get_path(file) : NULL;
    if (DeveloperVisible(dialog) && !dialog->generated)
    {
        if (file != NULL)
        {
            char destination[UMI_PATH_CAPACITY];
            UmiStatus status = parent != NULL && g_utf8_validate(parent, -1, NULL)
                                   ? UmiDeveloperProjectChooseDestination(
                                         parent, choice->folder, destination, sizeof destination)
                                   : UMI_STATUS_INVALID_ARGUMENT;
            if (status == UMI_STATUS_OK &&
                (strcmp(choice->previous, gtk_editable_get_text(GTK_EDITABLE(dialog->fields[4]))) !=
                     0 ||
                 strcmp(choice->folder,
                        gtk_editable_get_text(GTK_EDITABLE(dialog->projectFolderName))) != 0))
                status = UMI_STATUS_INVALID_STATE;
            if (status == UMI_STATUS_OK)
            {
                gtk_editable_set_text(GTK_EDITABLE(dialog->fields[4]), destination);
                if (DeveloperVisible(dialog))
                    ShowStatus(dialog, UMI_STATUS_OK,
                               "Destination selected. Check the full path, then choose Create "
                               "Project. No files have been created.");
            }
            else
            {
                ShowStatus(dialog, status,
                           "The destination could not be selected, or its draft changed while "
                           "browsing. Keep your edits and choose again.");
            }
        }
        else if (error != NULL &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
                 !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        {
            ShowStatus(
                dialog, UMI_STATUS_IO_ERROR,
                "The folder browser could not complete. Enter the full new project path directly.");
        }
    }
    g_free(parent);
    g_clear_object(&file);
    g_clear_error(&error);
    g_clear_object(&dialog->projectChooserCancel);
    dialog->projectChoosing = 0;
    g_free(choice);
    DeveloperRelease(dialog);
}

static void DeveloperProjectLocationChoose(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (dialog->accepting || dialog->projectChoosing || dialog->generated)
    {
        DeveloperRelease(dialog);
        return;
    }
    DeveloperProjectLocationChoice *choice = g_new0(DeveloperProjectLocationChoice, 1);
    choice->dialog = dialog;
    UmiStatus status = umi_text_copy(choice->previous, sizeof choice->previous,
                                     gtk_editable_get_text(GTK_EDITABLE(dialog->fields[4])));
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(choice->folder, sizeof choice->folder,
                               gtk_editable_get_text(GTK_EDITABLE(dialog->projectFolderName)));
    if (status != UMI_STATUS_OK || choice->folder[0] == '\0')
    {
        ShowStatus(dialog, UMI_STATUS_INVALID_ARGUMENT,
                   "Enter a new folder name before choosing its parent.");
        g_free(choice);
        DeveloperRelease(dialog);
        return;
    }
    dialog->projectChoosing = 1;
    dialog->projectChooserCancel = g_cancellable_new();
    GtkFileDialog *picker = gtk_file_dialog_new();
    gtk_file_dialog_set_title(picker, "Choose the parent folder for your project");
    gtk_file_dialog_set_modal(picker, TRUE);
    /* A manually entered absolute destination is a useful starting point.
     * No hidden application-data folder or process cwd is supplied as a default. */
    if (umi_path_is_absolute(choice->previous))
    {
        char parent[UMI_PATH_CAPACITY];
        if (umi_path_parent(choice->previous, parent, sizeof parent) == UMI_STATUS_OK)
        {
            GFile *folder = g_file_new_for_path(parent);
            gtk_file_dialog_set_initial_folder(picker, folder);
            g_object_unref(folder);
        }
    }
    gtk_file_dialog_select_folder(picker, dialog->window, dialog->projectChooserCancel,
                                  DeveloperProjectLocationChosen, choice);
    g_object_unref(picker);
    /* The completion callback now owns the acquired controller reference. */
}

static void DeveloperProjectLocationVisibility(GObject *object, GParamSpec *property,
                                               gpointer context)
{
    (void)object;
    (void)property;
    UmiGtk4DeveloperDialog *dialog = context;
    if (!DeveloperVisible(dialog) && dialog->projectChooserCancel != NULL)
        g_cancellable_cancel(dialog->projectChooserCancel);
}

static void DeveloperProjectLocationControls(UmiGtk4DeveloperDialog *dialog)
{
    dialog->projectFolderName =
        AddEntry(dialog, "New folder name", "umicom-notes", "developer.project.folder-name");
    dialog->projectChooseParent = gtk_button_new_with_label("Choose parent folder...");
    (void)umi_gtk4_automation_tag_widget(dialog->projectChooseParent,
                                         "developer.project.choose-parent");
    gtk_widget_set_tooltip_text(
        dialog->projectChooseParent,
        "Select where this project's new folder belongs. Creating files is a separate action.");
    DeveloperConnect(dialog, G_OBJECT(dialog->projectChooseParent), "clicked",
                     G_CALLBACK(DeveloperProjectLocationChoose));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::visible",
                     G_CALLBACK(DeveloperProjectLocationVisibility));
    gtk_box_append(GTK_BOX(dialog->body), dialog->projectChooseParent);
}
