/*-----------------------------------------------------------------------------
 * Umicom Framework — application development lessons
 * File: examples/learning/projects/notes_window.c
 * PURPOSE: Compose Framework widgets around the shared Notes model with menus and close protection.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <string.h>
#include "umicom/ui/gtk4/component_factory.h"
#include "notes_model.h"

typedef struct NotesWindow {
    UmiLearningNotes *model;
    GtkWindow *window;
    GtkWidget *editor;
    GtkWidget *path;
    GtkWidget *status;
    int editRejected;
    int refreshing;
    int discardConfirmed;
} NotesWindow;

/* Reuse Framework specifications; the adapter only arranges GTK widgets. */
static GtkWidget *Component(UmiUiComponentKind kind, const char *id, const char *text)
{
    UmiUiComponentSpec spec = umi_ui_component_spec_default(kind);
    if (umi_ui_component_spec_set_id(&spec, id) != UMI_STATUS_OK ||
        umi_ui_component_spec_set_text(&spec, text) != UMI_STATUS_OK) return NULL;
    spec.orientation = UMI_UI_VERTICAL;
    spec.spacing = 8;
    spec.hexpand = true;
    spec.visible = kind != UMI_UI_COMPONENT_WINDOW;
    return umi_gtk4_component_create(&spec);
}

static void DestroyState(gpointer data)
{
    NotesWindow *state = data;
    UmiLearningNotesDestroy(state->model);
    g_free(state);
}

static void ShowStatus(NotesWindow *state, UmiStatus status)
{
    const char *message = status == UMI_STATUS_OK ? "Ready. Changes are kept in the document model." :
        status == UMI_STATUS_ALREADY_EXISTS ? "That file already exists. Choose a new filename." :
        status == UMI_STATUS_INVALID_STATE ? "Save the current note first, or resolve an external file change." :
        umi_status_text(status);
    gtk_label_set_text(GTK_LABEL(state->status), message);
}

static void RefreshText(NotesWindow *state)
{
    char *text = NULL;
    UmiStatus status = UmiLearningNotesCopyText(state->model, &text);
    if (status == UMI_STATUS_OK) {
        state->refreshing = 1;
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->editor)), text, -1);
        state->refreshing = 0;
    }
    umi_document_store_free_text(text);
    ShowStatus(state, status);
}

static void TextChanged(GtkTextBuffer *buffer, gpointer userData)
{
    NotesWindow *state = userData;
    GtkTextIter first, last;
    if (state->refreshing) return;
    gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *text = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    UmiStatus status = UmiLearningNotesSetText(state->model, text);
    g_free(text);
    state->editRejected = status != UMI_STATUS_OK;
    ShowStatus(state, status);
}

static void ExecuteAction(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
    NotesWindow *state = userData;
    const char *name = g_action_get_name(G_ACTION(action));
    const char *command = strcmp(name, "save") == 0 ? "notes.save" :
        strcmp(name, "undo") == 0 ? "notes.undo" : "notes.new";
    (void)parameter;
    if (strcmp(name, "close") == 0) { gtk_window_close(state->window); return; }
    if (state->editRejected) {
        gtk_label_set_text(GTK_LABEL(state->status),
            "The last edit could not be stored. Shorten or repair the text before saving or replacing it.");
        return;
    }
    UmiStatus status = UmiLearningNotesExecute(state->model, command,
        strcmp(name, "save") == 0 ? gtk_editable_get_text(GTK_EDITABLE(state->path)) : NULL);
    if (status == UMI_STATUS_OK && strcmp(name, "save") != 0) RefreshText(state);
    else ShowStatus(state, status);
    if (status == UMI_STATUS_OK && strcmp(name, "save") == 0)
        gtk_label_set_text(GTK_LABEL(state->status), "Note saved.");
}

static gboolean CloseRequested(GtkWindow *window, gpointer userData)
{
    NotesWindow *state = userData;
    UmiDocumentWorkingCopySnapshot snapshot;
    (void)window;
    UmiStatus status = UmiLearningNotesSnapshot(state->model, &snapshot);
    if (!state->discardConfirmed && (state->editRejected || status != UMI_STATUS_OK ||
        (snapshot.dirty && (snapshot.has_path || snapshot.text_length != 0U)))) {
        gtk_label_set_text(GTK_LABEL(state->status),
            "This note has unsaved changes. Save it, or use Discard and Close.");
        return TRUE;
    }
    return FALSE;
}

static void DiscardAndClose(GtkButton *button, gpointer userData)
{
    NotesWindow *state = userData;
    (void)button;
    state->discardConfirmed = 1;
    gtk_window_close(state->window);
}

static void Activate(GtkApplication *application, gpointer userData)
{
    int *startupFailed = userData;
    GtkWindow *existing = gtk_application_get_active_window(application);
    if (existing != NULL) { gtk_window_present(existing); return; }
    NotesWindow *state = g_new0(NotesWindow, 1);
    if (UmiLearningNotesCreate(&state->model) != UMI_STATUS_OK) {
        *startupFailed = 1; g_free(state); g_application_quit(G_APPLICATION(application)); return;
    }
    GtkWidget *window = Component(UMI_UI_COMPONENT_WINDOW, "notes.window", "Umicom Notes");
    if (window == NULL) { *startupFailed = 1; DestroyState(state); g_application_quit(G_APPLICATION(application)); return; }
    state->window = GTK_WINDOW(window);
    gtk_window_set_application(state->window, application);
    gtk_window_set_default_size(state->window, 900, 600);
    g_object_set_data_full(G_OBJECT(window), "notes-state", state, DestroyState);
    GtkWidget *root = Component(UMI_UI_COMPONENT_BOX, "notes.root", "");
    gtk_window_set_child(state->window, root);
    GtkWidget *header = Component(UMI_UI_COMPONENT_LABEL, "notes.heading", "Umicom Notes — project notebook");
    gtk_box_append(GTK_BOX(root), header);

    const char *names[] = {"new", "save", "undo", "close"};
    GSimpleActionGroup *actions = g_simple_action_group_new();
    for (size_t index = 0U; index < sizeof(names)/sizeof(names[0]); ++index) {
        GSimpleAction *action = g_simple_action_new(names[index], NULL);
        g_signal_connect(action, "activate", G_CALLBACK(ExecuteAction), state);
        g_action_map_add_action(G_ACTION_MAP(actions), G_ACTION(action));
        g_object_unref(action);
    }
    gtk_widget_insert_action_group(root, "notes", G_ACTION_GROUP(actions));
    g_object_unref(actions);
    GMenu *menu = g_menu_new();
    GMenu *file = g_menu_new(); GMenu *edit = g_menu_new();
    g_menu_append(file, "New Note", "notes.new"); g_menu_append(file, "Save Note", "notes.save");
    g_menu_append(file, "Close", "notes.close"); g_menu_append(edit, "Undo", "notes.undo");
    g_menu_append_submenu(menu, "File", G_MENU_MODEL(file));
    g_menu_append_submenu(menu, "Edit", G_MENU_MODEL(edit));
    gtk_box_append(GTK_BOX(root), gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu)));
    g_object_unref(file); g_object_unref(edit); g_object_unref(menu);

    GtkWidget *split = Component(UMI_UI_COMPONENT_PANED, "notes.panels", "");
    gtk_orientable_set_orientation(GTK_ORIENTABLE(split), GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_vexpand(split, TRUE); gtk_box_append(GTK_BOX(root), split);
    GtkWidget *details = Component(UMI_UI_COMPONENT_BOX, "notes.details", "");
    gtk_paned_set_start_child(GTK_PANED(split), details);
    gtk_box_append(GTK_BOX(details), Component(UMI_UI_COMPONENT_LABEL, "notes.path-label", "Save destination"));
    state->path = Component(UMI_UI_COMPONENT_ENTRY, "notes.path", "");
    char *destination = g_build_filename(g_get_home_dir(), "UmicomNote.txt", NULL);
    gtk_editable_set_text(GTK_EDITABLE(state->path), destination); g_free(destination);
    gtk_box_append(GTK_BOX(details), state->path);
    GtkWidget *save = Component(UMI_UI_COMPONENT_BUTTON, "notes.save", "Save Note");
    gtk_actionable_set_action_name(GTK_ACTIONABLE(save), "notes.save");
    gtk_box_append(GTK_BOX(details), save);
    GtkWidget *discard = Component(UMI_UI_COMPONENT_BUTTON, "notes.discard-close", "Discard and Close");
    g_signal_connect(discard, "clicked", G_CALLBACK(DiscardAndClose), state);
    gtk_box_append(GTK_BOX(details), discard);
    state->editor = Component(UMI_UI_COMPONENT_TEXT_VIEW, "notes.editor", "");
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(state->editor), GTK_WRAP_WORD_CHAR);
    GtkWidget *scroll = Component(UMI_UI_COMPONENT_SCROLLED, "notes.scroll", "");
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), state->editor);
    gtk_paned_set_end_child(GTK_PANED(split), scroll);
    state->status = Component(UMI_UI_COMPONENT_LABEL, "notes.status", "Write a note, choose a new filename and save it.");
    gtk_box_append(GTK_BOX(root), state->status);
    g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(state->editor)), "changed", G_CALLBACK(TextChanged), state);
    g_signal_connect(window, "close-request", G_CALLBACK(CloseRequested), state);
    gtk_window_present(state->window);
}

int main(int argc, char **argv)
{
    GtkApplication *application = gtk_application_new("org.umicom.learning.notes", G_APPLICATION_DEFAULT_FLAGS);
    int startupFailed = 0;
    g_signal_connect(application, "activate", G_CALLBACK(Activate), &startupFailed);
    int result = g_application_run(G_APPLICATION(application), argc, argv);
    g_object_unref(application);
    return startupFailed ? 1 : result;
}
