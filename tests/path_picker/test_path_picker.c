/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/path_picker/test_path_picker.c
 * PURPOSE: Exercise native chooser completion without opening an operating-system file dialog.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/path_picker.h"
#include <stdio.h>
#include <string.h>

/* Replace only the native-dialog boundary. Real GTK widgets still exercise
 * weak references, unmapping, editable limits and the asynchronous completion.
 * No application file, database, broker session or model is opened. */
static GTask *pendingTask;
static GAsyncReadyCallback pendingCallback;
static gpointer pendingContext;
static unsigned starts;
static void FakeStart(GtkFileDialog *dialog, GtkWindow *parent, GCancellable *cancel,
    GAsyncReadyCallback callback, gpointer context)
{
    (void)parent;
    ++starts;
    pendingTask = g_task_new(dialog, cancel, NULL, NULL);
    g_task_set_check_cancellable(pendingTask, FALSE);
    pendingCallback = callback;
    pendingContext = context;
}
static GFile *FakeFinish(GtkFileDialog *dialog, GAsyncResult *result, GError **error)
{
    (void)dialog;
    return g_task_propagate_pointer(G_TASK(result), error);
}
#define gtk_file_dialog_open FakeStart
#define gtk_file_dialog_save FakeStart
#define gtk_file_dialog_select_folder FakeStart
#define gtk_file_dialog_open_finish FakeFinish
#define gtk_file_dialog_save_finish FakeFinish
#define gtk_file_dialog_select_folder_finish FakeFinish
#include "../../adapters/gtk4/path_picker_gtk4.c"
#undef gtk_file_dialog_open
#undef gtk_file_dialog_save
#undef gtk_file_dialog_select_folder
#undef gtk_file_dialog_open_finish
#undef gtk_file_dialog_save_finish
#undef gtk_file_dialog_select_folder_finish

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "line %d: %s\n", __LINE__, #expression); return 1; } } while (0)

static void Complete(GFile *file)
{
    GTask *task = pendingTask;
    GAsyncReadyCallback callback = pendingCallback;
    gpointer context = pendingContext;
    pendingTask = NULL;
    pendingCallback = NULL;
    pendingContext = NULL;
    if (file != NULL) g_task_return_pointer(task, file, g_object_unref);
    else g_task_return_new_error(task, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED, "Dismissed");
    callback(g_task_get_source_object(task), G_ASYNC_RESULT(task), context);
    g_object_unref(task);
}
static void Drain(void)
{
    while (g_main_context_iteration(NULL, FALSE)) {}
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    const char *scenario = argv[1];
    GtkWidget *entry = gtk_entry_new();
    g_object_ref_sink(entry);
    CHECK(UmiPathPickerGtk4Create(NULL, UMI_PATH_PICKER_NEW_FILE, "Choose", NULL) == NULL);
    CHECK(UmiPathPickerGtk4Create(GTK_ENTRY(entry), (UmiPathPickerMode)9, "Choose", NULL) == NULL);
    CHECK(UmiPathPickerGtk4Create(GTK_ENTRY(entry), UMI_PATH_PICKER_NEW_FILE, "", NULL) == NULL);
    CHECK(UmiPathPickerGtk4Create(GTK_ENTRY(entry), UMI_PATH_PICKER_NEW_FILE, "Choose", "../file") == NULL);
    UmiPathPickerMode mode = strcmp(scenario, "open") == 0 ? UMI_PATH_PICKER_EXISTING_FILE :
        strcmp(scenario, "folder") == 0 ? UMI_PATH_PICKER_DIRECTORY : UMI_PATH_PICKER_NEW_FILE;
    GtkWidget *control = UmiPathPickerGtk4Create(GTK_ENTRY(entry), mode, "Choose a path", "report.csv");
    CHECK(control != NULL);
    g_object_ref_sink(control);
    GtkWidget *button = gtk_widget_get_first_child(control);
    g_object_ref(button);
    GtkWidget *window = gtk_window_new();
    g_object_ref_sink(window);
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_append(GTK_BOX(box), entry);
    gtk_box_append(GTK_BOX(box), control);
    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_window_present(GTK_WINDOW(window));
    Drain();
    CHECK(gtk_widget_get_mapped(control));
    gtk_editable_set_text(GTK_EDITABLE(entry), "original draft");
    g_signal_emit_by_name(button, "clicked");
    CHECK(starts == 1U && pendingTask != NULL);
    g_signal_emit_by_name(button, "clicked");
    CHECK(starts == 1U);
    char *path = g_build_filename(g_get_tmp_dir(), "umicom-path-caf\xc3\xa9.csv", NULL);
    gboolean unchanged = FALSE;
    if (strcmp(scenario, "edited") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(entry), "newer draft");
        Complete(g_file_new_for_path(path));
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "newer draft") == 0);
    } else {
        if (strcmp(scenario, "long") == 0) {
            gtk_entry_set_max_length(GTK_ENTRY(entry), 14);
            unchanged = TRUE;
        } else if (strcmp(scenario, "close") == 0) {
            gtk_window_destroy(GTK_WINDOW(window));
            unchanged = TRUE;
        } else if (strcmp(scenario, "hidden") == 0) {
            gtk_widget_set_visible(control, FALSE);
            unchanged = TRUE;
        } else if (strcmp(scenario, "remote") == 0 || strcmp(scenario, "cancel") == 0)
            unchanged = TRUE;
        if (strcmp(scenario, "remote") == 0) Complete(g_file_new_for_uri("https://example.invalid/report.csv"));
        else if (strcmp(scenario, "cancel") == 0) Complete(NULL);
        else Complete(g_file_new_for_path(path));
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), unchanged ? "original draft" : path) == 0);
    }
    CHECK(Picker(control)->pending == NULL);
    gtk_window_destroy(GTK_WINDOW(window));
    g_object_unref(window);
    g_object_unref(control);
    /* The retained button can outlive its container. It must have no remaining
     * connection that launches a chooser or dereferences the former entry. */
    g_signal_emit_by_name(button, "clicked");
    CHECK(starts == 1U);
    g_object_unref(button);
    g_object_unref(entry);
    g_free(path);
    Drain();
    return 0;
}
