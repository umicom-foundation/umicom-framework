/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/path_picker_gtk4.c
 *
 * PURPOSE:
 *   Share asynchronous filename selection with weak widget ownership and stale-draft protection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/platform/path.h"
#include <string.h>

/* Validate and capture every input before any GTK notification. Applications
 * provide selected project/document context; Framework owns native navigation. */
/* Framework path identity rejects Windows drive-relative roots. The former GLib-only check is retained for review; the replacement cannot seed a location that depends on the current drive. */
#if 0
UmiStatus UmiPathPickerGtk4SetInitialLocation(GtkFileDialog *dialog,
    UmiPathPickerMode mode, const char *selectedPath,
    const char *fallbackFolder, const char *suggestedName)
{
    if (!GTK_IS_FILE_DIALOG(dialog) || mode < UMI_PATH_PICKER_EXISTING_FILE || mode > UMI_PATH_PICKER_DIRECTORY)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (selectedPath != NULL && selectedPath[0] == '\0') selectedPath = NULL;
    if (fallbackFolder != NULL && fallbackFolder[0] == '\0') fallbackFolder = NULL;
    if (suggestedName != NULL && suggestedName[0] == '\0') suggestedName = NULL;
    if ((selectedPath != NULL && (!g_utf8_validate(selectedPath, -1, NULL) || !g_path_is_absolute(selectedPath))) ||
        (fallbackFolder != NULL && (!g_utf8_validate(fallbackFolder, -1, NULL) || !g_path_is_absolute(fallbackFolder))) ||
        (suggestedName != NULL && (!g_utf8_validate(suggestedName, -1, NULL) ||
            strpbrk(suggestedName, "/\\") != NULL || strcmp(suggestedName, ".") == 0 || strcmp(suggestedName, "..") == 0)))
        return UMI_STATUS_INVALID_ARGUMENT;
    GFile *selected = selectedPath != NULL ? g_file_new_for_path(selectedPath) : NULL;
    GFile *folder = fallbackFolder != NULL ? g_file_new_for_path(fallbackFolder) : NULL;
    char *name = mode == UMI_PATH_PICKER_NEW_FILE ? g_strdup(suggestedName) : NULL;
    g_object_ref(dialog);
    g_object_freeze_notify(G_OBJECT(dialog));
    if (selected != NULL) {
        gtk_file_dialog_set_initial_file(dialog, selected);
    } else {
        gtk_file_dialog_set_initial_file(dialog, NULL);
        gtk_file_dialog_set_initial_folder(dialog, folder);
        gtk_file_dialog_set_initial_name(dialog, name);
    }
    g_object_thaw_notify(G_OBJECT(dialog));
    g_object_unref(dialog);
    g_clear_object(&selected);
    g_clear_object(&folder);
    g_free(name);
    return UMI_STATUS_OK;
}

#endif


/* Explicit folder and basename publication replaces the metadata-reading convenience setter, retaining selected-path precedence. Retained for engineering review. */
#if 0
UmiStatus UmiPathPickerGtk4SetInitialLocation(GtkFileDialog *dialog,
    UmiPathPickerMode mode, const char *selectedPath,
    const char *fallbackFolder, const char *suggestedName)
{
    if (!GTK_IS_FILE_DIALOG(dialog) || mode < UMI_PATH_PICKER_EXISTING_FILE || mode > UMI_PATH_PICKER_DIRECTORY)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (selectedPath != NULL && selectedPath[0] == '\0') selectedPath = NULL;
    if (fallbackFolder != NULL && fallbackFolder[0] == '\0') fallbackFolder = NULL;
    if (suggestedName != NULL && suggestedName[0] == '\0') suggestedName = NULL;
    if ((selectedPath != NULL && (!g_utf8_validate(selectedPath, -1, NULL) || !umi_path_is_absolute(selectedPath))) ||
        (fallbackFolder != NULL && (!g_utf8_validate(fallbackFolder, -1, NULL) || !umi_path_is_absolute(fallbackFolder))) ||
        (suggestedName != NULL && (!g_utf8_validate(suggestedName, -1, NULL) ||
            strpbrk(suggestedName, "/\\") != NULL || strcmp(suggestedName, ".") == 0 || strcmp(suggestedName, "..") == 0)))
        return UMI_STATUS_INVALID_ARGUMENT;
    GFile *selected = selectedPath != NULL ? g_file_new_for_path(selectedPath) : NULL;
    GFile *folder = fallbackFolder != NULL ? g_file_new_for_path(fallbackFolder) : NULL;
    char *name = mode == UMI_PATH_PICKER_NEW_FILE ? g_strdup(suggestedName) : NULL;
    g_object_ref(dialog);
    g_object_freeze_notify(G_OBJECT(dialog));
    if (selected != NULL) {
        gtk_file_dialog_set_initial_file(dialog, selected);
    } else {
        gtk_file_dialog_set_initial_file(dialog, NULL);
        gtk_file_dialog_set_initial_folder(dialog, folder);
        gtk_file_dialog_set_initial_name(dialog, name);
    }
    g_object_thaw_notify(G_OBJECT(dialog));
    g_object_unref(dialog);
    g_clear_object(&selected);
    g_clear_object(&folder);
    g_free(name);
    return UMI_STATUS_OK;
}

#endif
UmiStatus UmiPathPickerGtk4SetInitialLocation(GtkFileDialog *dialog,
    UmiPathPickerMode mode, const char *selectedPath,
    const char *fallbackFolder, const char *suggestedName)
{
    if (!GTK_IS_FILE_DIALOG(dialog) || mode < UMI_PATH_PICKER_EXISTING_FILE || mode > UMI_PATH_PICKER_DIRECTORY)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (selectedPath != NULL && selectedPath[0] == '\0') selectedPath = NULL;
    if (fallbackFolder != NULL && fallbackFolder[0] == '\0') fallbackFolder = NULL;
    if (suggestedName != NULL && suggestedName[0] == '\0') suggestedName = NULL;
    if ((selectedPath != NULL && (!g_utf8_validate(selectedPath, -1, NULL) || !umi_path_is_absolute(selectedPath))) ||
        (fallbackFolder != NULL && (!g_utf8_validate(fallbackFolder, -1, NULL) || !umi_path_is_absolute(fallbackFolder))) ||
        (suggestedName != NULL && (!g_utf8_validate(suggestedName, -1, NULL) ||
            strpbrk(suggestedName, "/\\") != NULL || strcmp(suggestedName, ".") == 0 || strcmp(suggestedName, "..") == 0)))
        return UMI_STATUS_INVALID_ARGUMENT;
    GFile *selected = selectedPath != NULL ? g_file_new_for_path(selectedPath) : NULL;
    GFile *folder = NULL;
    char *name = NULL;
    /* Derive chooser fields lexically. GTK's convenience file setter can query
     * filesystem metadata and has had stale cached-file behavior; composing the
     * folder and basename keeps this API free of I/O and repeated seeds exact. */
    if (selected != NULL) {
        folder = mode == UMI_PATH_PICKER_DIRECTORY ? g_object_ref(selected) : g_file_get_parent(selected);
        if (mode != UMI_PATH_PICKER_DIRECTORY && folder != NULL)
            name = g_file_get_basename(selected);
        if (folder == NULL) folder = g_object_ref(selected);
    } else {
        folder = fallbackFolder != NULL ? g_file_new_for_path(fallbackFolder) : NULL;
        name = mode == UMI_PATH_PICKER_NEW_FILE ? g_strdup(suggestedName) : NULL;
    }
    g_object_ref(dialog);
    g_object_freeze_notify(G_OBJECT(dialog));
    gtk_file_dialog_set_initial_file(dialog, NULL);
    gtk_file_dialog_set_initial_folder(dialog, folder);
    gtk_file_dialog_set_initial_name(dialog, name);
    g_object_thaw_notify(G_OBJECT(dialog));
    g_object_unref(dialog);
    g_clear_object(&selected);
    g_clear_object(&folder);
    g_free(name);
    return UMI_STATUS_OK;
}

typedef struct PathPickRequest {
    GWeakRef control, entry, window;
    GCancellable *cancel;
    UmiPathPickerMode mode;
    char *draft;
} PathPickRequest;

typedef struct PathPicker {
    GWeakRef entry;
    GtkWidget *message;
    UmiPathPickerMode mode;
    char *title, *suggestedName;
    PathPickRequest *pending; /* Borrowed until the dialog's completion callback. */
} PathPicker;

/* Policy belongs to the selected draft, so two pickers sharing an entry
 * publish to one entry-owned record. Typed edits invalidate that policy. */
typedef struct PathSelection {
    UmiPathPickerMode mode;
    char *path;
    gboolean has_mode, publishing;
} PathSelection;

static void FreeSelection(gpointer data)
{
    PathSelection *selection = data;
    g_free(selection->path);
    g_free(selection);
}

static void SelectionEdited(GtkEditable *entry, gpointer data)
{
    (void)entry;
    PathSelection *selection = data;
    if (!selection->publishing) selection->has_mode = FALSE;
}

static void SelectionUnmapped(GtkWidget *entry, gpointer data)
{
    (void)entry;
    PathSelection *selection = data;
    selection->has_mode = FALSE;
}

UmiStatus UmiPathPickerGtk4ReadSelectedMode(GtkEntry *entry, UmiPathPickerMode *out_mode)
{
    if (!GTK_IS_ENTRY(entry) || out_mode == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    PathSelection *selection = g_object_get_data(G_OBJECT(entry), "umicom-path-selection");
    if (selection != NULL && selection->publishing) return UMI_STATUS_BUSY;
    /* An earlier changed-signal observer may run before invalidation. Compare
     * the actual draft as well so that observer cannot read the previous mode. */
    if (selection == NULL || !selection->has_mode ||
        strcmp(selection->path, gtk_editable_get_text(GTK_EDITABLE(entry))) != 0)
        return UMI_STATUS_NOT_FOUND;
    *out_mode = selection->mode;
    return UMI_STATUS_OK;
}

/* Both editable steps can deliver user callbacks. Check that the original
 * entry and controls still share the captured mapped window before continuing. */
static gboolean SelectionWidgetsCurrent(GtkEntry *entry, GtkWidget *control, GtkWindow *window)
{
    return gtk_widget_get_mapped(control) && gtk_widget_get_mapped(GTK_WIDGET(entry)) &&
        gtk_widget_get_root(control) == GTK_ROOT(window) &&
        gtk_widget_get_root(GTK_WIDGET(entry)) == GTK_ROOT(window);
}

/* Publish text and its mode as one owner-thread action. A changed observer
 * may edit the text or retire either widget. A reentrant open may read BUSY
 * until both parts have settled, and cannot use an obsolete mode. */
static gboolean PublishSelection(GtkEntry *entry, GtkWidget *control, GtkWindow *window,
    const char *path, UmiPathPickerMode mode)
{
    PathSelection *selection = g_object_get_data(G_OBJECT(entry), "umicom-path-selection");
    if (selection != NULL && selection->publishing) return FALSE;
    if (selection == NULL) {
        selection = g_new0(PathSelection, 1);
        g_object_set_data_full(G_OBJECT(entry), "umicom-path-selection", selection, FreeSelection);
        g_signal_connect(entry, "changed", G_CALLBACK(SelectionEdited), selection);
        g_signal_connect(entry, "unmap", G_CALLBACK(SelectionUnmapped), selection);
    }
    selection->mode = mode;
    g_free(selection->path);
    selection->path = g_strdup(path);
    selection->has_mode = TRUE;
    selection->publishing = TRUE;
    /* The previous whole-text setter could insert the selected path after a
     * deletion observer had already entered a different draft. Keep it for
     * review and check the now-empty entry before making a separate insertion. */
#if 0
    gtk_editable_set_text(GTK_EDITABLE(entry), path);
#endif
    GtkEditable *editable = GTK_EDITABLE(entry);
    gtk_editable_delete_text(editable, 0, -1);
    if (selection->has_mode && SelectionWidgetsCurrent(entry, control, window) &&
        gtk_editable_get_text(editable)[0] == '\0') {
        int position = 0;
        gtk_editable_insert_text(editable, path, -1, &position);
    }
    selection->publishing = FALSE;
    selection->has_mode = selection->has_mode && SelectionWidgetsCurrent(entry, control, window) &&
        strcmp(gtk_editable_get_text(editable), path) == 0;
    return selection->has_mode;
}

static PathPicker *Picker(GtkWidget *control)
{
    return g_object_get_data(G_OBJECT(control), "umicom-path-picker");
}
static void Cancel(PathPicker *picker)
{
    /* Clear the identity first. Cancellation can schedule a completion, and
     * that old request must never clear or change a newer request's state. */
    PathPickRequest *request = picker->pending;
    picker->pending = NULL;
    if (request != NULL) g_cancellable_cancel(request->cancel);
}
static void Hidden(GtkWidget *control, gpointer unused)
{
    (void)unused;
    Cancel(Picker(control));
}
static void FreePicker(gpointer data)
{
    PathPicker *picker = data;
    Cancel(picker);
    g_weak_ref_clear(&picker->entry);
    g_free(picker->title);
    g_free(picker->suggestedName);
    g_free(picker);
}
static void FreeRequest(PathPickRequest *request)
{
    g_weak_ref_clear(&request->control);
    g_weak_ref_clear(&request->entry);
    g_weak_ref_clear(&request->window);
    g_object_unref(request->cancel);
    g_free(request->draft);
    g_free(request);
}
static gboolean Cancelled(const GError *error)
{
    return error != NULL &&
        (g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED) ||
         g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED) ||
         g_error_matches(error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED));
}
static void Selected(GObject *source, GAsyncResult *result, gpointer data)
{
    PathPickRequest *request = data;
    GError *error = NULL;
    GFile *file = NULL;
    if (request->mode == UMI_PATH_PICKER_EXISTING_FILE)
        file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(source), result, &error);
    else if (request->mode == UMI_PATH_PICKER_NEW_FILE)
        file = gtk_file_dialog_save_finish(GTK_FILE_DIALOG(source), result, &error);
    else
        file = gtk_file_dialog_select_folder_finish(GTK_FILE_DIALOG(source), result, &error);

    /* Complete the native request even after closure so its resources are
     * released. Resolve weak references only when delivering a usable result. */
    GtkWidget *control = g_weak_ref_get(&request->control);
    GtkEntry *entry = g_weak_ref_get(&request->entry);
    GtkWindow *window = g_weak_ref_get(&request->window);
    PathPicker *picker = control != NULL ? Picker(control) : NULL;
    char *path = file != NULL ? g_file_get_path(file) : NULL;
    if (picker != NULL && picker->pending == request) {
        picker->pending = NULL;
        if (entry != NULL && window != NULL && !g_cancellable_is_cancelled(request->cancel) &&
            gtk_widget_get_mapped(control) && gtk_widget_get_mapped(GTK_WIDGET(entry)) &&
            gtk_widget_get_root(control) == GTK_ROOT(window) &&
            gtk_widget_get_root(GTK_WIDGET(entry)) == GTK_ROOT(window)) {
            const char *message = "Selection cancelled; the path is unchanged.";
            gboolean publish = FALSE;
            if (strcmp(request->draft, gtk_editable_get_text(GTK_EDITABLE(entry))) != 0)
                message = "The path was edited while the chooser was open. Your newer text was kept.";
            else if (file != NULL) {
                int maximum = gtk_entry_get_max_length(entry);
                if (path == NULL || !g_utf8_validate(path, -1, NULL) || !g_path_is_absolute(path))
                    message = "Choose a local file or folder with an absolute path.";
                else if (maximum > 0 && g_utf8_strlen(path, -1) > maximum)
                    message = "The selected path is too long for this field. Its previous text was kept.";
                else {
                    message = "Path selected. Use the application's action to open or write it.";
                    publish = TRUE;
                }
            } else if (error != NULL && !Cancelled(error))
                message = "The chooser could not open. You can still type the full path.";
            /* Retain the label across entry-change callbacks: hosts may rebuild
             * their surface while receiving the new text. No host pointer is used. */
            GtkWidget *label = g_object_ref(picker->message);
            /* The old text-only publication remains for review. The adapter
             * now records how the current path was chosen and rechecks owner
             * and text after GTK notifications before exposing that mode. */
#if 0
            if (publish) gtk_editable_set_text(GTK_EDITABLE(entry), path);
#endif
            if (publish && !PublishSelection(entry, control, window, path, request->mode))
                message = "Selection interrupted; review the path before choosing an action.";
            gtk_label_set_text(GTK_LABEL(label), message);
            g_object_unref(label);
        }
    }
    g_free(path);
    g_clear_object(&file);
    g_clear_error(&error);
    g_clear_object(&entry);
    g_clear_object(&window);
    g_clear_object(&control);
    FreeRequest(request);
}
/* The shared chooser now retains its native owners and rechecks the captured request at every observable setup step. Retained for engineering review. */
#if 0
static void Browse(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *control = data;
    PathPicker *picker = Picker(control);
    if (picker->pending != NULL || !gtk_widget_get_mapped(control)) return;
    GtkEntry *entry = g_weak_ref_get(&picker->entry);
    GtkRoot *root = gtk_widget_get_root(control);
    if (entry == NULL || !GTK_IS_WINDOW(root) ||
        gtk_widget_get_root(GTK_WIDGET(entry)) != root ||
        !gtk_widget_get_mapped(GTK_WIDGET(entry))) {
        g_clear_object(&entry);
        return;
    }
    PathPickRequest *request = g_new0(PathPickRequest, 1);
    request->mode = picker->mode;
    request->draft = g_strdup(gtk_editable_get_text(GTK_EDITABLE(entry)));
    request->cancel = g_cancellable_new();
    g_weak_ref_init(&request->control, control);
    g_weak_ref_init(&request->entry, entry);
    g_weak_ref_init(&request->window, root);
    picker->pending = request;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_set_title(dialog, picker->title);
    gtk_file_dialog_set_modal(dialog, TRUE);
/* The shared location contract replaces inline chooser seeding. Retain the original implementation for compatibility review. */
#if 0
    /* An existing absolute draft seeds navigation; it is never opened here.
     * Relative input is left untouched until the user chooses a full path. */
    if (g_path_is_absolute(request->draft)) {
        GFile *initial = g_file_new_for_path(request->draft);
        gtk_file_dialog_set_initial_file(dialog, initial);
        g_object_unref(initial);
    } else if (request->mode == UMI_PATH_PICKER_NEW_FILE && picker->suggestedName != NULL)
        gtk_file_dialog_set_initial_name(dialog, picker->suggestedName);

#endif
    /* A relative entry is a draft, not permission to resolve the process's
     * working directory. The shared chooser policy uses explicit locations. */
    (void)UmiPathPickerGtk4SetInitialLocation(dialog, request->mode,
        g_path_is_absolute(request->draft) ? request->draft : NULL, NULL,
        picker->suggestedName);
    if (request->mode == UMI_PATH_PICKER_EXISTING_FILE)
        gtk_file_dialog_open(dialog, GTK_WINDOW(root), request->cancel, Selected, request);
    else if (request->mode == UMI_PATH_PICKER_NEW_FILE)
        gtk_file_dialog_save(dialog, GTK_WINDOW(root), request->cancel, Selected, request);
    else
        gtk_file_dialog_select_folder(dialog, GTK_WINDOW(root), request->cancel, Selected, request);
    g_object_unref(dialog);
    g_object_unref(entry);
}
#endif
/* Construction may notify code that closes, reparents or edits the host.
 * Strong local references protect memory; this predicate separately protects
 * the right to start a chooser for the captured field and unchanged draft. */
static gboolean RequestStillCurrent(GtkWidget *control, GtkEntry *entry,
    GtkWindow *window, const PathPickRequest *request)
{
    PathPicker *picker = Picker(control);
    return picker != NULL && picker->pending == request &&
        !g_cancellable_is_cancelled(request->cancel) &&
        gtk_widget_get_mapped(control) && gtk_widget_get_mapped(GTK_WIDGET(entry)) &&
        gtk_widget_get_root(control) == GTK_ROOT(window) &&
        gtk_widget_get_root(GTK_WIDGET(entry)) == GTK_ROOT(window) &&
        strcmp(request->draft, gtk_editable_get_text(GTK_EDITABLE(entry))) == 0;
}
static void Browse(GtkButton *button, gpointer data)
{
    (void)button;
    GtkWidget *control = data;
    PathPicker *picker = Picker(control);
    if (picker == NULL || picker->pending != NULL || !gtk_widget_get_mapped(control)) return;
    g_object_ref(control);
    GtkEntry *entry = g_weak_ref_get(&picker->entry);
    GtkRoot *root = gtk_widget_get_root(control);
    if (entry == NULL || !GTK_IS_WINDOW(root) ||
        gtk_widget_get_root(GTK_WIDGET(entry)) != root ||
        !gtk_widget_get_mapped(GTK_WIDGET(entry))) {
        g_clear_object(&entry); g_object_unref(control);
        return;
    }
    GtkWindow *window = g_object_ref(GTK_WINDOW(root));
    char *title = g_strdup(picker->title), *suggestedName = g_strdup(picker->suggestedName);
    PathPickRequest *request = g_new0(PathPickRequest, 1);
    request->mode = picker->mode;
    request->draft = g_strdup(gtk_editable_get_text(GTK_EDITABLE(entry)));
    request->cancel = g_cancellable_new();
    g_weak_ref_init(&request->control, control);
    g_weak_ref_init(&request->entry, entry);
    g_weak_ref_init(&request->window, window);
    picker->pending = request; /* Reserve before native construction can reenter. */
    GtkFileDialog *dialog = gtk_file_dialog_new();
    if (!RequestStillCurrent(control, entry, window, request)) goto abandoned;
    gtk_file_dialog_set_title(dialog, title);
    if (!RequestStillCurrent(control, entry, window, request)) goto abandoned;
    gtk_file_dialog_set_modal(dialog, TRUE);
    if (!RequestStillCurrent(control, entry, window, request)) goto abandoned;
    /* A relative entry remains a draft, not permission to resolve the process
     * directory. Seeding copies the chosen location without reading a file. */
    (void)UmiPathPickerGtk4SetInitialLocation(dialog, request->mode,
        g_path_is_absolute(request->draft) ? request->draft : NULL, NULL, suggestedName);
    if (!RequestStillCurrent(control, entry, window, request)) goto abandoned;
    /* Completion owns request from this point, even if a test/native backend
     * invokes it synchronously. Do not inspect request after dispatch returns. */
    if (request->mode == UMI_PATH_PICKER_EXISTING_FILE)
        gtk_file_dialog_open(dialog, window, request->cancel, Selected, request);
    else if (request->mode == UMI_PATH_PICKER_NEW_FILE)
        gtk_file_dialog_save(dialog, window, request->cancel, Selected, request);
    else
        gtk_file_dialog_select_folder(dialog, window, request->cancel, Selected, request);
    goto finished;
abandoned:
    if (Picker(control)->pending == request) Picker(control)->pending = NULL;
    g_cancellable_cancel(request->cancel);
    FreeRequest(request); /* No asynchronous operation was started. */
finished:
    g_object_unref(dialog);
    g_free(title); g_free(suggestedName);
    g_object_unref(window); g_object_unref(entry); g_object_unref(control);
}
GtkWidget *UmiPathPickerGtk4Create(GtkEntry *entry, UmiPathPickerMode mode,
    const char *title, const char *suggestedName)
{
    if (!GTK_IS_ENTRY(entry) || mode < UMI_PATH_PICKER_EXISTING_FILE || mode > UMI_PATH_PICKER_DIRECTORY ||
        title == NULL || title[0] == '\0' || !g_utf8_validate(title, -1, NULL) ||
        (suggestedName != NULL && (suggestedName[0] == '\0' ||
            !g_utf8_validate(suggestedName, -1, NULL) || strpbrk(suggestedName, "/\\") != NULL)))
        return NULL;
    PathPicker *picker = g_new0(PathPicker, 1);
    g_weak_ref_init(&picker->entry, entry);
    picker->mode = mode;
    picker->title = g_strdup(title);
    picker->suggestedName = g_strdup(suggestedName);
    GtkWidget *control = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    g_object_set_data_full(G_OBJECT(control), "umicom-path-picker", picker, FreePicker);
    GtkWidget *browse = gtk_button_new_with_label(mode == UMI_PATH_PICKER_EXISTING_FILE ?
        "Browse existing..." : mode == UMI_PATH_PICKER_NEW_FILE ? "Choose new location..." : "Choose folder...");
    gtk_widget_set_tooltip_text(browse, title);
    gtk_widget_set_halign(browse, GTK_ALIGN_START);
    gtk_widget_set_name(browse, "umicom-path-browse");
    gtk_box_append(GTK_BOX(control), browse);
    picker->message = gtk_label_new("Selecting a path does not open or write a file.");
    gtk_label_set_wrap(GTK_LABEL(picker->message), TRUE);
    gtk_label_set_xalign(GTK_LABEL(picker->message), 0.0F);
    gtk_widget_set_name(picker->message, "umicom-path-status");
    gtk_box_append(GTK_BOX(control), picker->message);
    /* Retained buttons have only a weak signal connection to their control.
     * A dialog owns its request, while that request owns no widget. */
    g_signal_connect_object(browse, "clicked", G_CALLBACK(Browse), control, 0);
    g_signal_connect(control, "unmap", G_CALLBACK(Hidden), NULL);
    return control;
}
