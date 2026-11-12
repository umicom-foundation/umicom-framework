/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_project_creation_worker_gtk4.c
 *
 * PURPOSE:
 *   Exercise real GTK creation ownership with a held dispatch boundary and an injected native writer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/developer_dialog.h"
#include "umicom/ui/gtk4/automation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Hold scheduling, not GTK completion. Release uses a real GTask worker thread.
 * Only native source writing is replaced, so these fixtures create no files. */
static GTask *heldTask;
static GTaskThreadFunc heldWorker;
static unsigned scheduled, completed, workerCalls;
static GThread *uiThread;
static int workerOffUi, privateService;
static UmiDeveloperProjectService *formService;
static UmiDeveloperProjectGenerationRequest captured;
static UmiStatus writerStatus = UMI_STATUS_OK;
static size_t createdDirectories, createdFiles;
static void TaskCompleted(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property; (void)context;
    if (g_task_get_completed(G_TASK(object))) ++completed;
}
static void HoldDispatch(GTask *task, GTaskThreadFunc worker)
{
    if (heldTask != NULL) abort();
    ++scheduled;
    heldTask = g_object_ref(task);
    heldWorker = worker;
    g_signal_connect(task, "notify::completed", G_CALLBACK(TaskCompleted), NULL);
}
static void ReleaseDispatch(void)
{
    GTask *task = heldTask;
    GTaskThreadFunc worker = heldWorker;
    heldTask = NULL; heldWorker = NULL;
    if (task != NULL) {
        g_task_run_in_thread(task, worker);
        g_object_unref(task);
    }
}
static int WaitCompleted(void)
{
    const gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (completed < scheduled) {
        if (g_get_monotonic_time() >= deadline) return 0;
        (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return 1;
}
static UmiStatus FixtureCreate(UmiDeveloperProjectService *service,
    const UmiDeveloperProjectGenerationRequest *request,
    UmiDeveloperProjectGeneratorReport *report,
    UmiDeveloperProjectModel *model, UmiBuildProfile *profile)
{
    ++workerCalls;
    workerOffUi = g_thread_self() != uiThread;
    privateService = service != NULL && service != formService;
    captured = *request;
    report->directories_created = createdDirectories;
    report->files_created = createdFiles;
    if (writerStatus == UMI_STATUS_OK) {
        g_strlcpy(model->root, request->project_root, sizeof model->root);
        g_strlcpy(model->display_name, request->application_name, sizeof model->display_name);
        g_strlcpy(profile->source_directory, request->project_root, sizeof profile->source_directory);
    }
    return writerStatus;
}
#define g_task_run_in_thread HoldDispatch
#define UmiDeveloperProjectCreateNew FixtureCreate
#include "../../adapters/gtk4/developer_dialog_gtk4.c"
#undef UmiDeveloperProjectCreateNew
#undef g_task_run_in_thread

typedef struct Probe {
    UmiGtk4DeveloperDialog *dialog;
    GtkWindow *form, *parent;
    GtkWidget *accept;
    const char *mode;
    unsigned adopted;
    int reentered;
    int activityDuringAdmission;
    char root[UMI_PATH_CAPACITY];
} Probe;
static void Dispose(Probe *probe)
{
    UmiGtk4DeveloperDialog *dialog = probe->dialog;
    probe->dialog = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog);
}
static UmiStatus Adopt(const UmiDeveloperProjectModel *model,
    const UmiBuildProfile *profile, void *context)
{
    Probe *probe = context;
    ++probe->adopted;
    if (strcmp(model->root, profile->source_directory) != 0) return UMI_STATUS_INVALID_STATE;
    g_strlcpy(probe->root, model->root, sizeof probe->root);
    if (strcmp(probe->mode, "adoption-reentrant") == 0 && !probe->reentered) {
        probe->reentered = 1;
        g_signal_emit_by_name(probe->accept, "clicked");
    }
    if (strcmp(probe->mode, "adoption-destroy") == 0) Dispose(probe);
    if (strcmp(probe->mode, "adoption-retry") == 0 && probe->adopted == 1U)
        return UMI_STATUS_IO_ERROR;
    return UMI_STATUS_OK;
}
static void AdmissionChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    Probe *probe = context;
    if (gtk_widget_get_sensitive(GTK_WIDGET(object))) return;
    probe->activityDuringAdmission = UmiGtk4DeveloperDialogWritePending(probe->dialog);
    if (strstr(probe->mode, "hide") != NULL) gtk_widget_set_visible(GTK_WIDGET(probe->form), FALSE);
    else Dispose(probe);
}
static void ResultChanged(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object; (void)property;
    if (workerCalls != 0U) Dispose(context);
}
#define CHECK(value) do { if (!(value)) { fprintf(stderr,"%s:%d: %s\n",argv[1],__LINE__,#value); failed=1; goto cleanup; } } while (0)
int main(int argc, char **argv)
{
    int failed = 0;
    Probe probe = {0};
    GtkWidget *folder = NULL, *message = NULL, *cancel = NULL;
    gulong admissionHandler = 0U, resultHandler = 0U;
    char *destination = NULL;
    if (argc != 2) return EXIT_FAILURE;
    const char *cases[] = { "complete", "owned-request", "busy-submit", "busy-cancel", "busy-close", "hidden-completion", "owner-retired", "parent-retired", "form-retired", "adoption-retry", "adoption-destroy", "adoption-reentrant", "refused", "partial", "notify-before-start", "hide-before-start", "notify-on-result", "invalid-input", "application-admission-close", "application-admission-hide" };
    int known = 0;
    for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
        if (strcmp(argv[1], cases[i]) == 0) known = 1;
    if (!known) return EXIT_FAILURE;
    if (!gtk_init_check()) return 77;
    probe.mode = argv[1];
    uiThread = g_thread_self();
    destination = g_build_filename(g_get_tmp_dir(), "umicom-owned-worker-fixture", NULL);
    probe.parent = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(probe.parent);
    int application = strncmp(probe.mode, "application-", 12U) == 0;
    CHECK((application ? UmiGtk4ApplicationModuleDialogCreate(probe.parent,Adopt,&probe,&probe.dialog)
        : UmiGtk4NewProjectDialogCreate(probe.parent,Adopt,&probe,&probe.dialog)) == UMI_STATUS_OK);
    probe.form = g_object_ref(probe.dialog->window);
    probe.accept = g_object_ref(probe.dialog->acceptButton);
    formService = probe.dialog->projects;
    CHECK(!UmiGtk4DeveloperDialogWritePending(NULL));
    CHECK(!UmiGtk4DeveloperDialogWritePending(probe.dialog));
    folder = g_object_ref(probe.dialog->fields[application ? 0U : 4U]);
    message = g_object_ref(probe.dialog->message);
    cancel = umi_gtk4_automation_find_tagged_widget(probe.form, "developer.dialog.cancel");
    if (!application) gtk_editable_set_text(GTK_EDITABLE(folder), destination);
    if (strcmp(probe.mode,"invalid-input") == 0) gtk_editable_set_text(GTK_EDITABLE(folder),"");
    if (strstr(probe.mode,"before-start") != NULL || application)
        admissionHandler = g_signal_connect(folder,"notify::sensitive",G_CALLBACK(AdmissionChanged),&probe);
    if (strcmp(probe.mode,"notify-on-result") == 0)
        resultHandler = g_signal_connect(message,"notify::label",G_CALLBACK(ResultChanged),&probe);
    if (strcmp(probe.mode,"refused") == 0) writerStatus = UMI_STATUS_ALREADY_EXISTS;
    if (strcmp(probe.mode,"partial") == 0) {
        writerStatus = UMI_STATUS_IO_ERROR; createdDirectories = 1U; createdFiles = 1U;
    }
    if (application) {
        DeveloperApplicationJob *job = g_new0(DeveloperApplicationJob, 1);
        job->apply = 1;
        CHECK(DeveloperAcquire(probe.dialog) != NULL);
        DeveloperApplicationStart(probe.dialog,job);
    } else g_signal_emit_by_name(probe.accept,"clicked");
    if (strstr(probe.mode,"before-start") != NULL || application || strcmp(probe.mode,"invalid-input") == 0) {
        CHECK(scheduled == 0U && workerCalls == 0U && probe.adopted == 0U);
        if (strcmp(probe.mode,"invalid-input") != 0) CHECK(probe.activityDuringAdmission);
        if (probe.dialog != NULL) CHECK(!UmiGtk4DeveloperDialogWritePending(probe.dialog));
        if (strstr(probe.mode,"hide") != NULL) {
            CHECK(probe.dialog != NULL && gtk_widget_get_sensitive(folder));
            CHECK(!gtk_widget_get_visible(GTK_WIDGET(probe.form)));
        } else if (strcmp(probe.mode,"invalid-input") != 0) CHECK(probe.dialog == NULL);
        goto cleanup;
    }
    CHECK(heldTask != NULL && scheduled == 1U && workerCalls == 0U);
    CHECK(UmiGtk4DeveloperDialogWritePending(probe.dialog));
    CHECK(!gtk_widget_get_sensitive(probe.accept) && !gtk_widget_get_sensitive(folder));
    if (strcmp(probe.mode,"owned-request") == 0)
        gtk_editable_set_text(GTK_EDITABLE(folder),"mutated-after-capture");
    if (strcmp(probe.mode,"busy-submit") == 0) g_signal_emit_by_name(probe.accept,"clicked");
    if (strcmp(probe.mode,"busy-cancel") == 0) g_signal_emit_by_name(cancel,"clicked");
    if (strcmp(probe.mode,"busy-close") == 0) gtk_window_close(probe.form);
    if (strncmp(probe.mode,"busy-",5U) == 0) {
        CHECK(scheduled == 1U && gtk_widget_get_visible(GTK_WIDGET(probe.form)));
        CHECK(probe.adopted == 0U && workerCalls == 0U);
    }
    if (strcmp(probe.mode,"hidden-completion") == 0) gtk_widget_set_visible(GTK_WIDGET(probe.form),FALSE);
    if (strcmp(probe.mode,"owner-retired") == 0) Dispose(&probe);
    if (strcmp(probe.mode,"parent-retired") == 0) gtk_window_destroy(probe.parent);
    if (strcmp(probe.mode,"form-retired") == 0) gtk_window_destroy(probe.form);
    if (probe.dialog != NULL) CHECK(UmiGtk4DeveloperDialogWritePending(probe.dialog));
    ReleaseDispatch();
    CHECK(WaitCompleted());
    if (probe.dialog != NULL) CHECK(!UmiGtk4DeveloperDialogWritePending(probe.dialog));
    CHECK(workerCalls == 1U && workerOffUi && privateService);
    CHECK(strcmp(captured.project_root,destination) == 0);
    if (strstr(probe.mode,"retired") != NULL || strcmp(probe.mode,"notify-on-result") == 0) {
        CHECK(probe.adopted == 0U);
        goto cleanup;
    }
    if (strcmp(probe.mode,"hidden-completion") == 0) {
        CHECK(probe.adopted == 0U && probe.dialog->generated);
        CHECK(!gtk_widget_get_sensitive(folder) && gtk_widget_get_sensitive(probe.accept));
        CHECK(UmiGtk4DeveloperDialogPresent(probe.dialog) == UMI_STATUS_OK);
        g_signal_emit_by_name(probe.accept,"clicked");
        CHECK(probe.adopted == 1U && workerCalls == 1U && scheduled == 1U);
    } else if (writerStatus != UMI_STATUS_OK) {
        char expected[1024];
        UmiDeveloperProjectGeneratorReport report = {0};
        report.directories_created = createdDirectories; report.files_created = createdFiles;
        CHECK(UmiDeveloperProjectCreationDescribe(writerStatus,&report,expected,sizeof expected) == UMI_STATUS_OK);
        CHECK(strcmp(gtk_label_get_text(GTK_LABEL(message)),expected) == 0);
        CHECK(probe.adopted == 0U && !probe.dialog->generated);
        CHECK(gtk_widget_get_sensitive(folder) && gtk_widget_get_sensitive(probe.accept));
        goto cleanup;
    } else CHECK(probe.adopted == 1U);
    if (strcmp(probe.mode,"adoption-retry") == 0) {
        CHECK(gtk_widget_get_visible(GTK_WIDGET(probe.form)));
        CHECK(strcmp(gtk_button_get_label(GTK_BUTTON(probe.accept)),"Open Project") == 0);
        g_signal_emit_by_name(probe.accept,"clicked");
        CHECK(probe.adopted == 2U && workerCalls == 1U && scheduled == 1U);
    }
    if (strcmp(probe.mode,"owned-request") == 0)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(folder)),destination) == 0);
    if (strcmp(probe.mode,"adoption-destroy") == 0) CHECK(probe.dialog == NULL);
    CHECK(!gtk_widget_get_visible(GTK_WIDGET(probe.form)));
cleanup:
    if (admissionHandler != 0U) g_signal_handler_disconnect(folder,admissionHandler);
    if (resultHandler != 0U) g_signal_handler_disconnect(message,resultHandler);
    Dispose(&probe);
    ReleaseDispatch();
    if (!WaitCompleted()) failed = 1;
    g_clear_object(&folder); g_clear_object(&message); g_clear_object(&probe.accept);
    g_clear_object(&probe.form);
    if (probe.parent != NULL) { gtk_window_destroy(probe.parent); g_object_unref(probe.parent); }
    g_free(destination);
    return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
