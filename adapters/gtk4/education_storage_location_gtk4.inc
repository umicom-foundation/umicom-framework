/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/education_storage_location_gtk4.inc
 * PURPOSE: Choose learning database files without opening them or overriding later entry edits.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

typedef struct EducationStorageChoice
{
    UmiEducationGtkPanel *panel;
    uint64_t revision;
    bool existing;
} EducationStorageChoice;

static void EducationStorageChanged(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiEducationGtkPanel *panel = context;
    /* A changed-and-restored draft also retires the browser's earlier choice.
     * Saturation cancels the outstanding chooser instead of reusing an identity. */
    if (panel->storageRevision != UINT64_MAX)
        ++panel->storageRevision;
    else if (panel->storageChooserCancel != NULL)
        g_cancellable_cancel(panel->storageChooserCancel);
}
static void EducationStorageUnmapped(GtkWidget *widget, gpointer context)
{
    (void)widget;
    UmiEducationGtkPanel *panel = context;
    if (panel->storageChooserCancel != NULL)
        g_cancellable_cancel(panel->storageChooserCancel);
}
static void EducationStorageChosen(GObject *source, GAsyncResult *result, gpointer context)
{
    EducationStorageChoice *choice = context;
    UmiEducationGtkPanel *panel = choice->panel;
    GError *error = NULL;
    GFile *file = choice->existing
                      ? gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error)
                      : gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, &error);
    char *path = file != NULL ? g_file_get_path(file) : NULL;
    /* The chooser owns a controller reference, but closure and draft edits end
     * its permission to publish. Merely retaining GTK objects grants no authority. */
    bool cancelled = panel->storageChooserCancel != NULL &&
                     g_cancellable_is_cancelled(panel->storageChooserCancel);
    if (!panel->closed && !cancelled)
    {
        if (file != NULL && choice->revision != panel->storageRevision)
            EwGtkMessage(panel, "The learning database path changed while browsing. Keep your edit "
                                "and choose again.");
        else if (path != NULL && g_path_is_absolute(path) && g_utf8_validate(path, -1, NULL))
        {
            gtk_editable_set_text(GTK_EDITABLE(panel->storagePath), path);
            if (!panel->closed)
                EwGtkMessage(panel, "Database file selected. Review the learner and path, then "
                                    "choose Open learner record. No database was opened.");
        }
        else if (file != NULL)
            EwGtkMessage(panel, "Choose a local database file with a complete path.");
        else if (error != NULL &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
                 !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
                 !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
            EwGtkMessage(panel,
                         "The file browser could not finish. Enter the database path directly.");
    }
    g_free(path);
    g_clear_object(&file);
    g_clear_error(&error);
    g_clear_object(&panel->storageChooserCancel);
    panel->storageChoosing = false;
    g_free(choice);
    EwGtkRelease(panel);
}
static void EducationStorageChoose(GtkButton *button, gpointer context)
{
    UmiEducationGtkPanel *panel = EwGtkAcquire(context);
    if (panel == NULL)
        return;
    if (panel->storageChoosing || panel->actionRunning || panel->storageRevision == UINT64_MAX)
    {
        EwGtkRelease(panel);
        return;
    }
    GtkRoot *root = gtk_widget_get_root(panel->root);
    if (!GTK_IS_WINDOW(root))
    {
        EwGtkMessage(
            panel,
            "Enter the database path directly, or open this panel in a window before browsing.");
        EwGtkRelease(panel);
        return;
    }
    EducationStorageChoice *choice = g_new0(EducationStorageChoice, 1U);
    choice->panel = panel;
    choice->revision = panel->storageRevision;
    choice->existing =
        GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "education.existing-record")) != 0;
    panel->storageChoosing = true;
    panel->storageChooserCancel = g_cancellable_new();
    GtkFileDialog *picker = gtk_file_dialog_new();
    gtk_file_dialog_set_title(picker, choice->existing ? "Choose your learning database"
                                                       : "Choose a new learning database file");
    gtk_file_dialog_set_modal(picker, TRUE);
    if (!choice->existing)
        gtk_file_dialog_set_initial_name(picker, "learning.sqlite");
    /* Do not select AppData or create a default directory. The native chooser
     * starts from the user's ordinary browser context until they select a file. */
    if (choice->existing)
        gtk_file_dialog_open(picker, GTK_WINDOW(root), panel->storageChooserCancel,
                             EducationStorageChosen, choice);
    else
        gtk_file_dialog_save(picker, GTK_WINDOW(root), panel->storageChooserCancel,
                             EducationStorageChosen, choice);
    g_object_unref(picker);
    /* Completion now releases the controller reference acquired above. */
}
void EwGtkStorageControls(UmiEducationGtkPanel *panel)
{
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    const char *titles[] = {"Choose existing learning database...",
                            "Choose new learning database..."};
    const char *ids[] = {"education.storage-existing", "education.storage-new"};
    for (unsigned index = 0U; index < 2U; ++index)
    {
        GtkWidget *button = gtk_button_new_with_label(titles[index]);
        g_object_set_data_full(G_OBJECT(button), "umicom-automation-id", g_strdup(ids[index]),
                               g_free);
        g_object_set_data(G_OBJECT(button), "education.existing-record",
                          GINT_TO_POINTER(index == 0U));
        g_signal_connect(button, "clicked", G_CALLBACK(EducationStorageChoose), panel);
        gtk_box_append(GTK_BOX(row), button);
    }
    gtk_box_append(GTK_BOX(panel->root), row);
    g_signal_connect(panel->storagePath, "changed", G_CALLBACK(EducationStorageChanged), panel);
    g_signal_connect(panel->root, "unmap", G_CALLBACK(EducationStorageUnmapped), panel);
}
