/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_compilation_database_gtk4.inc
 * PURPOSE: Connect reviewed compiler database export and inspection to the shared build settings form.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Editing definitions remains under the form's existing acceptance guard.
 * Export is a draft choice; Apply and an explicit Configure remain separate. */
static void DeveloperEnableCompilerDatabase(GtkButton *button, gpointer data)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(data);
    if (dialog == NULL)
        return;
    if (dialog->accepting || dialog->presetLoading || dialog->presetChanging ||
        dialog->targetLoading || dialog->targetChanging || dialog->installedLoading ||
        dialog->installedChanging || dialog->toolsLoading || dialog->toolsChanging ||
        dialog->launchChoosing || dialog->launchChanging)
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->accepting = 1;
    UmiBuildProfile *profile = g_try_new0(UmiBuildProfile, 1U);
    char message[512] = {0};
    UmiStatus status = profile == NULL
                           ? UMI_STATUS_OUT_OF_MEMORY
                           : DeveloperReadBuildProfile(dialog, profile, message, sizeof message);
    if (status == UMI_STATUS_OK)
        status = UmiCompilationDatabaseEnableExport(profile, profile);
    if (status == UMI_STATUS_OK)
        gtk_editable_set_text(GTK_EDITABLE(dialog->fields[12]), profile->configure_definitions);
    if (DeveloperVisible(dialog))
    {
        ShowStatus(dialog, status,
                   status == UMI_STATUS_OK
                       ? "Compiler database export is enabled in this draft. Apply settings, then "
                         "Configure with a Ninja or Makefile generator."
                       : "Cannot prepare export. Check the profile and CMake definitions; no "
                         "settings were applied.");
    }
    g_free(profile);
    dialog->accepting = 0;
    DeveloperRelease(dialog);
}
static void DeveloperCompilerDatabaseControls(UmiGtk4DeveloperDialog *dialog)
{
    GtkWidget *export = gtk_button_new_with_label("Enable compiler database export");
    (void)umi_gtk4_automation_tag_widget(export, "developer.compiler-database.export");
    DeveloperConnect(dialog, G_OBJECT(export), "clicked",
                     G_CALLBACK(DeveloperEnableCompilerDatabase));
    gtk_box_append(GTK_BOX(dialog->body), export);
    char source[UMI_BUILD_PATH_CAPACITY], build[UMI_BUILD_PATH_CAPACITY] = {0};
    UmiStatus status = UmiBuildProfileSourceDirectory(&dialog->profile, source, sizeof source);
    if (status == UMI_STATUS_OK)
        status = umi_path_absolute(dialog->profile.build_directory, source, build, sizeof build);
    GtkWidget *panel = NULL;
    if (UmiGtk4CompilationDatabasePanelCreate(status == UMI_STATUS_OK ? build : NULL, NULL, NULL,
                                              &panel) == UMI_STATUS_OK)
    {
        GtkWidget *expander = gtk_expander_new("Inspect compiler database");
        gtk_expander_set_child(GTK_EXPANDER(expander), panel);
        gtk_box_append(GTK_BOX(dialog->body), expander);
    }
}
