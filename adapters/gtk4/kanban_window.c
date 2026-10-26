/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_window.c
 * PURPOSE: Serialize board-file work away from GTK and preserve edits on refused storage operations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_window.h"
#include "umicom/ui/gtk4/kanban.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/kanban/document.h"
#include "umicom/kanban/binding_storage.h"
#include <string.h>
#include "window_removal_private.h"
#include "focus_handoff_private.h"

typedef enum JobKind { JOB_CREATE, JOB_OPEN, JOB_SAVE, JOB_CLOSE, JOB_SAVE_COPY,
    JOB_BINDING_SAVE, JOB_BINDING_LOAD } JobKind;
typedef struct WindowState {
    GtkWindow *window;
    GtkWidget *body, *controls, *status, *directory, *filename, *discard, *panel;
    UmiKanbanDocument *document;
    bool busy;
    bool closed;
} WindowState;
typedef struct Job {
    JobKind kind;
    UmiKanbanDocument *document;
    char *directory, *filename;
    char *settings_directory;
    UmiKanbanProjectBinding binding;
    uint64_t revision;
    bool discard, close_window;
    UmiStatus status;
    UmiKanbanSaveProblem problem;
} Job;
static void Say(WindowState *state, const char *message)
{ gtk_label_set_text(GTK_LABEL(state->status), message); }
/* Retain the former release order for review. Clear the published panel pointer
 * before notifications so recursive host teardown cannot release it twice. */
#if 0
static void ReleasePanel(WindowState *state)
{
    if (state->panel == NULL) return;
    UmiKanbanGtk4Retire(state->panel);
    if (gtk_widget_get_parent(state->panel) == state->body)
        gtk_box_remove(GTK_BOX(state->body), state->panel);
    g_clear_object(&state->panel);
}
#endif
static void ReleasePanel(WindowState *state)
{
    GtkWidget *panel = state->panel;
    if (panel == NULL) return;
    state->panel = NULL;
    UmiKanbanGtk4Retire(panel);
    if (gtk_widget_get_parent(panel) == state->body)
        gtk_box_remove(GTK_BOX(state->body), panel);
    g_object_unref(panel);
}
static void FreeState(gpointer data)
{
    WindowState *state = data;
    ReleasePanel(state);
    if (state->document != NULL) {
        /* Normal closure uses the explicit asynchronous close flow. A host
         * destroying its application directly cannot retain unsaved memory;
         * release the lease without replacing the last saved archive. */
        g_warning("Kanban host destroyed an open document; the saved archive is retained.");
        (void)umi_kanban_document_close(&state->document, true);
    }
    g_object_unref(state->body); g_object_unref(state->controls); g_object_unref(state->status);
    g_free(state);
}
static void FreeJob(gpointer data)
{
    Job *job = data;
    /* Completion normally returns ownership to the window. This fallback
     * releases a worker-owned lease if application teardown interrupts it. */
    if (job->document != NULL) (void)umi_kanban_document_close(&job->document, true);
    g_free(job->settings_directory);
    g_free(job->directory); g_free(job->filename); g_free(job);
}
static void Work(GTask *task, gpointer source, gpointer task_data, GCancellable *cancel)
{
    (void)source; (void)cancel; Job *job = task_data;
    if (job->kind == JOB_BINDING_SAVE) {
        job->status = UmiKanbanBindingStorageSave(job->settings_directory, &job->binding);
    } else if (job->kind == JOB_BINDING_LOAD) {
        job->status = UmiKanbanBindingStorageLoad(job->settings_directory,
            job->binding.project_root, &job->binding);
    } else if (job->kind == JOB_CREATE) {
        UmiKanbanBoard *template = NULL; UmiKanbanConfig config;
        job->status = umi_kanban_board_create_workflow("development", "Development board", &template);
        if (job->status == UMI_STATUS_OK) job->status = umi_kanban_board_config(template, &config);
        if (job->status == UMI_STATUS_OK)
            job->status = umi_kanban_document_create(job->directory, job->filename, &config, &job->document);
        umi_kanban_board_destroy(template);
    } else if (job->kind == JOB_OPEN) {
        job->status = umi_kanban_document_open(job->directory, job->filename, &job->document);
    } else if (job->kind == JOB_SAVE) {
        job->status = umi_kanban_document_save(job->document, job->revision, &job->problem);
    } else if (job->kind == JOB_SAVE_COPY) {
        job->status = umi_kanban_document_save_copy(job->document, job->revision, job->directory, job->filename);
    } else {
        job->status = umi_kanban_document_close(&job->document, job->discard);
    }
    g_task_return_boolean(task, TRUE);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer data);
/* Keep the original source-object retention for engineering review. GTask
 * can finalize its source object on the worker, so the replacement retains the
 * native window in callback data and explicitly releases it on the owner. */
#if 0
static void Dispatch(WindowState *state, Job *job)
{
    GTask *task = g_task_new(state->window, NULL, Complete, NULL);
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Work);
    g_object_unref(task);
}
#endif
static void CompleteOnOwner(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source;
    GtkWindow *window = data;
    Complete(G_OBJECT(window), result, NULL);
    /* Complete can schedule a second document-lease cleanup job. That job
     * takes its own reference before this callback releases the old one. */
    g_object_unref(window);
}
static void Dispatch(WindowState *state, Job *job)
{
    GTask *task = g_task_new(NULL, NULL, CompleteOnOwner, g_object_ref(state->window));
    g_task_set_task_data(task, job, FreeJob);
    g_task_run_in_thread(task, Work);
    g_object_unref(task);
}
/* A removed but retained native window is no longer an editor. Finish any
 * accepted file operation, then release its lease on the same worker path.
 * Forced host destruction discards memory only; it never saves over the file. */
static void CloseRetiredDocument(WindowState *state)
{
    ReleasePanel(state);
    if (state->document == NULL) { state->busy = false; return; }
    Job *cleanup = g_new0(Job, 1);
    cleanup->kind = JOB_CLOSE;
    cleanup->discard = true;
    cleanup->document = state->document;
    state->document = NULL;
    state->busy = true;
    Dispatch(state, cleanup);
}
static void WindowRemoved(GtkWidget *window, gpointer data)
{
    (void)window;
    WindowState *state = data;
    if (state->closed) return;
    state->closed = true;
    ReleasePanel(state);
    if (!state->busy) CloseRetiredDocument(state);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)data; GTask *task = G_TASK(result); Job *job = g_task_get_task_data(task);
    WindowState *state = g_object_get_data(source, "umicom-kanban-window");
    (void)g_task_propagate_boolean(task, NULL);
    state->document = job->document; job->document = NULL;
    if (state->closed) goto retired;
    /* A location is only a prefilled choice. Do not overwrite edits made while
     * its worker was reading, and never open the recalled archive implicitly. */
    if (job->kind == JOB_BINDING_LOAD && job->status == UMI_STATUS_OK) {
        if (strcmp(gtk_editable_get_text(GTK_EDITABLE(state->directory)), job->directory) != 0 ||
            strcmp(gtk_editable_get_text(GTK_EDITABLE(state->filename)), job->filename) != 0)
            job->status = UMI_STATUS_INVALID_STATE;
        else if (!g_utf8_validate(job->binding.directory, -1, NULL) ||
                 !g_utf8_validate(job->binding.filename, -1, NULL))
            job->status = UMI_STATUS_PARSE_ERROR;
        else {
            gtk_editable_set_text(GTK_EDITABLE(state->directory), job->binding.directory);
            if (state->closed) goto retired;
            /* A notification observer may edit the other field. Preserve it
             * instead of treating this partial native update as a transaction. */
            if (strcmp(gtk_editable_get_text(GTK_EDITABLE(state->filename)), job->filename) != 0)
                job->status = UMI_STATUS_INVALID_STATE;
            else gtk_editable_set_text(GTK_EDITABLE(state->filename), job->binding.filename);
            if (state->closed) goto retired;
            if (strcmp(gtk_editable_get_text(GTK_EDITABLE(state->directory)), job->binding.directory) != 0 ||
                strcmp(gtk_editable_get_text(GTK_EDITABLE(state->filename)), job->binding.filename) != 0)
                job->status = UMI_STATUS_INVALID_STATE;
        }
    }
    if (state->document != NULL && state->panel == NULL) {
        state->panel = UmiKanbanGtk4Create(umi_kanban_document_board(state->document));
        if (state->panel != NULL) {
            g_object_ref_sink(state->panel);
            if (state->closed) goto retired;
            gtk_widget_set_vexpand(state->panel, TRUE);
            if (state->closed) goto retired;
            gtk_box_append(GTK_BOX(state->body), state->panel);
            if (state->closed) goto retired;
        }
    }
    /* Keep the operation guard through every observable GTK update. A native
     * property observer must not start another file job during completion. */
    if (state->panel != NULL) {
        UmiKanbanGtk4SetBusy(state->panel, false);
        if (state->closed) goto retired;
        (void)UmiKanbanGtk4Refresh(state->panel);
        if (state->closed) goto retired;
    }
    gtk_widget_set_sensitive(state->controls, TRUE);
    if (state->closed) goto retired;
    /* Discard consent applies to one close attempt, never a later document. */
    gtk_check_button_set_active(GTK_CHECK_BUTTON(state->discard), FALSE);
    if (state->closed) goto retired;
    if (job->status == UMI_STATUS_OK) {
        Say(state, job->kind == JOB_BINDING_SAVE ? "Private project location remembered. No board archive was saved or published." :
            job->kind == JOB_BINDING_LOAD ? "Saved location recalled. Review the fields, then choose Open board." :
            job->kind == JOB_SAVE_COPY ? "Copy saved to the chosen new file. The current board keeps its original file and unsaved state; close it explicitly before opening the copy." :
            job->kind == JOB_SAVE ? "Board saved. Public website snapshots must be regenerated after changes." :
            job->kind == JOB_CLOSE ? "Board closed." : "Board opened. Changes remain local until you save them.");
    } else {
        char message[256];
        g_snprintf(message, sizeof(message), "Operation refused: %s (save detail %d). Existing saved files are retained. Review the path or keep the open document to recover edits.",
            umi_status_text(job->status), (int)job->problem);
        Say(state, message);
    }
    if (state->closed) goto retired;
    if (state->document != NULL && state->panel == NULL)
        Say(state, "The document opened, but its editor could not be created. Close the board before trying again.");
    if (state->closed) goto retired;
    state->busy = false;
    if (job->close_window && state->document == NULL && job->status == UMI_STATUS_OK)
        gtk_window_destroy(state->window);
    return;
retired:
    CloseRetiredDocument(state);
}
static void Start(WindowState *state, JobKind kind, bool close_window)
{
    /* Reentrant observers must not receive another status notification here. */
    if (state->busy || state->closed) return;
    if ((kind == JOB_BINDING_SAVE || kind == JOB_BINDING_LOAD) && state->document != NULL) {
        Say(state, "Save and close the board before changing its remembered location."); return;
    }
    if ((kind == JOB_CREATE || kind == JOB_OPEN) && state->document != NULL) {
        Say(state, "Save and close the current board before opening another."); return;
    }
    if ((kind == JOB_SAVE || kind == JOB_CLOSE || kind == JOB_SAVE_COPY) && state->document == NULL) {
        Say(state, "Open or create a board first."); return;
    }
    Job *job = g_new0(Job, 1); job->kind = kind; job->close_window = close_window;
    job->directory = g_strdup(gtk_editable_get_text(GTK_EDITABLE(state->directory)));
    job->filename = g_strdup(gtk_editable_get_text(GTK_EDITABLE(state->filename)));
    job->discard = gtk_check_button_get_active(GTK_CHECK_BUTTON(state->discard));
    if (kind == JOB_BINDING_SAVE || kind == JOB_BINDING_LOAD) {
        const UmiKanbanProjectLocation *project =
            g_object_get_data(G_OBJECT(state->window), "umicom-kanban-project");
        const char *settings = g_object_get_data(G_OBJECT(state->window), "umicom-kanban-settings");
        if (project == NULL || settings == NULL) {
            FreeJob(job); Say(state, "This window has no private project settings location."); return;
        }
        job->settings_directory = g_strdup(settings);
        UmiStatus status = UMI_STATUS_OK;
        if (kind == JOB_BINDING_SAVE)
            status = UmiKanbanProjectBindingCreate(project->project_root, job->directory, job->filename, &job->binding);
        else memcpy(job->binding.project_root, project->project_root, strlen(project->project_root)+1U);
        if (status != UMI_STATUS_OK) {
            FreeJob(job); Say(state, "Choose a valid absolute folder and board filename before remembering it."); return;
        }
    }
    if (state->document != NULL) {
        UmiKanbanDocumentState snapshot;
        if (umi_kanban_document_state(state->document, &snapshot) != UMI_STATUS_OK) {
            FreeJob(job); Say(state, "Cannot read the open document state."); return;
        }
        job->revision = snapshot.board_revision;
        if (kind == JOB_CLOSE && snapshot.unsaved_changes && !job->discard) {
            FreeJob(job); Say(state, "The board has unsaved changes. Save it, or explicitly enable discard before closing."); return;
        }
    }
    /* Retain the earlier dispatch sequence for ownership review. The new
     * sequence also handles native removal during preparation notifications. */
#if 0
    /* Set the guard before GTK notifications. Transfer document ownership only
     * after freezing its borrowed panel; no GTK callback may race the worker. */
    state->busy = true;
    if (state->panel != NULL) UmiKanbanGtk4SetBusy(state->panel, true);
    if (kind == JOB_CLOSE) ReleasePanel(state);
    gtk_widget_set_sensitive(state->controls, FALSE);
    job->document = state->document; state->document = NULL;
    Say(state, "Working with the board file…");
    GTask *task = g_task_new(state->window, NULL, Complete, NULL);
    g_task_set_task_data(task, job, FreeJob); g_task_run_in_thread(task, Work); g_object_unref(task);

#endif
    state->busy = true;
    job->document = state->document;
    state->document = NULL;
    if (state->panel != NULL) UmiKanbanGtk4SetBusy(state->panel, true);
    if (state->closed) goto abandoned;
    if (kind == JOB_CLOSE) ReleasePanel(state);
    if (state->closed) goto abandoned;
    UmiGtk4ReleaseSubtreeFocus(state->controls);
    if (state->closed) goto abandoned;
    gtk_widget_set_sensitive(state->controls, FALSE);
    if (state->closed) goto abandoned;
    Say(state, "Working with the board file…");
    if (state->closed) goto abandoned;
    Dispatch(state, job);
    return;
abandoned:
    state->document = job->document;
    job->document = NULL;
    FreeJob(job);
    CloseRetiredDocument(state);
}
static void Click(GtkButton *button, gpointer data)
{
    GtkWindow *window = data; g_object_ref(window);
    WindowState *state = g_object_get_data(G_OBJECT(window), "umicom-kanban-window");
    JobKind kind = (JobKind)GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "kanban-job"));
    if (state != NULL && gtk_window_get_application(window) != NULL) Start(state, kind, false);
    g_object_unref(window);
}
static gboolean CloseRequest(GtkWindow *window, gpointer data)
{
    (void)window; WindowState *state = data;
    if (state->busy) return TRUE;
    if (state->document == NULL) return FALSE;
    Start(state, JOB_CLOSE, true); return TRUE;
}
static GtkWidget *Field(GtkWidget *box, const char *text, int limit)
{
    GtkWidget *label = gtk_label_new(text); gtk_label_set_xalign(GTK_LABEL(label), 0.0F); gtk_box_append(GTK_BOX(box), label);
    GtkWidget *entry = gtk_entry_new(); gtk_entry_set_max_length(GTK_ENTRY(entry), limit); gtk_box_append(GTK_BOX(box), entry); return entry;
}
GtkWindow *UmiKanbanGtk4CreateWindow(GtkApplication *application)
{
    if (!GTK_IS_APPLICATION(application)) return NULL;
    WindowState *state = g_new0(WindowState, 1);
    state->window = GTK_WINDOW(gtk_application_window_new(application));
    gtk_widget_set_name(GTK_WIDGET(state->window), "umicom-kanban-window");
    gtk_window_set_title(state->window, "Umicom Kanban"); gtk_window_set_default_size(state->window, 1320, 880);
    state->body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12); g_object_ref_sink(state->body);
    gtk_widget_set_margin_start(state->body, 16); gtk_widget_set_margin_end(state->body, 16);
    gtk_widget_set_margin_top(state->body, 16); gtk_widget_set_margin_bottom(state->body, 16);
    GtkWidget *scroll = gtk_scrolled_window_new(); gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), state->body);
    gtk_window_set_child(state->window, scroll);
    state->controls = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); g_object_ref_sink(state->controls);
    gtk_widget_set_name(state->controls, "kanban-document-controls");
    gtk_box_append(GTK_BOX(state->body), state->controls);
    state->directory = Field(state->controls, "Existing absolute folder for private board files", (int)UMI_PATH_CAPACITY - 1);
    gtk_box_append(GTK_BOX(state->controls), UmiPathPickerGtk4Create(GTK_ENTRY(state->directory), UMI_PATH_PICKER_DIRECTORY, "Choose board folder", NULL));
    gtk_widget_set_name(state->directory, "kanban-document-directory");
    state->filename = Field(state->controls, "Filename for Open/Create/Save copy (.umikb). Save board keeps the opened filename.", (int)UMI_KANBAN_FILENAME_CAPACITY - 1);
    gtk_widget_set_name(state->filename, "kanban-document-filename");
    gtk_editable_set_text(GTK_EDITABLE(state->filename), "development.umikb");
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8); gtk_box_append(GTK_BOX(state->controls), buttons);
    const char *labels[] = {"Create board", "Open board", "Save board", "Close board", "Save copy to new file"};
    for (int i = 0; i < 5; ++i) {
        GtkWidget *button = gtk_button_new_with_label(labels[i]);
        g_object_set_data(G_OBJECT(button), "kanban-job", GINT_TO_POINTER(i));
        /* Retained controls disconnect when their window owner is finalized. */
        g_signal_connect_object(button, "clicked", G_CALLBACK(Click), state->window, 0);
        gtk_box_append(GTK_BOX(buttons), button);
    }
    state->discard = gtk_check_button_new_with_label("Discard unsaved board edits on this close attempt");
    gtk_widget_set_name(state->discard, "kanban-document-discard");
    gtk_box_append(GTK_BOX(state->controls), state->discard);
    state->status = gtk_label_new("Choose a local folder. Create refuses an existing file; Open validates a saved board. No website is uploaded automatically.");
    gtk_widget_set_name(state->status, "kanban-document-status");
    g_object_ref_sink(state->status); gtk_label_set_xalign(GTK_LABEL(state->status), 0.0F); gtk_label_set_wrap(GTK_LABEL(state->status), TRUE);
    gtk_box_append(GTK_BOX(state->body), state->status);
    g_object_set_data_full(G_OBJECT(state->window), "umicom-kanban-window", state, FreeState);
    g_signal_connect(state->window, "close-request", G_CALLBACK(CloseRequest), state);
    UmiGtk4ObserveWindowRemoval(state->window, G_OBJECT(state->window), WindowRemoved, state);
    return state->window;
}

static void Launch(GtkButton *button, gpointer data)
{
    GtkWindow *owner = data; g_object_ref(owner);
    if (gtk_widget_get_root(GTK_WIDGET(button)) == GTK_ROOT(owner)) {
        GtkApplication *application = gtk_window_get_application(owner);
        if (application != NULL) {
            GtkWindow *board = UmiKanbanGtk4CreateWindow(application);
            if (board != NULL) {
                /* Native construction can notify observers. Do not open a new
                 * window on behalf of a launcher detached during construction. */
                if (gtk_widget_get_root(GTK_WIDGET(button)) == GTK_ROOT(owner)) gtk_window_present(board);
                else gtk_window_destroy(board);
            }
        }
    }
    g_object_unref(owner);
}
GtkWidget *UmiKanbanGtk4CreateLauncher(GtkWindow *owner)
{
    if (!GTK_IS_WINDOW(owner)) return NULL;
    GtkWidget *button = gtk_button_new_with_label("Open Kanban board");
    gtk_widget_set_name(button, "umicom-kanban-launcher");
    gtk_widget_set_tooltip_text(button, "Open a local planning board and prepare a reviewed contributor website");
    g_signal_connect_object(button, "clicked", G_CALLBACK(Launch), owner, 0);
    return button;
}

/* Poll only from the GTK owner thread. Cleanup is included so an embedding
 * host can wait for lease retirement without inspecting widgets after close. */
bool UmiKanbanGtk4WindowBusy(GtkWindow *window)
{
    if (!GTK_IS_WINDOW(window)) return false;
    WindowState *state = g_object_get_data(G_OBJECT(window), "umicom-kanban-window");
    return state != NULL && state->busy;
}

/* Project windows share the existing serialized document and close ownership. */
#include "kanban_project.inc"

#include "kanban_binding.inc"
