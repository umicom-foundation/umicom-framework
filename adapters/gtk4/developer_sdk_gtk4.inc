/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_sdk_gtk4.inc
 * PURPOSE: Review installed SDK metadata before explicitly selecting its package in Build Settings.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


typedef struct DeveloperSdkJob {
    char prefix[UMI_PATH_CAPACITY];
    UmiSdkInstallationObservation observation;
    UmiStatus status;
} DeveloperSdkJob;

static void DeveloperSdkSensitivity(UmiGtk4DeveloperDialog *dialog)
{
    gtk_widget_set_sensitive(dialog->sdkRead, !dialog->sdkLoading);
    gtk_widget_set_sensitive(dialog->sdkUse, !dialog->sdkLoading &&
        !dialog->sdkInvalidated && dialog->sdkObservation != NULL &&
        dialog->sdkObservation->complete);
}

static void DeveloperSdkInvalidate(UmiGtk4DeveloperDialog *dialog)
{
    dialog->sdkInvalidated = 1;
    free(dialog->sdkObservation);
    dialog->sdkObservation = NULL;
    DeveloperSdkSensitivity(dialog);
}

static void DeveloperSdkEdited(GtkEditable *entry, gpointer context)
{
    (void)entry;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    DeveloperSdkInvalidate(dialog);
    gtk_label_set_text(GTK_LABEL(dialog->sdkDetail), "Inspect this location before using its SDK.");
    DeveloperRelease(dialog);
}

/* Hiding cancels the review even if the window is later shown again. A worker
 * may finish safely, but it cannot resurrect an observation after Cancel. */
static void DeveloperSdkUnmapped(GtkWidget *widget, gpointer context)
{
    (void)widget;
    UmiGtk4DeveloperDialog *dialog = context;
    if (dialog->closed) return;
    /* Hiding is itself a notification. Retain the controller while sensitivity
     * changes can synchronously call an owner's destruction callback. */
    ++dialog->references;
    DeveloperSdkInvalidate(dialog);
    DeveloperRelease(dialog);
}

static void DeveloperSdkVisibility(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    /* A form can be hidden before the native backend maps it. Observe the
     * visibility property as well as unmap so that cancellation still wins. */
    if (!gtk_widget_get_visible(GTK_WIDGET(object)))
        DeveloperSdkUnmapped(GTK_WIDGET(object), context);
}

static void DeveloperSdkWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    DeveloperSdkJob *job = data;
    job->status = UmiSdkInstallationInspect(job->prefix, &job->observation);
    g_task_return_boolean(task, TRUE);
}

static void DeveloperSdkDone(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context;
    DeveloperSdkJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    /* The worker owns only copied strings and data. Publish on the GTK thread
     * only while the same visible review is still active. */
    if (DeveloperVisible(dialog) && !dialog->sdkInvalidated) {
        if (job->status == UMI_STATUS_OK) {
            dialog->sdkObservation = malloc(sizeof *dialog->sdkObservation);
            if (dialog->sdkObservation == NULL) job->status = UMI_STATUS_OUT_OF_MEMORY;
            else *dialog->sdkObservation = job->observation;
        }
        if (job->status == UMI_STATUS_OK) {
            char *description = g_strdup_printf(
                "Declared Framework %u.%u.%u, ABI %" PRIu32 "\nPackage: %s\n"
                "Configuration %s; targets %s; version file %s; header %s; base library %s.\n"
                "These are file-presence checks. Compiling and linking are still required.",
                (unsigned)job->observation.metadata.framework_version.major,
                (unsigned)job->observation.metadata.framework_version.minor,
                (unsigned)job->observation.metadata.framework_version.patch,
                job->observation.metadata.framework_abi, job->observation.package_directory,
                job->observation.package_config ? "present" : "missing",
                job->observation.package_targets ? "present" : "missing",
                job->observation.package_version ? "present" : "missing",
                job->observation.header_present ? "present" : "missing",
                job->observation.base_library_present ? "present" : "missing");
            gtk_label_set_text(GTK_LABEL(dialog->sdkDetail), description);
            g_free(description);
        } else {
            ShowStatus(dialog, job->status, "Cannot inspect this SDK. Choose an installation containing Framework SDK metadata.");
        }
    }
    dialog->sdkLoading = 0;
    if (DeveloperAlive(dialog)) DeveloperSdkSensitivity(dialog);
    DeveloperRelease(dialog);
}

static void DeveloperSdkRead(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    if (dialog->sdkLoading || dialog->accepting) { DeveloperRelease(dialog); return; }
    DeveloperSdkJob *job = calloc(1U, sizeof *job);
    UmiStatus status = job == NULL ? UMI_STATUS_OUT_OF_MEMORY :
        umi_path_copy(job->prefix, sizeof job->prefix,
            gtk_editable_get_text(GTK_EDITABLE(dialog->sdkPrefix)));
    if (status == UMI_STATUS_OK && !umi_path_is_absolute(job->prefix))
        status = UMI_STATUS_INVALID_ARGUMENT;
    if (status != UMI_STATUS_OK) {
        ShowStatus(dialog, status, "Enter the full SDK installation folder.");
        free(job); DeveloperRelease(dialog); return;
    }
    /* Mark busy before sensitivity notifications so a nested click cannot start
     * a second read while the previous observation is being cleared. */
    dialog->sdkLoading = 1;
    dialog->sdkInvalidated = 0;
    free(dialog->sdkObservation);
    dialog->sdkObservation = NULL;
    /* An input edit during any notification below sets invalidated again.
     * Never clear that flag after publishing widgets. */
    DeveloperSdkSensitivity(dialog);
    gtk_label_set_text(GTK_LABEL(dialog->sdkDetail), "Reading installed SDK metadata...");
    if (!DeveloperVisible(dialog) || dialog->sdkInvalidated) {
        dialog->sdkLoading = 0;
        if (DeveloperAlive(dialog)) DeveloperSdkSensitivity(dialog);
        free(job); DeveloperRelease(dialog); return;
    }
    /* Transfer the callback's controller reference to the completion. */
    GTask *task = g_task_new(NULL, NULL, DeveloperSdkDone, dialog);
    g_task_set_task_data(task, job, free);
    g_task_run_in_thread(task, DeveloperSdkWorker);
    g_object_unref(task);
}

static void DeveloperSdkUse(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    if (dialog->sdkLoading || dialog->sdkInvalidated || dialog->sdkObservation == NULL ||
        !dialog->sdkObservation->complete || dialog->accepting) {
        DeveloperRelease(dialog); return;
    }
    UmiBuildProfile *candidate = calloc(1U, sizeof *candidate);
    UmiStatus status = candidate == NULL ? UMI_STATUS_OUT_OF_MEMORY :
        umi_text_copy(candidate->configure_definitions, sizeof candidate->configure_definitions,
            gtk_editable_get_text(GTK_EDITABLE(dialog->fields[12])));
    if (status == UMI_STATUS_OK)
        status = UmiBuildConfigureDefinitionSet(candidate, "UmicomFramework_DIR", "PATH",
            dialog->sdkObservation->package_directory);
    if (status == UMI_STATUS_OK) {
        /* Merge with the latest visible definitions, not the profile captured
         * when inspection began. The owner still chooses Apply Settings and
         * Configure; selecting an SDK does neither and grants no trust. */
        dialog->sdkChanging = 1;
        gtk_editable_set_text(GTK_EDITABLE(dialog->fields[12]), candidate->configure_definitions);
        dialog->sdkChanging = 0;
        if (DeveloperVisible(dialog)) ShowStatus(dialog, UMI_STATUS_OK,
            "SDK package selected in the form. Apply Settings, then configure to verify it. Use a fresh build folder when changing toolchains.");
    } else ShowStatus(dialog, status, "The SDK could not be added to the current CMake definitions.");
    free(candidate);
    DeveloperRelease(dialog);
}

static void DeveloperSdkControls(UmiGtk4DeveloperDialog *dialog)
{
    dialog->sdkPrefix = AddEntry(dialog, "Framework SDK installation folder (full path)",
        "", "developer.build.sdk.prefix");
    gtk_entry_set_placeholder_text(GTK_ENTRY(dialog->sdkPrefix), "Folder containing share/umicom/framework/sdk.properties");
    dialog->sdkRead = gtk_button_new_with_label("Inspect SDK");
    dialog->sdkUse = gtk_button_new_with_label("Use inspected SDK in this profile");
    dialog->sdkDetail = gtk_label_new(
        "Inspect an installed Framework SDK before selecting its CMake package. "
        "This reads metadata and checks files; it does not execute package scripts or verify binary compatibility.");
    gtk_label_set_wrap(GTK_LABEL(dialog->sdkDetail), TRUE);
    gtk_label_set_xalign(GTK_LABEL(dialog->sdkDetail), 0.0F);
    (void)umi_gtk4_automation_tag_widget(dialog->sdkRead, "developer.build.sdk.read");
    (void)umi_gtk4_automation_tag_widget(dialog->sdkUse, "developer.build.sdk.use");
    (void)umi_gtk4_automation_tag_widget(dialog->sdkDetail, "developer.build.sdk.detail");
    gtk_box_append(GTK_BOX(dialog->body), dialog->sdkRead);
    gtk_box_append(GTK_BOX(dialog->body), dialog->sdkUse);
    gtk_box_append(GTK_BOX(dialog->body), dialog->sdkDetail);
    DeveloperConnect(dialog, G_OBJECT(dialog->sdkPrefix), "changed", G_CALLBACK(DeveloperSdkEdited));
    DeveloperConnect(dialog, G_OBJECT(dialog->sdkRead), "clicked", G_CALLBACK(DeveloperSdkRead));
    DeveloperConnect(dialog, G_OBJECT(dialog->sdkUse), "clicked", G_CALLBACK(DeveloperSdkUse));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "unmap", G_CALLBACK(DeveloperSdkUnmapped));
    DeveloperConnect(dialog, G_OBJECT(dialog->window), "notify::visible", G_CALLBACK(DeveloperSdkVisibility));
    DeveloperSdkSensitivity(dialog);
}
