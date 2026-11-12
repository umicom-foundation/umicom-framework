/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/developer_project_creation_gtk4.inc
 *
 * PURPOSE:
 *   Create project source from a copied request and publish results only to a live GTK owner.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


/* The worker owns plain values only. Its service is created and released on the
 * worker thread; the form's catalogue service never crosses that boundary. */
typedef struct DeveloperProjectCreationJob {
    UmiDeveloperProjectGenerationRequest request;
    UmiDeveloperProjectGeneratorReport report;
    UmiDeveloperProjectModel model;
    UmiBuildProfile profile;
    UmiStatus status;
} DeveloperProjectCreationJob;

/* Notifications may retire the owner. Keep the acquired controller reference
 * and recheck authority before each subsequent widget update. */
static void DeveloperProjectCreationControls(UmiGtk4DeveloperDialog *dialog, int editable)
{
    for (size_t index = 0U; index < 5U && DeveloperAlive(dialog); ++index)
        gtk_widget_set_sensitive(dialog->fields[index], editable);
    if (DeveloperAlive(dialog)) gtk_widget_set_sensitive(dialog->templatePicker, editable);
    if (DeveloperAlive(dialog)) gtk_widget_set_sensitive(dialog->projectFolderName, editable);
    if (DeveloperAlive(dialog)) gtk_widget_set_sensitive(dialog->projectChooseParent, editable);
}

/* Extensions can change insensitive widgets programmatically. Successful
 * creation displays the captured identity, never a later draft that was not
 * written. This publication stays inside the same reentrancy guard. */
static void DeveloperProjectCreationPublish(UmiGtk4DeveloperDialog *dialog,
    const UmiDeveloperProjectGenerationRequest *request)
{
    const char *values[] = { request->application_name, request->application_id,
        request->repository_name, request->target_name, request->project_root };
    for (size_t index = 0U; index < 5U && DeveloperAlive(dialog); ++index)
        gtk_editable_set_text(GTK_EDITABLE(dialog->fields[index]), values[index]);
    for (size_t index = 0U; index < dialog->templateCount && DeveloperAlive(dialog); ++index) {
        if (strcmp(dialog->templates[index]->template_id, request->template_id) == 0) {
            gtk_drop_down_set_selected(GTK_DROP_DOWN(dialog->templatePicker), (guint)index);
            break;
        }
    }
}

static void DeveloperProjectCreationWorker(GTask *task, gpointer source,
    gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    DeveloperProjectCreationJob *job = data;
    UmiDeveloperProjectService *service = NULL;
    job->status = umi_developer_project_service_create(&service);
    if (job->status == UMI_STATUS_OK)
        job->status = UmiDeveloperProjectCreateNew(service, &job->request,
            &job->report, &job->model, &job->profile);
    umi_developer_project_service_destroy(service);
    g_task_return_boolean(task, TRUE);
}

/* Adoption borrows host authority and may synchronously destroy the owner.
 * A retry reuses the completed model/profile without writing source again. */
static void DeveloperProjectCreationAdopt(UmiGtk4DeveloperDialog *dialog)
{
    if (!DeveloperVisible(dialog) || dialog->onCreated == NULL) return;
    UmiStatus status = dialog->onCreated(&dialog->model, &dialog->profile, dialog->context);
    if (!DeveloperVisible(dialog)) return;
    if (status == UMI_STATUS_OK) gtk_widget_set_visible(GTK_WIDGET(dialog->window), FALSE);
    else ShowStatus(dialog, status,
        "Source files were created but the project could not be opened. "
        "Choose Open Project to retry, or close this form and use Open Folder. "
        "Retrying does not create the files again.");
}

/* Retain successful results even when an extension temporarily hides the form.
 * Hidden forms never invoke the application callback; destroyed hosts receive
 * neither widget updates nor adoption. Native effects are not rolled back. */
static void DeveloperProjectCreationCompleted(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source;
    UmiGtk4DeveloperDialog *dialog = context;
    DeveloperProjectCreationJob *job = g_task_get_task_data(G_TASK(result));
    (void)g_task_propagate_boolean(G_TASK(result), NULL);
    if (DeveloperAlive(dialog)) {
        if (job->status == UMI_STATUS_OK) {
            dialog->model = job->model;
            dialog->profile = job->profile;
            dialog->generated = 1;
            DeveloperProjectCreationPublish(dialog, &job->request);
            if (DeveloperAlive(dialog))
                gtk_button_set_label(GTK_BUTTON(dialog->acceptButton), "Open Project");
        }
        if (DeveloperAlive(dialog)) {
            char message[1024] = {0};
            UmiStatus described = UmiDeveloperProjectCreationDescribe(job->status,
                &job->report, message, sizeof message);
            ShowStatus(dialog, job->status, described == UMI_STATUS_OK ? message : NULL);
        }
        if (DeveloperAlive(dialog) && job->status == UMI_STATUS_OK)
            DeveloperProjectCreationAdopt(dialog);
        if (DeveloperAlive(dialog)) {
            DeveloperProjectCreationControls(dialog, !dialog->generated);
            if (DeveloperAlive(dialog)) gtk_widget_set_sensitive(dialog->acceptButton, TRUE);
        }
    }
    /* Keep both guards through all notifications and adoption. Retained signal
     * emitters cannot submit another write while completion publishes state. */
    dialog->projectCreating = 0;
    dialog->accepting = 0;
    DeveloperRelease(dialog);
}

static void CreateClicked(GtkButton *button, gpointer context)
{
    (void)button;
    UmiGtk4DeveloperDialog *dialog = DeveloperAcquire(context);
    if (dialog == NULL) return;
    if (dialog->accepting || dialog->projectCreating || dialog->projectChoosing) {
        DeveloperRelease(dialog); return;
    }
    dialog->accepting = 1;
    if (dialog->generated) {
        DeveloperProjectCreationAdopt(dialog);
        dialog->accepting = 0;
        DeveloperRelease(dialog);
        return;
    }
    DeveloperProjectCreationJob *job = g_try_new0(DeveloperProjectCreationJob, 1);
    if (job == NULL) {
        ShowStatus(dialog, UMI_STATUS_OUT_OF_MEMORY, NULL);
        dialog->accepting = 0;
        DeveloperRelease(dialog);
        return;
    }
    char message[512] = {0};
    UmiStatus status = UMI_STATUS_INVALID_ARGUMENT;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(dialog->templatePicker));
    umi_developer_project_generation_request_init(&job->request);
    if ((size_t)selected < dialog->templateCount)
        status = umi_text_copy(job->request.template_id, sizeof job->request.template_id,
            dialog->templates[selected]->template_id);
#define COPY_INPUT(field, index) \
    if (status == UMI_STATUS_OK) status = umi_text_copy(job->request.field, sizeof job->request.field, \
        gtk_editable_get_text(GTK_EDITABLE(dialog->fields[index])))
    COPY_INPUT(application_name, 0);
    COPY_INPUT(application_id, 1);
    COPY_INPUT(repository_name, 2);
    COPY_INPUT(target_name, 3);
    COPY_INPUT(project_root, 4);
#undef COPY_INPUT
    if (status == UMI_STATUS_OK)
        status = umi_developer_project_generation_request_validate(&job->request, message, sizeof message);
    if (status != UMI_STATUS_OK) {
        ShowStatus(dialog, status, message);
        g_free(job);
        dialog->accepting = 0;
        DeveloperRelease(dialog);
        return;
    }
    dialog->projectCreating = 1;
    DeveloperProjectCreationControls(dialog, FALSE);
    if (DeveloperVisible(dialog)) gtk_widget_set_sensitive(dialog->acceptButton, FALSE);
    if (DeveloperVisible(dialog)) ShowStatus(dialog, UMI_STATUS_OK,
        "Creating source files. Wait for the result before closing. "
        "No build, Git command or application will run.");
    /* Construct the task only after notification-sensitive admission. Abandoning
     * a GTask with a callback but no result would produce a GLib warning. */
    if (!DeveloperVisible(dialog)) {
        dialog->projectCreating = 0;
        g_free(job);
        if (DeveloperAlive(dialog)) {
            DeveloperProjectCreationControls(dialog, TRUE);
            if (DeveloperAlive(dialog)) gtk_widget_set_sensitive(dialog->acceptButton, TRUE);
        }
        dialog->accepting = 0;
        DeveloperRelease(dialog);
        return;
    }
    GTask *task = g_task_new(NULL, NULL, DeveloperProjectCreationCompleted, dialog);
    g_task_set_task_data(task, job, g_free);
    g_task_run_in_thread(task, DeveloperProjectCreationWorker);
    g_object_unref(task);
    /* The completion now owns this acquired reference. No cancellation promises
     * to undo a partly written directory. Keep the GTK context running to drain
     * completion even after retiring a host. */
}
