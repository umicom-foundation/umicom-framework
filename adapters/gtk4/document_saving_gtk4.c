/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/document_saving_gtk4.c
 *
 * PURPOSE:
 *   Drive the shared Save All sequence from GTK, collect untitled filenames
 *   asynchronously, and invalidate queued work before its host is destroyed.
 *
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "gtk4_internal.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <stdio.h>

/* The adapter owns one reference while attached. Each queued idle or chooser
 * owns another. The editing token is retained, but the adapter and documents
 * are borrowed and may only be used while that token still points to them. */
typedef struct SaveRun {
    unsigned references;
    GObject *lifetime;
    GCancellable *cancel;
    GtkFileDialog *dialog; /* Owns the copied initial folder for this sequence. */
    UmiDocumentSaveSession *session;
    UmiGtk4DocumentSaveResultFn completed;
    void *context;
    guint idle;
    int choosing;
} SaveRun;

static SaveRun *SaveRunRef(SaveRun *run) { ++run->references; return run; }
static void SaveRunUnref(gpointer data)
{
    SaveRun *run = data;
    if (--run->references != 0U) return;
    UmiDocumentSaveSessionDestroy(run->session);
    g_clear_object(&run->cancel);
    g_clear_object(&run->dialog);
    g_clear_object(&run->lifetime);
    g_free(run);
}
static UmiGtk4Adapter *SaveOwner(SaveRun *run)
{
    UmiGtk4Adapter *adapter = g_object_get_data(run->lifetime, "adapter");
    return adapter != NULL && adapter->document_save_run == run ? adapter : NULL;
}
static gboolean SavePump(gpointer data);
static void SaveSchedule(SaveRun *run)
{
    if (run->idle == 0U && !run->choosing && SaveOwner(run) != NULL)
        run->idle = g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, SavePump,
            SaveRunRef(run), SaveRunUnref);
}

/* Callers hold an idle/chooser reference. Release the adapter-owned reference
 * after any notification; the notification may destroy or rebind its host. */
static void SaveFinish(SaveRun *run, UmiStatus hostStatus)
{
    UmiGtk4Adapter *adapter = SaveOwner(run);
    if (adapter == NULL) return;
    UmiDocumentSaveProgress progress;
    if (UmiDocumentSaveSessionProgress(run->session, &progress) != UMI_STATUS_OK) return;
    if (hostStatus != UMI_STATUS_OK) {
        progress.last_status = hostStatus;
        progress.phase = hostStatus == UMI_STATUS_CANCELLED
            ? UMI_DOCUMENT_SAVE_CANCELLED : UMI_DOCUMENT_SAVE_FAILED;
    }
    adapter->document_save_run = NULL;
    if (adapter->shell != NULL && adapter->window != NULL) {
        /* A rendering failure is not a failed disk write: retain saved counts. */
        UmiStatus refresh = umi_gtk4_refresh_documents(adapter,
            umi_ui_application_shell_workbench(adapter->shell));
        if (progress.last_status == UMI_STATUS_OK && refresh != UMI_STATUS_OK) {
            progress.last_status = refresh;
            progress.phase = UMI_DOCUMENT_SAVE_FAILED;
        }
    }
    /* A detached token must never deliver to its old application context. */
    if (g_object_get_data(run->lifetime, "adapter") != NULL && run->completed != NULL)
        run->completed(run->context, &progress);
    SaveRunUnref(run);
}

static void SavePathChosen(GObject *source, GAsyncResult *result, gpointer data)
{
    SaveRun *run = data;
    GError *error = NULL;
    GFile *file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, &error);
    char *path = file != NULL ? g_file_get_path(file) : NULL;
    run->choosing = 0;
    UmiGtk4Adapter *adapter = SaveOwner(run);
    if (adapter != NULL) {
        if (g_cancellable_is_cancelled(run->cancel)) {
            (void)UmiDocumentSaveSessionCancel(run->session);
            SaveFinish(run, UMI_STATUS_CANCELLED);
        } else if (path != NULL) {
            UmiStatus status = UmiDocumentSaveSessionProvidePath(run->session, path);
            if (status != UMI_STATUS_OK) SaveFinish(run, status);
            else SaveSchedule(run);
        } else {
            int cancelled = g_cancellable_is_cancelled(run->cancel) ||
                (error != NULL && (g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) ||
                    g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) ||
                    g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED)));
            (void)UmiDocumentSaveSessionCancel(run->session);
            SaveFinish(run, cancelled ? UMI_STATUS_CANCELLED : UMI_STATUS_UNAVAILABLE);
        }
    }
    g_free(path);
    g_clear_object(&file);
    g_clear_error(&error);
    SaveRunUnref(run);
}

/* The explicit-location prompt now rechecks its borrowed adapter after observable GTK property updates. Preserve the earlier pump for ownership review. */
#if 0
static gboolean SavePump(gpointer data)
{
    SaveRun *run = data;
    run->idle = 0U;
    UmiGtk4Adapter *adapter = SaveOwner(run);
    if (adapter == NULL) return G_SOURCE_REMOVE;
    (void)UmiDocumentSaveSessionStep(run->session);
    /* A provider must preserve its document owner during a synchronous save.
     * A native rebind still invalidates subsequent UI work and completion. */
    adapter = SaveOwner(run);
    if (adapter == NULL) return G_SOURCE_REMOVE;
    UmiDocumentSaveProgress progress;
    if (UmiDocumentSaveSessionProgress(run->session, &progress) != UMI_STATUS_OK)
        return G_SOURCE_REMOVE;
    if (progress.phase == UMI_DOCUMENT_SAVE_READY) SaveSchedule(run);
    else if (progress.phase == UMI_DOCUMENT_SAVE_NEEDS_PATH) {
/* The per-file chooser previously lacked application location context. Reuse the sequence-owned dialog whose explicit starting folder was copied during admission; retain the previous prompt for comparison. */
#if 0
        GtkFileDialog *dialog = gtk_file_dialog_new();
        char title[UMI_DOCUMENT_NAME_CAPACITY + 80U];
        (void)snprintf(title, sizeof(title), "Save All — choose a filename for %s", progress.display_name);
        gtk_file_dialog_set_title(dialog, title);
        gtk_file_dialog_set_initial_name(dialog, progress.display_name);
        gtk_file_dialog_set_modal(dialog, TRUE);
        run->choosing = 1;
        gtk_file_dialog_save(dialog, adapter->window, run->cancel,
            SavePathChosen, SaveRunRef(run));
        g_object_unref(dialog);
#endif

        char title[UMI_DOCUMENT_NAME_CAPACITY + 80U];
        (void)snprintf(title, sizeof(title), "Save All — choose a filename for %s", progress.display_name);
        gtk_file_dialog_set_title(run->dialog, title);
        gtk_file_dialog_set_initial_name(run->dialog, progress.display_name);
        gtk_file_dialog_set_modal(run->dialog, TRUE);
        run->choosing = 1;
        gtk_file_dialog_save(run->dialog, adapter->window, run->cancel,
            SavePathChosen, SaveRunRef(run));
    } else SaveFinish(run, UMI_STATUS_OK);
    return G_SOURCE_REMOVE;
}

#endif


static gboolean SavePump(gpointer data)
{
    SaveRun *run = data;
    run->idle = 0U;
    UmiGtk4Adapter *adapter = SaveOwner(run);
    if (adapter == NULL) return G_SOURCE_REMOVE;
    (void)UmiDocumentSaveSessionStep(run->session);
    /* A provider must preserve its document owner during a synchronous save.
     * A native rebind still invalidates subsequent UI work and completion. */
    adapter = SaveOwner(run);
    if (adapter == NULL) return G_SOURCE_REMOVE;
    UmiDocumentSaveProgress progress;
    if (UmiDocumentSaveSessionProgress(run->session, &progress) != UMI_STATUS_OK)
        return G_SOURCE_REMOVE;
    if (progress.phase == UMI_DOCUMENT_SAVE_READY) SaveSchedule(run);
    else if (progress.phase == UMI_DOCUMENT_SAVE_NEEDS_PATH) {
/* The per-file chooser previously lacked application location context. Reuse the sequence-owned dialog whose explicit starting folder was copied during admission; retain the previous prompt for comparison. */
#if 0
        GtkFileDialog *dialog = gtk_file_dialog_new();
        char title[UMI_DOCUMENT_NAME_CAPACITY + 80U];
        (void)snprintf(title, sizeof(title), "Save All — choose a filename for %s", progress.display_name);
        gtk_file_dialog_set_title(dialog, title);
        gtk_file_dialog_set_initial_name(dialog, progress.display_name);
        gtk_file_dialog_set_modal(dialog, TRUE);
        run->choosing = 1;
        gtk_file_dialog_save(dialog, adapter->window, run->cancel,
            SavePathChosen, SaveRunRef(run));
        g_object_unref(dialog);
#endif

        char title[UMI_DOCUMENT_NAME_CAPACITY + 80U];
        (void)snprintf(title, sizeof(title), "Save All — choose a filename for %s", progress.display_name);
        gtk_file_dialog_set_title(run->dialog, title);
        gtk_file_dialog_set_initial_name(run->dialog, progress.display_name);
        gtk_file_dialog_set_modal(run->dialog, TRUE);
        /* Property observers may detach the editing binding. The idle retains
         * the run, but its borrowed adapter must be resolved again here. */
        adapter = SaveOwner(run);
        if (adapter == NULL) return G_SOURCE_REMOVE;
        run->choosing = 1;
        gtk_file_dialog_save(run->dialog, adapter->window, run->cancel,
            SavePathChosen, SaveRunRef(run));
    } else SaveFinish(run, UMI_STATUS_OK);
    return G_SOURCE_REMOVE;
}

/* Explicit chooser location is now an optional shared service input. The compatibility wrapper below keeps the original interface; retain the previous admission implementation for ownership review. */
#if 0
UmiStatus UmiGtk4AdapterDocumentSaveAll(UmiGtk4Adapter *adapter,
    UmiGtk4DocumentSaveResultFn completed, void *context)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
        adapter->window == NULL || adapter->shell == NULL) return UMI_STATUS_UNAVAILABLE;
    /* Former guard retained below. An unanswered close also owns a document
     * decision, so reject Save All until that question/sequence has ended. */
    // if (adapter->document_save_run != NULL || adapter->edit_cancel != NULL) return UMI_STATUS_BUSY;
    if (adapter->document_save_run != NULL || adapter->edit_cancel != NULL ||
        UmiGtk4AdapterDocumentCloseBusy(adapter)) return UMI_STATUS_BUSY;
    /* Keep saving separate from a currently reviewed draft replacement. */
    if (UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_BUSY;
    SaveRun *run = g_try_new0(SaveRun, 1);
    if (run == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    run->references = 1U;
    UmiStatus status = UmiDocumentSaveSessionCreate(adapter->edit_coordinator, &run->session);
    if (status != UMI_STATUS_OK) { g_free(run); return status; }
    run->lifetime = g_object_ref(adapter->edit_lifetime);
    run->cancel = g_cancellable_new();
    run->completed = completed;
    run->context = context;
    adapter->document_save_run = run;
    SaveSchedule(run);
    return UMI_STATUS_OK;
}

#endif


/* Capture location before queuing any writes. The chooser owns the copied
 * GFile, so later caller edits, workspace switches or cwd changes cannot retarget
 * untitled prompts. Existing named documents retain their original paths. */
/* Capture the editing lifetime before constructing GTK chooser state. The replacement rejects binding retirement during synchronous property observation; retain this draft for review. */
#if 0
UmiStatus UmiGtk4AdapterDocumentSaveAllAt(UmiGtk4Adapter *adapter,
    const char *initialFolder, UmiGtk4DocumentSaveResultFn completed, void *context)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
        adapter->window == NULL || adapter->shell == NULL) return UMI_STATUS_UNAVAILABLE;
    if (adapter->document_save_run != NULL || adapter->edit_cancel != NULL ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_BUSY;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    if (dialog == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    UmiStatus status = UmiPathPickerGtk4SetInitialLocation(dialog,
        UMI_PATH_PICKER_NEW_FILE, NULL, initialFolder, NULL);
    if (status != UMI_STATUS_OK) { g_object_unref(dialog); return status; }
    SaveRun *run = g_try_new0(SaveRun, 1);
    if (run == NULL) { g_object_unref(dialog); return UMI_STATUS_OUT_OF_MEMORY; }
    run->references = 1U;
    run->dialog = dialog;
    status = UmiDocumentSaveSessionCreate(adapter->edit_coordinator, &run->session);
    if (status != UMI_STATUS_OK) { SaveRunUnref(run); return status; }
    run->lifetime = g_object_ref(adapter->edit_lifetime);
    run->cancel = g_cancellable_new();
    run->completed = completed;
    run->context = context;
    adapter->document_save_run = run;
    SaveSchedule(run);
    return UMI_STATUS_OK;
}

#endif


UmiStatus UmiGtk4AdapterDocumentSaveAllAt(UmiGtk4Adapter *adapter,
    const char *initialFolder, UmiGtk4DocumentSaveResultFn completed, void *context)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (adapter->edit_coordinator == NULL || adapter->edit_lifetime == NULL ||
        adapter->window == NULL || adapter->shell == NULL) return UMI_STATUS_UNAVAILABLE;
    if (adapter->document_save_run != NULL || adapter->edit_cancel != NULL ||
        UmiGtk4AdapterDocumentCloseBusy(adapter) ||
        UmiGtk4AdapterReplacementReviewBusy(adapter)) return UMI_STATUS_BUSY;
    GObject *lifetime = g_object_ref(adapter->edit_lifetime);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    if (dialog == NULL) { g_object_unref(lifetime); return UMI_STATUS_OUT_OF_MEMORY; }
    UmiStatus status = UmiPathPickerGtk4SetInitialLocation(dialog,
        UMI_PATH_PICKER_NEW_FILE, NULL, initialFolder, NULL);
    if (status == UMI_STATUS_OK && g_object_get_data(lifetime, "adapter") != adapter)
        status = UMI_STATUS_CANCELLED;
    if (status != UMI_STATUS_OK) {
        g_object_unref(dialog);
        g_object_unref(lifetime);
        return status;
    }
    SaveRun *run = g_try_new0(SaveRun, 1);
    if (run == NULL) {
        g_object_unref(dialog);
        g_object_unref(lifetime);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    run->references = 1U;
    run->dialog = dialog;
    run->lifetime = lifetime;
    status = UmiDocumentSaveSessionCreate(adapter->edit_coordinator, &run->session);
    if (status != UMI_STATUS_OK) { SaveRunUnref(run); return status; }
    run->cancel = g_cancellable_new();
    run->completed = completed;
    run->context = context;
    adapter->document_save_run = run;
    SaveSchedule(run);
    return UMI_STATUS_OK;
}

/* Compatibility: callers with no explicit location retain the operating-system
 * chooser default. No current-directory or settings-directory guess is made. */
UmiStatus UmiGtk4AdapterDocumentSaveAll(UmiGtk4Adapter *adapter,
    UmiGtk4DocumentSaveResultFn completed, void *context)
{
    return UmiGtk4AdapterDocumentSaveAllAt(adapter, NULL, completed, context);
}
/* Observing a save must not schedule another step or retain a borrowed result. */
UmiStatus UmiGtk4AdapterDocumentSaveAllProgress(const UmiGtk4Adapter *adapter,
    UmiDocumentSaveProgress *outProgress)
{
    if (adapter == NULL || outProgress == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* This public accessor is implemented in the GTK library, not in Studio.
     * Former direct-copy statements are retained below. Validate ownership and
     * the copied snapshot before publishing it into the caller's storage. */
    // const SaveRun *run = adapter->document_save_run;
    // if (run == NULL) return UMI_STATUS_NOT_FOUND;
    // return UmiDocumentSaveSessionProgress(run->session, outProgress);
    SaveRun *run = adapter->document_save_run;
    if (run == NULL) return UMI_STATUS_NOT_FOUND;
    if (run->lifetime == NULL || SaveOwner(run) != adapter ||
        adapter->edit_coordinator == NULL)
        return UMI_STATUS_INVALID_STATE;
    UmiDocumentSaveProgress progress;
    UmiStatus status = UmiDocumentSaveSessionProgress(run->session, &progress);
    if (status == UMI_STATUS_OK) status = UmiDocumentSaveProgressValidate(&progress);
    if (status == UMI_STATUS_OK) *outProgress = progress;
    return status;
}

int UmiGtk4AdapterDocumentSaveAllBusy(const UmiGtk4Adapter *adapter)
{ return adapter != NULL && adapter->document_save_run != NULL; }

UmiStatus UmiGtk4AdapterCancelDocumentSaveAll(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    SaveRun *run = adapter->document_save_run;
    if (run == NULL) return UMI_STATUS_OK;
    SaveRunRef(run);
    (void)UmiDocumentSaveSessionCancel(run->session);
    g_cancellable_cancel(run->cancel);
    SaveSchedule(run);
    SaveRunUnref(run);
    return UMI_STATUS_OK;
}

void UmiGtk4DocumentSaveAllDetach(UmiGtk4Adapter *adapter)
{
    if (adapter == NULL) return;
    SaveRun *run = adapter->document_save_run;
    if (run == NULL) return;
    adapter->document_save_run = NULL;
    (void)UmiDocumentSaveSessionCancel(run->session);
    if (run->idle != 0U) {
        guint idle = run->idle; run->idle = 0U;
        (void)g_source_remove(idle);
    }
    g_cancellable_cancel(run->cancel);
    SaveRunUnref(run);
}
