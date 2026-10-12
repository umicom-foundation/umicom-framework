/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workspace_library_gtk4.c
 * PURPOSE: Share visible library selection and asynchronous picker ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workspace_library.h"
#include <string.h>
#include "umicom/platform/recent_items.h"

struct UmiWorkspaceLibraryGtk {
    GtkWidget *root, *path, *message, *active, *confirm;
    GPtrArray *controls;
    GtkFileDialog *chooser;
    GCancellable *cancel;
    UmiWorkspaceLibraryOpen open;
    void *context;
    char *current;
    char *historyPath, *chooserDraft;
    GtkWidget *recent, *historyMessage;
    GQueue historyQueue;
    bool historyBusy;
    bool closed, choosingNew;
};

static void Tag(GtkWidget *widget, const char *id)
{
    g_object_set_data_full(G_OBJECT(widget), "umicom-automation-id", g_strdup(id), g_free);
}
static GtkWidget *Label(GtkWidget *box, const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_box_append(GTK_BOX(box), label);
    return label;
}
static void Message(UmiWorkspaceLibraryGtk *library, UmiStatus status)
{
    const char *message = "The library could not open. The current workspace is unchanged.";
    switch (status) {
    case UMI_STATUS_OK: message = "Library ready. Its full location is shown above."; break;
    case UMI_STATUS_INVALID_ARGUMENT: message = "Choose an absolute path to a local database file."; break;
    case UMI_STATUS_ALREADY_EXISTS: message = "That file already exists. Use Open existing or choose a new filename."; break;
    case UMI_STATUS_NOT_FOUND: message = "The file or its parent folder does not exist. Check the path before continuing."; break;
    case UMI_STATUS_PERMISSION_DENIED: message = "This location is not writable. Choose a folder you can write to."; break;
    case UMI_STATUS_BUSY: message = "Finish or cancel the current operation before changing libraries."; break;
    case UMI_STATUS_UNAVAILABLE: message = "SQLite storage is unavailable in this build."; break;
    default: break;
    }
    gtk_label_set_text(GTK_LABEL(library->message), message);
}
static void Open(GtkButton *button, gpointer data)
{
    UmiWorkspaceLibraryGtk *library = data;
    if (library->closed || library->chooser != NULL) return;
    const char *input = gtk_editable_get_text(GTK_EDITABLE(library->path));
    if (!g_path_is_absolute(input)) { Message(library, UMI_STATUS_INVALID_ARGUMENT); return; }
    char *path = g_canonicalize_filename(input, NULL);
    bool create = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "library-create")) != 0;
    /* Reopening the displayed library is a no-op. In particular, it must not
     * destroy unsaved form input or reset an in-progress inspection. */
    if (library->current != NULL && strcmp(path, library->current) == 0) {
        Message(library, create ? UMI_STATUS_ALREADY_EXISTS : UMI_STATUS_OK);
        g_free(path); return;
    }
    if (library->current != NULL && !gtk_check_button_get_active(GTK_CHECK_BUTTON(library->confirm))) {
        gtk_label_set_text(GTK_LABEL(library->message), "Confirm closing the current forms before changing libraries. Saved work remains in its original file.");
        g_free(path); return;
    }
    UmiStatus status = library->open(path, create, library->context);
    if (status == UMI_STATUS_OK) {
        /* Publish the selected location only after the host has fully opened
         * its replacement services. A rejected candidate leaves this untouched. */
        UmiWorkspaceLibraryGtkSetCurrentPath(library, path);
        if (library->historyPath != NULL) (void)UmiWorkspaceLibraryGtkRememberCurrent(library);
    }
    Message(library, status); g_free(path);
}
/* Retain the earlier chooser completion for review. The replacement below
 * also checks the typed draft, so a late native selection cannot erase newer
 * user input. Both implementations keep the host closed after retirement. */
#if 0
static void Selected(GObject *source, GAsyncResult *result, gpointer data)
{
    /* The completion owns only the root reference. Destroy marks its context
     * closed before cancelling, so a late chooser cannot call the application. */
    GtkWidget *root = data;
    UmiWorkspaceLibraryGtk *library = g_object_get_data(G_OBJECT(root), "umicom-library-owner");
    GError *error = NULL;
    GFile *file = library->choosingNew ? gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, &error) :
        gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    g_clear_object(&library->chooser); g_clear_object(&library->cancel);
    if (!library->closed && file != NULL) {
        char *path = g_file_get_path(file);
        if (path != NULL) UmiWorkspaceLibraryGtkSetPath(library, path);
        else Message(library, UMI_STATUS_INVALID_ARGUMENT);
        g_free(path);
    }
    if (!library->closed && error != NULL &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
        !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        gtk_label_set_text(GTK_LABEL(library->message), "The file picker could not open. You can still type an absolute path.");
    g_clear_object(&file); g_clear_error(&error); g_object_unref(root);
}
#endif
static void Selected(GObject *source, GAsyncResult *result, gpointer data)
{
    /* The completion owns only the root reference. Destroy marks its context
     * closed before cancelling, so a late chooser cannot call the application. */
    GtkWidget *root = data;
    UmiWorkspaceLibraryGtk *library = g_object_get_data(G_OBJECT(root), "umicom-library-owner");
    GError *error = NULL;
    GFile *file = library->choosingNew ? gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, &error) :
        gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    g_clear_object(&library->chooser); g_clear_object(&library->cancel);
    /* The user may type while a native dialog is visible. Its late result must
     * not replace that newer draft or automatically open either location. */
    bool unchanged = !library->closed && g_strcmp0(library->chooserDraft,
        gtk_editable_get_text(GTK_EDITABLE(library->path))) == 0;
    g_clear_pointer(&library->chooserDraft, g_free);
    if (unchanged && file != NULL) {
        char *path = g_file_get_path(file);
        if (path != NULL) UmiWorkspaceLibraryGtkSetPath(library, path);
        else Message(library, UMI_STATUS_INVALID_ARGUMENT);
        g_free(path);
    }
    if (!library->closed && error != NULL &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
        !g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) &&
        !g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED))
        gtk_label_set_text(GTK_LABEL(library->message), "The file picker could not open. You can still type an absolute path.");
    g_clear_object(&file); g_clear_error(&error); g_object_unref(root);
}
static void Browse(GtkButton *button, gpointer data)
{
    UmiWorkspaceLibraryGtk *library = data;
    if (library->closed || library->chooser != NULL) return;
    g_free(library->chooserDraft);
    library->chooserDraft = g_strdup(gtk_editable_get_text(GTK_EDITABLE(library->path)));
    library->choosingNew = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "library-create")) != 0;
    library->chooser = gtk_file_dialog_new(); library->cancel = g_cancellable_new();
    gtk_file_dialog_set_title(library->chooser, library->choosingNew ? "Choose a new library location" : "Choose an existing library");
    gtk_file_dialog_set_modal(library->chooser, TRUE);
    GtkRoot *root = gtk_widget_get_root(library->root);
    GtkWindow *parent = GTK_IS_WINDOW(root) ? GTK_WINDOW(root) : NULL;
    if (library->choosingNew) {
        gtk_file_dialog_set_initial_name(library->chooser, "workspace.sqlite3");
        gtk_file_dialog_save(library->chooser, parent, library->cancel, Selected, g_object_ref(library->root));
    } else gtk_file_dialog_open(library->chooser, parent, library->cancel, Selected, g_object_ref(library->root));
}
static void Button(UmiWorkspaceLibraryGtk *library, GtkWidget *box, const char *label,
    const char *id, bool create, GCallback callback)
{
    GtkWidget *button = gtk_button_new_with_label(label); Tag(button, id);
    g_object_set_data(G_OBJECT(button), "library-create", GINT_TO_POINTER(create));
    g_ptr_array_add(library->controls, g_object_ref(button));
    g_signal_connect(button, "clicked", callback, library);
    gtk_box_append(GTK_BOX(box), button);
}
/* Keep metadata work separate from database ownership. This implementation
 * borrows only the selector and uses its root reference for late completions. */
#include "workspace_library_history_gtk4.inc"

static void Free(gpointer data)
{
    UmiWorkspaceLibraryGtk *library = data;
    g_clear_object(&library->chooser); g_clear_object(&library->cancel);
    g_queue_clear_full(&library->historyQueue, HistoryJobFree);
    g_free(library->historyPath); g_free(library->chooserDraft);
    g_ptr_array_unref(library->controls); g_free(library->current); g_free(library);
}
UmiStatus UmiWorkspaceLibraryGtkCreate(UmiWorkspaceLibraryOpen open, void *context, UmiWorkspaceLibraryGtk **outLibrary)
{
    if (outLibrary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *outLibrary = NULL;
    if (open == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiWorkspaceLibraryGtk *library = g_new0(UmiWorkspaceLibraryGtk, 1);
    library->open = open; library->context = context;
    library->controls = g_ptr_array_new_with_free_func(g_object_unref);
    library->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8); g_object_ref_sink(library->root);
    g_object_set_data_full(G_OBJECT(library->root), "umicom-library-owner", library, Free);
    Tag(library->root, "workspace.library");
    Label(library->root, "Choose where your library is stored");
    Label(library->root, "Create a new database or open one you already saved. Files stay on this computer; their contents are not encrypted.");
    library->path = gtk_entry_new(); Tag(library->path, "workspace.library.path");
    gtk_entry_set_placeholder_text(GTK_ENTRY(library->path), "Absolute path to a local .sqlite3 file");
    gtk_box_append(GTK_BOX(library->root), library->path);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(library->root), row);
    Button(library, row, "Browse existing...", "workspace.library.browse", false, G_CALLBACK(Browse));
    Button(library, row, "Choose new location...", "workspace.library.choose_new", true, G_CALLBACK(Browse));
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6); gtk_box_append(GTK_BOX(library->root), row);
    Button(library, row, "Open existing", "workspace.library.open", false, G_CALLBACK(Open));
    Button(library, row, "Create new", "workspace.library.create", true, G_CALLBACK(Open));
    library->confirm = gtk_check_button_new_with_label("Close current forms and undo history when changing libraries");
    Tag(library->confirm, "workspace.library.confirm");
    gtk_box_append(GTK_BOX(library->root), library->confirm); gtk_widget_set_visible(library->confirm, FALSE);
    library->active = Label(library->root, "No library is open."); Tag(library->active, "workspace.library.active");
    gtk_label_set_selectable(GTK_LABEL(library->active), TRUE);
    library->message = Label(library->root, "Selecting a filename does not open or change it."); Tag(library->message, "workspace.library.status");
    *outLibrary = library; return UMI_STATUS_OK;
}
GtkWidget *UmiWorkspaceLibraryGtkWidget(UmiWorkspaceLibraryGtk *library) { return library != NULL ? library->root : NULL; }
void UmiWorkspaceLibraryGtkSetPath(UmiWorkspaceLibraryGtk *library, const char *path)
{
    if (library != NULL && !library->closed && path != NULL && g_utf8_validate(path, -1, NULL))
        gtk_editable_set_text(GTK_EDITABLE(library->path), path);
}
void UmiWorkspaceLibraryGtkSetCurrentPath(UmiWorkspaceLibraryGtk *library, const char *path)
{
    if (library == NULL || library->closed || path == NULL || !g_utf8_validate(path, -1, NULL) ||
        !g_path_is_absolute(path)) return;
    char *canonical = g_canonicalize_filename(path, NULL);
    g_free(library->current); library->current = canonical;
    gtk_label_set_text(GTK_LABEL(library->active), canonical);
    gtk_widget_set_visible(library->confirm, TRUE);
    gtk_check_button_set_active(GTK_CHECK_BUTTON(library->confirm), FALSE);
}
void UmiWorkspaceLibraryGtkDestroy(UmiWorkspaceLibraryGtk *library)
{
    if (library == NULL || library->closed) return;
    library->closed = true; library->context = NULL; library->open = NULL;
    /* Finish the one accepted metadata write on its worker. Unstarted jobs
     * retire here, and completion will never call the released application. */
    g_queue_clear_full(&library->historyQueue, HistoryJobFree);
    if (library->cancel != NULL) g_cancellable_cancel(library->cancel);
    for (guint index = 0U; index < library->controls->len; ++index)
        g_signal_handlers_disconnect_by_data(g_ptr_array_index(library->controls, index), library);
    gtk_widget_set_sensitive(library->root, FALSE);
    g_object_unref(library->root);
}
