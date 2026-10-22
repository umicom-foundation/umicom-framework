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
#include "../test_runtime/path_identity.h"

/* Replace only the native-dialog boundary. Real GTK widgets still exercise
 * weak references, unmapping, editable limits and the asynchronous completion.
 * No application file, database, broker session or model is opened. */
static GTask *pendingTask;
static GAsyncReadyCallback pendingCallback;
static gpointer pendingContext;
static unsigned starts;
/* Inject native setup notifications without displaying a system chooser.
 * The real dialog properties and real widgets retain their normal ownership. */
typedef struct SetupProbe {
    GtkWidget *window, *control, *entry, *button;
    const char *scenario;
    unsigned calls;
} SetupProbe;
static SetupProbe *setupProbe;
static int completeInline;
static void Complete(GFile *file);
static void SetupAction(void)
{
    SetupProbe *probe = setupProbe;
    if (probe == NULL || probe->calls != 0U) return;
    ++probe->calls;
    if (strstr(probe->scenario, "reenter") != NULL)
        g_signal_emit_by_name(probe->button, "clicked");
    else if (strstr(probe->scenario, "edit") != NULL)
        gtk_editable_set_text(GTK_EDITABLE(probe->entry), "newer draft");
    else if (strstr(probe->scenario, "hide") != NULL)
        gtk_widget_set_visible(probe->control, FALSE);
    else gtk_window_destroy(GTK_WINDOW(probe->window));
}
static void SetupNotify(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; (void)data;
    SetupAction();
}
static GtkFileDialog *FakeDialogNew(void)
{
    GtkFileDialog *dialog = gtk_file_dialog_new();
    if (setupProbe != NULL) {
        if (strstr(setupProbe->scenario, "construct-") != NULL) SetupAction();
        else if (strstr(setupProbe->scenario, "title-") != NULL)
            g_signal_connect(dialog, "notify::title", G_CALLBACK(SetupNotify), NULL);
        else if (strstr(setupProbe->scenario, "seed-") != NULL)
            g_signal_connect(dialog, "notify::initial-name", G_CALLBACK(SetupNotify), NULL);
    }
    return dialog;
}
static void FakeStart(GtkFileDialog *dialog, GtkWindow *parent, GCancellable *cancel,
    GAsyncReadyCallback callback, gpointer context)
{
    (void)parent;
    ++starts;
    pendingTask = g_task_new(dialog, cancel, NULL, NULL);
    g_task_set_check_cancellable(pendingTask, FALSE);
    pendingCallback = callback;
    pendingContext = context;
    if (completeInline) Complete(NULL);
}
static GFile *FakeFinish(GtkFileDialog *dialog, GAsyncResult *result, GError **error)
{
    (void)dialog;
    return g_task_propagate_pointer(G_TASK(result), error);
}
#define gtk_file_dialog_new FakeDialogNew
#define gtk_file_dialog_open FakeStart
#define gtk_file_dialog_save FakeStart
#define gtk_file_dialog_select_folder FakeStart
#define gtk_file_dialog_open_finish FakeFinish
#define gtk_file_dialog_save_finish FakeFinish
#define gtk_file_dialog_select_folder_finish FakeFinish
#include "../../adapters/gtk4/path_picker_gtk4.c"
#undef gtk_file_dialog_new
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
/* Inspect configured locations without displaying a native dialog or creating
 * a file. A selected root must not be replaced by the process directory. */
static int CheckInitialLocation(const char *scenario)
{
    char *cwd = g_get_current_dir();
    char *folder = g_build_filename(cwd, "chosen project", NULL);
    char *selectedPath = g_build_filename(cwd, "selected source.c", NULL);
    GFile *folderFile = g_file_new_for_path(folder);
    GFile *selectedFile = g_file_new_for_path(selectedPath);
    GtkFileDialog *dialog = gtk_file_dialog_new();
    CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
        NULL, folder, "notes caf\xc3\xa9.c") == UMI_STATUS_OK);
    CHECK(g_file_equal(gtk_file_dialog_get_initial_folder(dialog), folderFile));
    CHECK(strcmp(gtk_file_dialog_get_initial_name(dialog), "notes caf\xc3\xa9.c") == 0);
    if (strcmp(scenario, "seed-file") == 0) {
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            selectedPath, folder, "unused.c") == UMI_STATUS_OK);
        CHECK(g_file_equal(gtk_file_dialog_get_initial_file(dialog), selectedFile));
        /* Re-seeding the same file must remain observable and replace both fields. */
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            selectedPath, folder, "ignored.c") == UMI_STATUS_OK);
        CHECK(g_file_equal(gtk_file_dialog_get_initial_file(dialog), selectedFile));
        GFile *parent = g_file_get_parent(selectedFile);
        char *basename = g_file_get_basename(selectedFile);
        CHECK(g_file_equal(gtk_file_dialog_get_initial_folder(dialog), parent));
        CHECK(g_strcmp0(gtk_file_dialog_get_initial_name(dialog), basename) == 0);
        g_object_unref(parent); g_free(basename);
    } else if (strcmp(scenario, "seed-clear") == 0) {
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_EXISTING_FILE,
            "", "", "") == UMI_STATUS_OK);
        CHECK(gtk_file_dialog_get_initial_file(dialog) == NULL);
        CHECK(gtk_file_dialog_get_initial_folder(dialog) == NULL);
        CHECK(gtk_file_dialog_get_initial_name(dialog) == NULL);
    } else if (strcmp(scenario, "seed-invalid") == 0) {
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            "relative.c", folder, "notes.c") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            NULL, "relative-folder", "notes.c") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            NULL, folder, "../notes.c") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            NULL, folder, "\xff") == UMI_STATUS_INVALID_ARGUMENT);
#ifdef _WIN32
        /* A rooted path without a drive still depends on the process drive. */
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            NULL, "/chosen", "notes.c") == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_NEW_FILE,
            NULL, "C:chosen", "notes.c") == UMI_STATUS_INVALID_ARGUMENT);
#endif
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, (UmiPathPickerMode)99,
            NULL, NULL, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(g_file_equal(gtk_file_dialog_get_initial_folder(dialog), folderFile));
        CHECK(strcmp(gtk_file_dialog_get_initial_name(dialog), "notes caf\xc3\xa9.c") == 0);
    } else if (strcmp(scenario, "seed-folder") == 0) {
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_DIRECTORY,
            NULL, folder, NULL) == UMI_STATUS_OK);
        CHECK(g_file_equal(gtk_file_dialog_get_initial_folder(dialog), folderFile));
        CHECK(gtk_file_dialog_get_initial_name(dialog) == NULL);
        CHECK(UmiPathPickerGtk4SetInitialLocation(dialog, UMI_PATH_PICKER_DIRECTORY,
            folder, cwd, "ignored.c") == UMI_STATUS_OK);
        CHECK(g_file_equal(gtk_file_dialog_get_initial_folder(dialog), folderFile));
        CHECK(gtk_file_dialog_get_initial_name(dialog) == NULL);
    } else return 2;
    g_object_unref(dialog);
    g_object_unref(folderFile);
    g_object_unref(selectedFile);
    g_free(cwd);
    g_free(folder);
    g_free(selectedPath);
    return 0;
}

static int CheckSetup(const char *scenario)
{
    GtkWidget *entry = g_object_ref_sink(gtk_entry_new());
    GtkWidget *control = g_object_ref_sink(UmiPathPickerGtk4Create(GTK_ENTRY(entry),
        UMI_PATH_PICKER_NEW_FILE, "Choose a path", "report.csv"));
    GtkWidget *window = g_object_ref_sink(gtk_window_new());
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
    gtk_box_append(GTK_BOX(box), entry); gtk_box_append(GTK_BOX(box), control);
    gtk_window_set_child(GTK_WINDOW(window), box);
    gtk_window_present(GTK_WINDOW(window)); Drain();
    CHECK(gtk_widget_get_mapped(control));
    gtk_editable_set_text(GTK_EDITABLE(entry), "original draft");
    GtkWidget *button = g_object_ref(gtk_widget_get_first_child(control));
    SetupProbe probe = {window, control, entry, button, scenario, 0U};
    setupProbe = &probe;
    completeInline = strcmp(scenario, "setup-inline-complete") == 0;
    g_signal_emit_by_name(button, "clicked");
    setupProbe = NULL; completeInline = 0;
    if (strstr(scenario, "reenter") != NULL) {
        CHECK(probe.calls == 1U && starts == 1U && pendingTask != NULL);
        Complete(NULL);
    } else if (strcmp(scenario, "setup-inline-complete") == 0) {
        CHECK(starts == 1U && pendingTask == NULL);
    } else {
        CHECK(probe.calls == 1U && starts == 0U && pendingTask == NULL);
    }
    CHECK(Picker(control)->pending == NULL);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)),
        strstr(scenario, "edit") != NULL ? "newer draft" : "original draft") == 0);
    gtk_window_destroy(GTK_WINDOW(window)); g_object_unref(window);
    g_object_unref(control); g_object_unref(button); g_object_unref(entry); Drain();
    return 0;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    if (!gtk_init_check()) return 77;
    const char *scenario = argv[1];
    if (strncmp(scenario, "setup-", 6U) == 0) return CheckSetup(scenario);
    if (strncmp(scenario, "seed-", 5U) == 0) return CheckInitialLocation(scenario);
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
        /* Retained spelling comparison; native canonicalisation may change separators without changing the selected file. */
#if 0
CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), unchanged ? "original draft" : path) == 0);
#endif
CHECK(unchanged ? strcmp(gtk_editable_get_text(GTK_EDITABLE(entry)), "original draft") == 0 :
            UmiTestSameLocalPath(gtk_editable_get_text(GTK_EDITABLE(entry)), path));
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
