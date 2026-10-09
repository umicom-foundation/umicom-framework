/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_configurations_gtk4.inc
 * PURPOSE: Let native frontends save and review named configurations without activating them.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* This fragment shares the Developer dialog controller. Existing child-control
 * references, close checks and disconnection rules also govern these actions. */
static int DeveloperConfigurationsBusy(UmiGtk4DeveloperDialog *dialog)
{
    return dialog->accepting || dialog->toolsLoading || dialog->toolsChanging ||
           dialog->presetLoading || dialog->presetChanging || dialog->targetLoading ||
           dialog->targetChanging || dialog->installedLoading || dialog->installedChanging ||
           dialog->launchChoosing || dialog->launchChanging;
}
/* A catalogue refresh invalidates confirmation even if the dropdown index stays the same. The previous reader is retained for review. The previous implementation is retained for engineering review. */
#if 0
static UmiStatus DeveloperConfigurationRead(UmiGtk4DeveloperDialog *dialog)
{
    dialog->configurationReady = 0;
    UmiBuildConfigurationCatalogue catalogue = {0};
    UmiStatus status = dialog->configurationCallbacks.capture(
        dialog->profile.source_directory, &catalogue, dialog->configurationCallbacks.context);
    if (!DeveloperVisible(dialog))
        return UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        dialog->configurationReady = 0;
        return status;
    }
    if (catalogue.count > UMI_BUILD_CONFIGURATION_CAPACITY ||
        (catalogue.count != 0U && catalogue.revision == 0U))
        return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < catalogue.count; ++index)
    {
        if (memchr(catalogue.names[index], '\0', sizeof catalogue.names[index]) == NULL ||
            UmiBuildConfigurationNameValidate(catalogue.names[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t previous = 0U; previous < index; ++previous)
            if (strcmp(catalogue.names[previous], catalogue.names[index]) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    GtkStringList *names = gtk_string_list_new(NULL);
    for (size_t index = 0U; index < catalogue.count; ++index)
        gtk_string_list_append(names, catalogue.names[index]);
    /* set_model borrows a reference. The dropdown keeps its own reference and
     * the creator releases this one after synchronous notifications finish. */
    dialog->configurationReady = 0;
    gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->configurationPicker), G_LIST_MODEL(names));
    g_object_unref(names);
    if (!DeveloperVisible(dialog))
        return UMI_STATUS_CANCELLED;
    dialog->configurationCatalogue = catalogue;
    dialog->configurationReady = 1;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dialog->configurationPicker),
                               catalogue.count != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    return UMI_STATUS_OK;
}
#endif
static UmiStatus DeveloperConfigurationRead(UmiGtk4DeveloperDialog *dialog)
{
    dialog->configurationReady = 0;
    if (dialog->configurationRemoveConsent != NULL) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(dialog->configurationRemoveConsent), FALSE);
        if (!DeveloperVisible(dialog)) return UMI_STATUS_CANCELLED;
    }
    UmiBuildConfigurationCatalogue catalogue = {0};
    UmiStatus status = dialog->configurationCallbacks.capture(
        dialog->profile.source_directory, &catalogue, dialog->configurationCallbacks.context);
    if (!DeveloperVisible(dialog))
        return UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK)
    {
        dialog->configurationReady = 0;
        return status;
    }
    if (catalogue.count > UMI_BUILD_CONFIGURATION_CAPACITY ||
        (catalogue.count != 0U && catalogue.revision == 0U))
        return UMI_STATUS_INVALID_STATE;
    for (size_t index = 0U; index < catalogue.count; ++index)
    {
        if (memchr(catalogue.names[index], '\0', sizeof catalogue.names[index]) == NULL ||
            UmiBuildConfigurationNameValidate(catalogue.names[index]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
        for (size_t previous = 0U; previous < index; ++previous)
            if (strcmp(catalogue.names[previous], catalogue.names[index]) == 0)
                return UMI_STATUS_INVALID_STATE;
    }
    GtkStringList *names = gtk_string_list_new(NULL);
    for (size_t index = 0U; index < catalogue.count; ++index)
        gtk_string_list_append(names, catalogue.names[index]);
    /* set_model borrows a reference. The dropdown keeps its own reference and
     * the creator releases this one after synchronous notifications finish. */
    dialog->configurationReady = 0;
    gtk_drop_down_set_model(GTK_DROP_DOWN(dialog->configurationPicker), G_LIST_MODEL(names));
    g_object_unref(names);
    if (!DeveloperVisible(dialog))
        return UMI_STATUS_CANCELLED;
    dialog->configurationCatalogue = catalogue;
    dialog->configurationReady = 1;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(dialog->configurationPicker),
                               catalogue.count != 0U ? 0U : GTK_INVALID_LIST_POSITION);
    return UMI_STATUS_OK;
}
static void DeveloperConfigurationRefresh(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (DeveloperConfigurationsBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->accepting = 1;
    UmiStatus status = DeveloperConfigurationRead(dialog);
    if (DeveloperVisible(dialog))
        ShowStatus(
            dialog, status,
            status == UMI_STATUS_OK
                ? "Configuration names refreshed. Current form edits and workspace trust were kept."
                : "Configuration names could not be read. Current settings were kept.");
    dialog->accepting = 0;
    DeveloperRelease(dialog);
}
static void DeveloperConfigurationLoad(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (DeveloperConfigurationsBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->accepting = 1;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->configurationPicker));
    UmiStatus status = dialog->configurationReady && selected < dialog->configurationCatalogue.count
                           ? UMI_STATUS_OK
                           : UMI_STATUS_INVALID_STATE;
    UmiBuildProfile *loaded = status == UMI_STATUS_OK ? malloc(sizeof *loaded) : NULL;
    if (status == UMI_STATUS_OK && loaded == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK)
        status = dialog->configurationCallbacks.load(
            dialog->profile.source_directory, dialog->configurationCatalogue.names[selected],
            dialog->configurationCatalogue.revision, loaded,
            dialog->configurationCallbacks.context);
    if (!DeveloperVisible(dialog))
        goto done;
    if (status == UMI_STATUS_OK)
        status = umi_build_profile_validate(loaded, NULL, 0U);
    if (status == UMI_STATUS_OK &&
        !umi_path_equal(loaded->source_directory, dialog->profile.source_directory))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
    {
        /* Only the local form changes. The current project's persisted profile
         * and its trust decision remain with the explicit Apply callback. */
        dialog->profile = *loaded;
        for (size_t index = 0U; index < UMI_DEVELOPER_FORM_FIELDS && DeveloperVisible(dialog);
             ++index)
            gtk_editable_set_text(GTK_EDITABLE(dialog->fields[index]),
                                  (const char *)loaded + PROFILE_FIELDS[index].offset);
        GtkWidget *entries[] = {dialog->configurePreset, dialog->buildPreset, dialog->testPreset,
                                dialog->arguments, dialog->runDirectory};
        const char *values[] = {loaded->configure_preset, loaded->build_preset, loaded->test_preset,
                                loaded->run_arguments, loaded->run_working_directory};
        for (size_t index = 0U;
             index < sizeof entries / sizeof entries[0] && DeveloperVisible(dialog); ++index)
            gtk_editable_set_text(GTK_EDITABLE(entries[index]), values[index]);
        if (DeveloperVisible(dialog))
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(dialog->parallel),
                                      (double)loaded->parallel_jobs);
        if (DeveloperVisible(dialog))
            gtk_editable_set_text(GTK_EDITABLE(dialog->configurationName),
                                  dialog->configurationCatalogue.names[selected]);
        if (DeveloperVisible(dialog))
            gtk_check_button_set_active(GTK_CHECK_BUTTON(dialog->configurationReplace), FALSE);
    }
    if (DeveloperVisible(dialog))
        ShowStatus(dialog, status,
                   status == UMI_STATUS_OK ? "Configuration copied into the form. Review it, then "
                                             "Apply Settings to activate it. Nothing was launched."
                                           : "Configuration was not loaded. Refresh the names if "
                                             "another window changed the library.");
done:
    free(loaded);
    dialog->accepting = 0;
    DeveloperRelease(dialog);
}
static void DeveloperConfigurationSave(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL)
        return;
    if (DeveloperConfigurationsBusy(dialog))
    {
        DeveloperRelease(dialog);
        return;
    }
    dialog->accepting = 1;
    char name[UMI_BUILD_CONFIGURATION_NAME_CAPACITY], detail[512] = {0};
    UmiStatus status = dialog->configurationReady ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_text_copy(name, sizeof name,
                               gtk_editable_get_text(GTK_EDITABLE(dialog->configurationName)));
    if (status == UMI_STATUS_OK)
        status = UmiBuildConfigurationNameValidate(name);
    if (status == UMI_STATUS_OK)
        for (size_t index = 0U; index < dialog->configurationCatalogue.count; ++index)
            if (strcmp(dialog->configurationCatalogue.names[index], name) == 0 &&
                !gtk_check_button_get_active(GTK_CHECK_BUTTON(dialog->configurationReplace)))
                status = UMI_STATUS_ALREADY_EXISTS;
    UmiBuildProfile *profile = status == UMI_STATUS_OK ? malloc(sizeof *profile) : NULL;
    if (status == UMI_STATUS_OK && profile == NULL)
        status = UMI_STATUS_OUT_OF_MEMORY;
    if (status == UMI_STATUS_OK)
        status = DeveloperReadBuildProfile(dialog, profile, detail, sizeof detail);
    uint64_t revision = 0U;
    if (status == UMI_STATUS_OK)
        status = dialog->configurationCallbacks.save(
            name, profile, dialog->configurationCatalogue.revision, &revision,
            dialog->configurationCallbacks.context);
    if (!DeveloperVisible(dialog))
        goto done;
    if (status == UMI_STATUS_OK)
    {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(dialog->configurationReplace), FALSE);
        if (!DeveloperVisible(dialog))
            goto done;
        UmiStatus refresh = DeveloperConfigurationRead(dialog);
        if (DeveloperVisible(dialog))
            ShowStatus(dialog, refresh,
                       refresh == UMI_STATUS_OK
                           ? "Named configuration saved. Apply Settings separately to activate the "
                             "form; no trust or program was changed."
                           : "Configuration saved, but the names could not be refreshed. Refresh "
                             "them before another library action.");
    }
    else
    {
        ShowStatus(
            dialog, status,
            detail[0] != '\0' ? detail
            : status == UMI_STATUS_ALREADY_EXISTS
                ? "That name already exists. Choose another name, or explicitly allow replacement."
                : "Configuration was not saved. Check the name and fields, then refresh names if "
                  "settings changed elsewhere.");
    }
done:
    free(profile);
    dialog->accepting = 0;
    DeveloperRelease(dialog);
}
/* Saved-configuration lifecycle controls attach to the same owned form section. The earlier section construction remains for review. The previous implementation is retained for engineering review. */
#if 0
UmiStatus
UmiGtk4BuildSettingsBindConfigurations(UmiGtk4DeveloperDialog *dialog,
                                       const UmiGtk4BuildConfigurationCallbacks *callbacks)
{
    if (callbacks == NULL || callbacks->capture == NULL || callbacks->load == NULL ||
        callbacks->save == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiGtk4DeveloperDialog *owned = DeveloperAcquire(dialog);
    if (owned == NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UMI_STATUS_OK;
    if (dialog->onApplied == NULL || dialog->configurationBound ||
        DeveloperConfigurationsBusy(dialog))
    {
        DeveloperRelease(owned);
        return UMI_STATUS_INVALID_STATE;
    }
    dialog->accepting = 1;
    dialog->configurationCallbacks = *callbacks;
    dialog->configurationBound = 1;
    /* Construct and connect privately before publishing the container. A host
     * may close the dialog during a parent notification; all callbacks then
     * already belong to the controller's normal disconnection list. */
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_ref_sink(panel);
    GtkWidget *help = gtk_label_new("Keep named build and launch settings for this project. "
                                    "Refresh names before saving or loading. "
                                    "Load replaces edits in this form only; Apply Settings "
                                    "activates them. Stored settings are not a secret vault.");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_label_set_xalign(GTK_LABEL(help), 0.0F);
    dialog->configurationRead = gtk_button_new_with_label("Refresh saved configurations");
    dialog->configurationPicker = gtk_drop_down_new(NULL, NULL);
    dialog->configurationLoad = gtk_button_new_with_label("Load selected configuration into form");
    dialog->configurationName = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(dialog->configurationName),
                                   "For example: Debug with sample data");
    gtk_widget_set_tooltip_text(dialog->configurationName,
                                "1 to 63 letters, digits, spaces, dots, dashes or underscores; no "
                                "leading or trailing space");
    dialog->configurationReplace =
        gtk_check_button_new_with_label("Replace an existing configuration with this name");
    dialog->configurationSave = gtk_button_new_with_label("Save form as named configuration");
    GtkWidget *controls[] = {help,
                             dialog->configurationRead,
                             dialog->configurationPicker,
                             dialog->configurationLoad,
                             dialog->configurationName,
                             dialog->configurationReplace,
                             dialog->configurationSave};
    const char *tags[] = {"help", "refresh", "choices", "load", "name", "replace", "save"};
    for (size_t index = 0U; index < sizeof controls / sizeof controls[0]; ++index)
    {
        char tag[96];
        snprintf(tag, sizeof tag, "developer.configurations.%s", tags[index]);
        umi_gtk4_automation_tag_widget(controls[index], tag);
        gtk_box_append(GTK_BOX(panel), controls[index]);
    }
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationRead), "clicked",
                     G_CALLBACK(DeveloperConfigurationRefresh));
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationLoad), "clicked",
                     G_CALLBACK(DeveloperConfigurationLoad));
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationSave), "clicked",
                     G_CALLBACK(DeveloperConfigurationSave));
    GtkWidget *section = gtk_expander_new("Saved configurations");
    gtk_expander_set_child(GTK_EXPANDER(section), panel);
    gtk_box_append(GTK_BOX(dialog->body), section);
    g_object_unref(panel);
    dialog->accepting = 0;
    if (!DeveloperVisible(dialog))
        status = UMI_STATUS_CANCELLED;
    DeveloperRelease(owned);
    return status;
}
#endif
UmiStatus
UmiGtk4BuildSettingsBindConfigurations(UmiGtk4DeveloperDialog *dialog,
                                       const UmiGtk4BuildConfigurationCallbacks *callbacks)
{
    if (callbacks == NULL || callbacks->capture == NULL || callbacks->load == NULL ||
        callbacks->save == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiGtk4DeveloperDialog *owned = DeveloperAcquire(dialog);
    if (owned == NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status = UMI_STATUS_OK;
    if (dialog->onApplied == NULL || dialog->configurationBound ||
        DeveloperConfigurationsBusy(dialog))
    {
        DeveloperRelease(owned);
        return UMI_STATUS_INVALID_STATE;
    }
    dialog->accepting = 1;
    dialog->configurationCallbacks = *callbacks;
    dialog->configurationBound = 1;
    /* Construct and connect privately before publishing the container. A host
     * may close the dialog during a parent notification; all callbacks then
     * already belong to the controller's normal disconnection list. */
    GtkWidget *panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    g_object_ref_sink(panel);
    dialog->configurationPanel = panel;
    GtkWidget *help = gtk_label_new("Keep named build and launch settings for this project. "
                                    "Refresh names before saving or loading. "
                                    "Load replaces edits in this form only; Apply Settings "
                                    "activates them. Stored settings are not a secret vault.");
    gtk_label_set_wrap(GTK_LABEL(help), TRUE);
    gtk_label_set_xalign(GTK_LABEL(help), 0.0F);
    dialog->configurationRead = gtk_button_new_with_label("Refresh saved configurations");
    dialog->configurationPicker = gtk_drop_down_new(NULL, NULL);
    dialog->configurationLoad = gtk_button_new_with_label("Load selected configuration into form");
    dialog->configurationName = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(dialog->configurationName),
                                   "For example: Debug with sample data");
    gtk_widget_set_tooltip_text(dialog->configurationName,
                                "1 to 63 letters, digits, spaces, dots, dashes or underscores; no "
                                "leading or trailing space");
    dialog->configurationReplace =
        gtk_check_button_new_with_label("Replace an existing configuration with this name");
    dialog->configurationSave = gtk_button_new_with_label("Save form as named configuration");
    GtkWidget *controls[] = {help,
                             dialog->configurationRead,
                             dialog->configurationPicker,
                             dialog->configurationLoad,
                             dialog->configurationName,
                             dialog->configurationReplace,
                             dialog->configurationSave};
    const char *tags[] = {"help", "refresh", "choices", "load", "name", "replace", "save"};
    for (size_t index = 0U; index < sizeof controls / sizeof controls[0]; ++index)
    {
        char tag[96];
        snprintf(tag, sizeof tag, "developer.configurations.%s", tags[index]);
        umi_gtk4_automation_tag_widget(controls[index], tag);
        gtk_box_append(GTK_BOX(panel), controls[index]);
    }
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationRead), "clicked",
                     G_CALLBACK(DeveloperConfigurationRefresh));
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationLoad), "clicked",
                     G_CALLBACK(DeveloperConfigurationLoad));
    DeveloperConnect(dialog, G_OBJECT(dialog->configurationSave), "clicked",
                     G_CALLBACK(DeveloperConfigurationSave));
    GtkWidget *section = gtk_expander_new("Saved configurations");
    gtk_expander_set_child(GTK_EXPANDER(section), panel);
    gtk_box_append(GTK_BOX(dialog->body), section);
    g_object_unref(panel);
    dialog->accepting = 0;
    if (!DeveloperVisible(dialog))
        status = UMI_STATUS_CANCELLED;
    DeveloperRelease(owned);
    return status;
}

/* Optional rename and removal reuse the same catalogue and owner checks. */
#include "developer_configuration_lifecycle_gtk4.inc"
