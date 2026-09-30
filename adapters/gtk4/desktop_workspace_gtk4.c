/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/desktop_workspace_gtk4.c
 * PURPOSE:
 *   A draft editor over the shared desktop-workspace service. The window never reaches into
 *   SQLite and it never owns settings for the rest of the desktop. Storage work completes on
 *   its sole task before explicit close is finalised.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
 * Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A draft editor over the shared desktop-workspace service. The window never
 * reaches into SQLite and it never owns settings for the rest of the desktop.
 * Storage work completes on its sole task before explicit close is finalised.
 *---------------------------------------------------------------------------*/
#include "umicom/desktop_workspace/gtk4.h"
#include "umicom/desktop_workspace/workspace.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib/gstdio.h>
#include "desktop_system_brand.inc"

typedef struct WorkspaceUi {
    GtkWindow *window;
    GtkWidget *root, *open, *editor, *sidebar, *title, *body, *notes, *theme,
        *font, *visible, *save, *discard, *add, *remove, *checkpoint, *preview,
        *status, *revision;
    GtkStringList *titles;
    GtkCssProvider *css;
    GdkDisplay *display;
    char *directory, *cssClass;
    UmiDesktopWorkspace *workspace;
    UmiDesktopWorkspaceSnapshot *draft;
    int busy, dirty, loading, closing, allowClose;
    GtkWindow *question;
} WorkspaceUi;
typedef struct WorkspaceJob {
    WorkspaceUi *ui;
    GtkApplication *application;
    int operation;
    uint64_t checkpoint;
    UmiStatus status;
    char message[256];
    UmiDesktopWorkspaceSnapshot *snapshot;
} WorkspaceJob;
static void Begin(WorkspaceUi *ui, int operation, uint64_t checkpoint);
static void Status(WorkspaceUi *ui, const char *message)
{ gtk_label_set_text(GTK_LABEL(ui->status), message); }
static void Buttons(WorkspaceUi *ui)
{
    int idle = !ui->busy;
    gtk_widget_set_sensitive(ui->open, idle && !ui->workspace);
    gtk_widget_set_sensitive(ui->editor, idle && ui->workspace != NULL);
    gtk_widget_set_sensitive(ui->save, idle && ui->workspace != NULL && ui->dirty);
    gtk_widget_set_sensitive(ui->discard, idle && ui->workspace != NULL && ui->dirty);
    gtk_widget_set_sensitive(ui->preview, idle && ui->workspace != NULL && !ui->dirty);
    g_object_set_data(G_OBJECT(ui->window), "umicom.desktop-workspace.busy", GINT_TO_POINTER(ui->busy));
}
static void Changed(gpointer widget, gpointer data)
{
    (void)widget; WorkspaceUi *ui = data;
    if (ui->loading || ui->busy || !ui->workspace) return;
    ui->dirty = 1; Status(ui, "Unsaved draft. Save checkpoint records notes and preferences together."); Buttons(ui);
}
static void NotifyChanged(GObject *object, GParamSpec *spec, gpointer data)
{ (void)spec; Changed(object, data); }
static void Appearance(WorkspaceUi *ui)
{
    unsigned font = ui->draft->fontPoints;
    const char *palette = ui->draft->theme == UMI_DESKTOP_WORKSPACE_DARK ?
        "background-color:#142332;color:#eff5fa;" : ui->draft->theme == UMI_DESKTOP_WORKSPACE_LIGHT ?
        "background-color:#ffffff;color:#172838;" : "";
    char *css = g_strdup_printf(".%s {font-size:%upt;%s} .%s textview text {font-size:%upt;%s}",
        ui->cssClass, font, palette, ui->cssClass, font, palette);
#if GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(ui->css, css);
#else
    gtk_css_provider_load_from_data(ui->css, css, -1);
#endif
    g_free(css);
    gtk_widget_set_visible(ui->sidebar, ui->draft->sidebarVisible != 0);
}
static void ShowNote(WorkspaceUi *ui)
{
    const UmiDesktopWorkspaceNote *note = NULL;
    for (size_t i = 0; i < ui->draft->noteCount; ++i)
        if (!strcmp(ui->draft->selectedNote, ui->draft->notes[i].id)) note = &ui->draft->notes[i];
    ui->loading = 1;
    gtk_editable_set_text(GTK_EDITABLE(ui->title), note ? note->title : "");
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->body)), note ? note->body : "", -1);
    gtk_widget_set_sensitive(ui->title, note != NULL); gtk_widget_set_sensitive(ui->body, note != NULL);
    gtk_widget_set_sensitive(ui->remove, note != NULL);
    ui->loading = 0;
}
static int ReadControls(WorkspaceUi *ui)
{
    if (ui->draft->selectedNote[0]) {
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->body));
        GtkTextIter start, end; gtk_text_buffer_get_bounds(buffer, &start, &end);
        char *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
        char id[UMI_DESKTOP_WORKSPACE_ID]; strcpy(id, ui->draft->selectedNote);
        UmiStatus status = UmiDesktopWorkspacePutNote(ui->draft, id,
            gtk_editable_get_text(GTK_EDITABLE(ui->title)), text);
        g_free(text);
        if (status != UMI_STATUS_OK) {
            Status(ui, "The title must contain 1–95 UTF-8 bytes; note text is limited to 4095 bytes. No checkpoint was changed.");
            return 0;
        }
    }
    ui->draft->theme = (UmiDesktopWorkspaceTheme)gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->theme));
    ui->draft->fontPoints = (unsigned)gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->font));
    ui->draft->sidebarVisible = gtk_check_button_get_active(GTK_CHECK_BUTTON(ui->visible));
    return UmiDesktopWorkspaceValidate(ui->draft) == UMI_STATUS_OK;
}
static void Render(WorkspaceUi *ui)
{
    ui->loading = 1;
    guint count = g_list_model_get_n_items(G_LIST_MODEL(ui->titles));
    gtk_string_list_splice(ui->titles, 0, count, NULL);
    guint selected = GTK_INVALID_LIST_POSITION;
    for (size_t i = 0; i < ui->draft->noteCount; ++i) {
        gtk_string_list_append(ui->titles, ui->draft->notes[i].title);
        if (!strcmp(ui->draft->selectedNote, ui->draft->notes[i].id)) selected = (guint)i;
    }
    gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->notes), selected);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->theme), (guint)ui->draft->theme);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->font), (double)ui->draft->fontPoints);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->visible), ui->draft->sidebarVisible != 0);
    char label[160];
    (void)snprintf(label, sizeof label, "Saved revision: %" PRIu64 " · retained: %" PRIu64 "–%" PRIu64,
        ui->draft->revision, UmiDesktopWorkspaceOldestRevision(ui->workspace), ui->draft->revision);
    gtk_label_set_text(GTK_LABEL(ui->revision), label);
    ui->loading = 0; ShowNote(ui); Appearance(ui); Buttons(ui);
}
static void SelectNote(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)object; (void)spec; WorkspaceUi *ui = data;
    if (ui->loading || ui->busy || !ui->workspace) return;
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(ui->notes));
    if (selected >= ui->draft->noteCount) return;
    if (!ReadControls(ui)) {
        ui->loading = 1;
        for (size_t i = 0; i < ui->draft->noteCount; ++i)
            if (!strcmp(ui->draft->selectedNote, ui->draft->notes[i].id))
                gtk_drop_down_set_selected(GTK_DROP_DOWN(ui->notes), (guint)i);
        ui->loading = 0; return;
    }
    strcpy(ui->draft->selectedNote, ui->draft->notes[selected].id);
    ui->dirty = 1; ShowNote(ui); Buttons(ui);
}
static void AddNote(GtkButton *button, gpointer data)
{
    (void)button; WorkspaceUi *ui = data;
    if (ui->busy || !ui->workspace || !ReadControls(ui)) return;
    char *id = g_uuid_string_random();
    UmiStatus status = UmiDesktopWorkspacePutNote(ui->draft, id, "New note", ""); g_free(id);
    if (status != UMI_STATUS_OK) { Status(ui, "This workspace already contains 16 notes."); return; }
    ui->dirty = 1; Render(ui); Status(ui, "New note in draft. Give it a title, then save a checkpoint.");
}
static void RemoveNote(GtkButton *button, gpointer data)
{
    (void)button; WorkspaceUi *ui = data;
    if (ui->busy || !ui->workspace || !ui->draft->selectedNote[0]) return;
    char id[UMI_DESKTOP_WORKSPACE_ID]; strcpy(id, ui->draft->selectedNote);
    if (UmiDesktopWorkspaceRemoveNote(ui->draft, id) != UMI_STATUS_OK) return;
    ui->dirty = 1; Render(ui); Status(ui, "Note removed from the draft. Saved checkpoints are unchanged until Save.");
}
static void Discard(GtkButton *button, gpointer data)
{
    (void)button; WorkspaceUi *ui = data;
    if (ui->busy || !ui->workspace) return;
    if (UmiDesktopWorkspaceRead(ui->workspace, ui->draft) != UMI_STATUS_OK) return;
    ui->dirty = 0; Render(ui); Status(ui, "Draft discarded. The last committed checkpoint is displayed.");
}
static void Run(GTask *task, gpointer source, gpointer taskData, GCancellable *cancel)
{
    (void)source; (void)cancel; WorkspaceJob *job = taskData; WorkspaceUi *ui = job->ui;
    if (job->operation == 1) {
        char *parent = g_path_get_dirname(ui->directory);
        if (g_mkdir_with_parents(parent, 0700) != 0) job->status = UMI_STATUS_IO_ERROR;
        else job->status = UmiDesktopWorkspaceOpenDirectory(ui->directory, &ui->workspace);
        g_free(parent);
    } else if (job->operation == 2)
        job->status = UmiDesktopWorkspaceCommit(ui->workspace, job->snapshot->revision, job->snapshot);
    else if (job->operation == 3)
        job->status = UmiDesktopWorkspaceReadCheckpoint(ui->workspace, job->checkpoint, job->snapshot);
    else if (job->operation == 4)
        job->status = UmiDesktopWorkspaceRestore(ui->workspace, job->snapshot->revision, job->checkpoint);
    else if (job->operation == 5) job->status = UmiDesktopWorkspaceCloseClean(ui->workspace);
    else job->status = UMI_STATUS_INVALID_ARGUMENT;
    if (job->status == UMI_STATUS_OK && job->operation != 3 && job->operation != 5)
        job->status = UmiDesktopWorkspaceRead(ui->workspace, job->snapshot);
    (void)snprintf(job->message, sizeof job->message, "%s", UmiDesktopWorkspaceDetail(ui->workspace));
    g_task_return_boolean(task, TRUE);
}
static void FreeJob(gpointer data)
{ WorkspaceJob *job = data; free(job->snapshot); free(job); }
typedef struct QuestionUi { WorkspaceUi *owner; GtkWindow *window; uint64_t checkpoint; } QuestionUi;
static void FreeQuestion(gpointer data)
{
    QuestionUi *q = data;
    if (q->owner->question == q->window) q->owner->question = NULL;
    g_object_unref(q->owner->window); free(q);
}
static void QuestionAction(GtkButton *button, gpointer data)
{
    QuestionUi *q = data; WorkspaceUi *ui = q->owner;
    int response = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "response"));
    uint64_t revision = q->checkpoint;
    ui->question = NULL;
    if (response == 1 && revision && !ui->busy && !ui->dirty) Begin(ui, 4, revision);
    else if (response == 2 && ReadControls(ui)) { ui->closing = 1; Begin(ui, 2, 0); }
    else if (response == 3) { ui->dirty = 0; ui->closing = 1; Begin(ui, 5, 0); }
    gtk_window_destroy(q->window);
}
static void QuestionButton(GtkWidget *box, QuestionUi *q, const char *label, int response)
{
    GtkWidget *button = gtk_button_new_with_label(label); gtk_box_append(GTK_BOX(box), button);
    g_object_set_data(G_OBJECT(button), "response", GINT_TO_POINTER(response));
    g_signal_connect(button, "clicked", G_CALLBACK(QuestionAction), q);
}
static void Question(WorkspaceUi *ui, const char *title, const char *message, uint64_t revision)
{
    QuestionUi *q = calloc(1, sizeof *q);
    if (!q) { Status(ui, "Not enough memory to open the confirmation."); return; }
    q->owner = ui; q->checkpoint = revision; q->window = GTK_WINDOW(gtk_window_new());
    g_object_ref(ui->window);
    gtk_window_set_title(q->window, title); gtk_window_set_default_size(q->window, 500, 180);
    gtk_window_set_transient_for(q->window, ui->window); gtk_window_set_modal(q->window, TRUE);
    gtk_window_set_destroy_with_parent(q->window, TRUE);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_top(box, 18); gtk_widget_set_margin_bottom(box, 18);
    gtk_widget_set_margin_start(box, 18); gtk_widget_set_margin_end(box, 18);
    GtkWidget *label = gtk_label_new(message); gtk_label_set_wrap(GTK_LABEL(label), TRUE); gtk_box_append(GTK_BOX(box), label);
    GtkWidget *buttons = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); gtk_box_append(GTK_BOX(box), buttons);
    QuestionButton(buttons, q, "Cancel", 0);
    if (revision) QuestionButton(buttons, q, "Restore as new checkpoint", 1);
    else { QuestionButton(buttons, q, "Discard draft and close", 3); QuestionButton(buttons, q, "Save and close", 2); }
    gtk_window_set_child(q->window, box); ui->question = q->window;
    g_object_set_data_full(G_OBJECT(q->window), "umicom.workspace.question", q, FreeQuestion);
    gtk_window_present(q->window);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; WorkspaceUi *ui = data; GTask *task = G_TASK(result);
    WorkspaceJob *job = g_task_get_task_data(task); (void)g_task_propagate_boolean(task, NULL);
    if (job->application) {
        g_application_release(G_APPLICATION(job->application));
        g_clear_object(&job->application);
    }
    ui->busy = 0;
    g_object_set_data(G_OBJECT(ui->window), "umicom.desktop-workspace.busy", NULL);
    /* The task owns a reference to the window, and the window owns the root.
     * An external destroy can unparent it, but never frees our callback data
     * while this task is running. Do not redraw an externally removed body. */
    if (gtk_widget_get_root(ui->root) != GTK_ROOT(ui->window)) return;
    if (job->status != UMI_STATUS_OK) {
        char text[400]; (void)snprintf(text, sizeof text, "Operation stopped (status %d). %.255s", (int)job->status, job->message);
        ui->closing = 0; Status(ui, text); Buttons(ui); return;
    }
    if (job->operation == 5) {
        UmiDesktopWorkspaceDestroy(ui->workspace); ui->workspace = NULL;
        ui->allowClose = 1; gtk_window_destroy(ui->window); return;
    }
    if (job->operation != 3) { *ui->draft = *job->snapshot; ui->dirty = 0; Render(ui); Status(ui, job->message); }
    if (ui->closing) { Begin(ui, 5, 0); return; }
    if (job->operation == 3) {
        char message[320]; (void)snprintf(message, sizeof message,
            "Checkpoint %" PRIu64 " contains %zu notes and a %u pt workspace font.\n"
            "Restore creates a new saved revision. It does not change other applications or business transactions.",
            job->snapshot->revision, job->snapshot->noteCount, job->snapshot->fontPoints);
        Question(ui, "Restore saved checkpoint?", message, job->checkpoint);
    }
    Buttons(ui);
}
static void Begin(WorkspaceUi *ui, int operation, uint64_t checkpoint)
{
    if (ui->busy || (operation != 1 && !ui->workspace)) return;
    WorkspaceJob *job = calloc(1, sizeof *job);
    if (!job) { Status(ui, "Not enough memory."); return; }
    job->snapshot = malloc(sizeof *job->snapshot);
    if (!job->snapshot) { free(job); Status(ui, "Not enough memory."); return; }
    job->ui = ui; job->operation = operation; job->checkpoint = checkpoint; *job->snapshot = *ui->draft;
    ui->busy = 1; Buttons(ui); Status(ui, "Working with the saved workspace. Closing waits for this operation to finish.");
    job->application = gtk_window_get_application(ui->window);
    if (job->application) {
        g_object_ref(job->application);
        g_application_hold(G_APPLICATION(job->application));
    }
    GTask *task = g_task_new(ui->window, NULL, Complete, ui);
    g_task_set_task_data(task, job, FreeJob); g_task_run_in_thread(task, Run); g_object_unref(task);
}
static void Open(GtkButton *button, gpointer data) { (void)button; Begin(data, 1, 0); }
static void Save(GtkButton *button, gpointer data)
{ (void)button; WorkspaceUi *ui = data; if (!ui->busy && ui->workspace && ReadControls(ui)) Begin(ui, 2, 0); }
static void Preview(GtkButton *button, gpointer data)
{
    (void)button; WorkspaceUi *ui = data;
    if (ui->busy || ui->dirty || !ui->workspace) return;
    const char *text = gtk_editable_get_text(GTK_EDITABLE(ui->checkpoint)); char *end = NULL;
    errno = 0; guint64 value = g_ascii_strtoull(text, &end, 10);
    if (!*text || *text == '-' || *text == '+' || errno || !end || *end || !value) {
        Status(ui, "Enter a retained checkpoint number, using decimal digits."); return;
    }
    Begin(ui, 3, value);
}
static gboolean Close(GtkWindow *window, gpointer data)
{
    (void)window; WorkspaceUi *ui = data;
    if (ui->allowClose) return FALSE;
    if (ui->question) { gtk_window_present(ui->question); return TRUE; }
    if (ui->busy) { ui->closing = 1; return TRUE; }
    if (!ui->workspace) return FALSE;
    if (ui->dirty) {
        Question(ui, "Close workspace?", "Save the current draft before closing, or discard only the unsaved edits. Existing checkpoints remain saved.", 0);
        return TRUE;
    }
    ui->closing = 1; Begin(ui, 5, 0); return TRUE;
}
static void FreeUi(gpointer data)
{
    WorkspaceUi *ui = data;
    ui->busy = 1; ui->loading = 1;
    UmiDesktopWorkspaceDestroy(ui->workspace); ui->workspace = NULL; free(ui->draft);
    gtk_style_context_remove_provider_for_display(ui->display, GTK_STYLE_PROVIDER(ui->css));
    g_object_unref(ui->css); g_object_unref(ui->display); g_object_unref(ui->titles); g_object_unref(ui->root);
    g_free(ui->cssClass); g_free(ui->directory); free(ui);
}
static GtkWidget *Button(GtkWidget *box, const char *name, const char *label, GCallback callback, WorkspaceUi *ui)
{
    GtkWidget *button = gtk_button_new_with_label(label); gtk_widget_set_name(button, name);
    gtk_box_append(GTK_BOX(box), button); g_signal_connect(button, "clicked", callback, ui); return button;
}
static void Margins(GtkWidget *widget, int size)
{ gtk_widget_set_margin_top(widget,size);gtk_widget_set_margin_bottom(widget,size);gtk_widget_set_margin_start(widget,size);gtk_widget_set_margin_end(widget,size); }
GtkWindow *UmiDesktopWorkspaceGtkCreate(GtkApplication *application, const char *directory)
{
    WorkspaceUi *ui = calloc(1, sizeof *ui); if (!ui) return NULL;
    ui->draft = calloc(1, sizeof *ui->draft); if (!ui->draft) { free(ui); return NULL; }
    UmiDesktopWorkspaceSnapshotInit(ui->draft);
    ui->directory = directory ? g_strdup(directory) : g_build_filename(g_get_user_data_dir(), "umicom", "desktop-workspace", NULL);
    ui->window = GTK_WINDOW(application ? gtk_application_window_new(application) : gtk_window_new());
    gtk_window_set_title(ui->window, "Umicom Desktop Workspace"); gtk_window_set_default_size(ui->window, 900, 690);
    ui->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10); g_object_ref_sink(ui->root); Margins(ui->root, 16);
    char *id = g_uuid_string_random(); ui->cssClass = g_strconcat("umicom-workspace-", id, NULL); g_free(id);
    gtk_widget_add_css_class(ui->root, ui->cssClass);
    ui->display = g_object_ref(gtk_widget_get_display(GTK_WIDGET(ui->window))); ui->css = gtk_css_provider_new();
    gtk_style_context_add_provider_for_display(ui->display, GTK_STYLE_PROVIDER(ui->css), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    GBytes *bytes = g_bytes_new_static(UMICOM_SYSTEM_LOGO, sizeof UMICOM_SYSTEM_LOGO);
    GdkTexture *texture = gdk_texture_new_from_bytes(bytes, NULL); g_bytes_unref(bytes);
    if (texture) { GtkWidget *logo = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture)); g_object_unref(texture);
        gtk_widget_set_size_request(logo, 202, 55); gtk_widget_set_halign(logo, GTK_ALIGN_START); gtk_box_append(GTK_BOX(ui->root), logo); }
    bytes = g_bytes_new_static(UMICOM_SYSTEM_ICON, sizeof UMICOM_SYSTEM_ICON);
    texture = gdk_texture_new_from_bytes(bytes, NULL); g_bytes_unref(bytes);
    if (texture) {
        GtkWidget *icon = gtk_picture_new_for_paintable(GDK_PAINTABLE(texture)); g_object_unref(texture);
        gtk_widget_set_size_request(icon, 32, 32); gtk_widget_set_halign(icon, GTK_ALIGN_END);
        gtk_widget_set_tooltip_text(icon, "Umicom Desktop Workspace"); gtk_box_append(GTK_BOX(ui->root), icon);
    }
    GtkWidget *path = gtk_label_new(ui->directory); gtk_label_set_wrap(GTK_LABEL(path), TRUE); gtk_label_set_selectable(GTK_LABEL(path), TRUE);
    gtk_label_set_xalign(GTK_LABEL(path), 0); gtk_box_append(GTK_BOX(ui->root), path);
    ui->open = Button(ui->root, "umicom.workspace.open", "Open workspace", G_CALLBACK(Open), ui);
    ui->revision = gtk_label_new("No workspace opened");gtk_label_set_xalign(GTK_LABEL(ui->revision), 0);gtk_box_append(GTK_BOX(ui->root), ui->revision);
    ui->editor = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);gtk_box_append(GTK_BOX(ui->root), ui->editor);
    GtkWidget *settings = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);gtk_box_append(GTK_BOX(ui->editor), settings);
    const char *themes[] = {"System appearance", "Light workspace", "Dark workspace", NULL};
    ui->theme = gtk_drop_down_new_from_strings(themes);gtk_widget_set_name(ui->theme,"umicom.workspace.theme");gtk_box_append(GTK_BOX(settings), ui->theme);
    ui->font = gtk_spin_button_new_with_range(10,28,1);gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->font),12);gtk_widget_set_tooltip_text(ui->font,"Workspace font size in points");gtk_box_append(GTK_BOX(settings), ui->font);
    ui->visible = gtk_check_button_new_with_label("Show note list");gtk_check_button_set_active(GTK_CHECK_BUTTON(ui->visible),TRUE);gtk_box_append(GTK_BOX(settings), ui->visible);
    ui->sidebar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);gtk_box_append(GTK_BOX(ui->editor),ui->sidebar);
    ui->titles = gtk_string_list_new(NULL);ui->notes = gtk_drop_down_new(G_LIST_MODEL(g_object_ref(ui->titles)),NULL);gtk_widget_set_hexpand(ui->notes,TRUE);gtk_box_append(GTK_BOX(ui->sidebar),ui->notes);
    ui->add=Button(ui->sidebar,"umicom.workspace.add","New note",G_CALLBACK(AddNote),ui);
    ui->remove=Button(ui->sidebar,"umicom.workspace.remove","Remove from draft",G_CALLBACK(RemoveNote),ui);
    ui->title=gtk_entry_new();gtk_widget_set_name(ui->title,"umicom.workspace.title");gtk_entry_set_placeholder_text(GTK_ENTRY(ui->title),"Note title");gtk_box_append(GTK_BOX(ui->editor),ui->title);
    ui->body=gtk_text_view_new();gtk_widget_set_name(ui->body,"umicom.workspace.body");gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(ui->body),GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll=gtk_scrolled_window_new();gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),ui->body);gtk_widget_set_vexpand(scroll,TRUE);gtk_widget_set_size_request(scroll,-1,210);gtk_box_append(GTK_BOX(ui->editor),scroll);
    GtkWidget *actions=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,8);gtk_box_append(GTK_BOX(ui->root),actions);
    ui->save=Button(actions,"umicom.workspace.save","Save checkpoint",G_CALLBACK(Save),ui);
    ui->discard=Button(actions,"umicom.workspace.discard","Discard draft",G_CALLBACK(Discard),ui);
    ui->checkpoint=gtk_entry_new();gtk_widget_set_tooltip_text(ui->checkpoint,"Retained checkpoint number");gtk_editable_set_text(GTK_EDITABLE(ui->checkpoint),"1");gtk_widget_set_size_request(ui->checkpoint,80,-1);gtk_box_append(GTK_BOX(actions),ui->checkpoint);
    ui->preview=Button(actions,"umicom.workspace.preview","Preview checkpoint",G_CALLBACK(Preview),ui);
    ui->status=gtk_label_new("Open workspace to begin. Nothing is saved until you choose to open storage and commit a checkpoint.");gtk_label_set_wrap(GTK_LABEL(ui->status),TRUE);gtk_label_set_xalign(GTK_LABEL(ui->status),0);gtk_box_append(GTK_BOX(ui->root),ui->status);
    GtkWidget *page=gtk_scrolled_window_new();gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(page),ui->root);gtk_window_set_child(ui->window,page);
    g_signal_connect(ui->title,"changed",G_CALLBACK(Changed),ui);g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(ui->body)),"changed",G_CALLBACK(Changed),ui);
    g_signal_connect(ui->theme,"notify::selected",G_CALLBACK(NotifyChanged),ui);g_signal_connect(ui->font,"value-changed",G_CALLBACK(Changed),ui);
    g_signal_connect(ui->visible,"toggled",G_CALLBACK(Changed),ui);g_signal_connect(ui->notes,"notify::selected",G_CALLBACK(SelectNote),ui);
    g_signal_connect(ui->window,"close-request",G_CALLBACK(Close),ui);g_object_set_data_full(G_OBJECT(ui->window),"umicom.desktop-workspace",ui,FreeUi);
    Buttons(ui); return ui->window;
}
static void Launch(GtkButton *button, gpointer data)
{
    (void)data;GtkRoot *root=gtk_widget_get_root(GTK_WIDGET(button));
    GtkApplication *app=GTK_IS_WINDOW(root)?gtk_window_get_application(GTK_WINDOW(root)):NULL;
    GtkWindow *window=UmiDesktopWorkspaceGtkCreate(app,NULL);
    if(window)gtk_window_present(window);
}
UmiStatus UmiDesktopWorkspaceGtkAttach(GtkWidget *box)
{
    if(!GTK_IS_BOX(box))return UMI_STATUS_INVALID_ARGUMENT;
    GtkWidget *button=gtk_button_new_with_label("Open persistent desktop workspace");
    gtk_widget_set_name(button,"umicom.workspace.launcher");
    gtk_widget_set_tooltip_text(button,"Saved notes, workspace appearance and recoverable checkpoints. Does not change the operating-system session.");
    g_signal_connect(button,"clicked",G_CALLBACK(Launch),NULL);gtk_box_append(GTK_BOX(box),button);return UMI_STATUS_OK;
}
