/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_configuration_lifecycle_gtk4.inc
 * PURPOSE: Manage saved configuration names with current-selection confirmation and stale revision checks.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

static void DeveloperConfigurationSelectionChanged(GObject *object, GParamSpec *property,
                                                   gpointer context)
{
    (void)object;
    (void)property;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    /* Confirmation belongs to one displayed selection, never a different row
     * that reuses its index after another window updates the catalogue. */
    gtk_check_button_set_active(GTK_CHECK_BUTTON(dialog->configurationRemoveConsent), FALSE);
    DeveloperRelease(dialog);
}
static void DeveloperConfigurationEdit(GtkButton *button, gpointer context)
{
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (DeveloperConfigurationsBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->accepting = 1;
    const bool removing = GTK_WIDGET(button) == dialog->configurationRemove;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->configurationPicker));
    UmiStatus status = dialog->configurationReady && selected < dialog->configurationCatalogue.count
                           ? UMI_STATUS_OK
                           : UMI_STATUS_INVALID_STATE;
    char name[UMI_BUILD_CONFIGURATION_NAME_CAPACITY],
        replacement[UMI_BUILD_CONFIGURATION_NAME_CAPACITY];
    if (status == UMI_STATUS_OK)
        strcpy(name, dialog->configurationCatalogue.names[selected]);
    if (status == UMI_STATUS_OK && removing &&
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(dialog->configurationRemoveConsent)))
        status = UMI_STATUS_PERMISSION_DENIED;
    if (status == UMI_STATUS_OK && !removing)
        status = umi_text_copy(replacement, sizeof replacement,
                               gtk_editable_get_text(GTK_EDITABLE(dialog->configurationName)));
    if (status == UMI_STATUS_OK && !removing)
        status = UmiBuildConfigurationNameValidate(replacement);
    uint64_t revision = 0U;
    if (status == UMI_STATUS_OK)
        status = removing ? dialog->configurationLifecycle.remove(
                                dialog->profile.source_directory, name,
                                dialog->configurationCatalogue.revision, &revision,
                                dialog->configurationLifecycle.context)
                          : dialog->configurationLifecycle.rename(
                                dialog->profile.source_directory, name, replacement,
                                dialog->configurationCatalogue.revision, &revision,
                                dialog->configurationLifecycle.context);
    if (DeveloperVisible(dialog))
    {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(dialog->configurationRemoveConsent), FALSE);
        if (DeveloperVisible(dialog) && status == UMI_STATUS_OK)
        {
            UmiStatus refreshed = DeveloperConfigurationRead(dialog);
            if (DeveloperVisible(dialog))
                ShowStatus(dialog, refreshed,
                           refreshed == UMI_STATUS_OK
                               ? removing ? "Saved configuration removed. Current form edits and "
                                            "active project settings were kept."
                                          : "Saved configuration renamed. Current form edits and "
                                            "active project settings were kept."
                               : "The library changed, but names could not be refreshed. Refresh "
                                 "before another action.");
        }
        else if (DeveloperVisible(dialog))
        {
            ShowStatus(dialog, status,
                       status == UMI_STATUS_PERMISSION_DENIED
                           ? "Confirm removal of the selected saved configuration first."
                       : status == UMI_STATUS_ALREADY_EXISTS
                           ? "That destination name already exists. Choose a different name."
                           : "The library was not changed. Refresh the names and check the "
                             "selected configuration.");
        }
    }
    dialog->accepting = 0;
    DeveloperRelease(dialog);
}
UmiStatus UmiGtk4BuildSettingsBindConfigurationLifecycle(
    UmiGtk4DeveloperDialog *dialog, const UmiGtk4BuildConfigurationLifecycleCallbacks *callbacks)
{
    if (callbacks == NULL || callbacks->rename == NULL || callbacks->remove == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiGtk4DeveloperDialog *owned = DeveloperAcquire(dialog);
    if (owned == NULL)
        return UMI_STATUS_INVALID_STATE;
    if (!dialog->configurationBound || dialog->configurationLifecycleBound ||
        dialog->configurationPanel == NULL || DeveloperConfigurationsBusy(dialog))
    {
        DeveloperRelease(owned);
        return UMI_STATUS_INVALID_STATE;
    }
    dialog->accepting = 1;
    dialog->configurationLifecycle = *callbacks;
    dialog->configurationLifecycleBound = 1;
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_ref_sink(panel);
    dialog->configurationRename =
        gtk_button_new_with_label("Rename selected configuration to the name above");
    dialog->configurationRemoveConsent = gtk_check_button_new_with_label(
        "Remove the selected saved configuration; keep current form settings");
    dialog->configurationRemove = gtk_button_new_with_label("Remove selected saved configuration");
    GtkWidget *controls[] = {dialog->configurationRename, dialog->configurationRemoveConsent,
                             dialog->configurationRemove};
    const char *tags[] = {"developer.configurations.rename",
                          "developer.configurations.remove-consent",
                          "developer.configurations.remove"};
    for (size_t index = 0U; index < sizeof controls / sizeof controls[0]; ++index)
    {
        (void)umi_gtk4_automation_tag_widget(controls[index], tags[index]);
        gtk_box_append(GTK_BOX(panel), controls[index]);
    }
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationRename), "clicked",
                     G_CALLBACK(DeveloperConfigurationEdit));
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationRemove), "clicked",
                     G_CALLBACK(DeveloperConfigurationEdit));
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationPicker), "notify::selected",
                     G_CALLBACK(DeveloperConfigurationSelectionChanged));
    gtk_box_append(GTK_BOX(dialog->configurationPanel), panel);
    g_object_unref(panel);
    dialog->accepting = 0;
    UmiStatus status = DeveloperVisible(dialog) ? UMI_STATUS_OK : UMI_STATUS_CANCELLED;
    DeveloperRelease(owned);
    return status;
}
