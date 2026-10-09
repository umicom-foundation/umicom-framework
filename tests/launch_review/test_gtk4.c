/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/launch_review/test_gtk4.c
 * PURPOSE: Exercise the actual settings controller with deterministic native-picker replies.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include <gtk/gtk.h>
#include "umicom/ui/gtk4/developer_dialog.h"
#include <stdio.h>
#include <string.h>

/* Substitute only the operating-system picker boundary. The real controller,
 * GTK controls, notification handlers and reference management remain active.
 * These fixtures never display an OS picker or execute a selected program. */
static GTask *pending;
static unsigned requests;
static int requestedFolder;
static void TestCancel(GCancellable *cancel, gpointer data)
{
    GTask *task = data;
    g_signal_handlers_disconnect_by_data(cancel, task);
    g_task_return_new_error(task, G_IO_ERROR, G_IO_ERROR_CANCELLED, "Fixture cancelled");
    g_clear_object(&pending);
}
static void TestStart(GtkFileDialog *picker, GtkWindow *parent, GCancellable *cancel,
                      GAsyncReadyCallback callback, gpointer context, int folder)
{
    (void)parent;
    ++requests;
    requestedFolder = folder;
    pending = g_task_new(picker, cancel, callback, context);
    g_signal_connect(cancel, "cancelled", G_CALLBACK(TestCancel), pending);
}
static void TestOpen(GtkFileDialog *picker, GtkWindow *parent, GCancellable *cancel,
                     GAsyncReadyCallback callback, gpointer context)
{
    TestStart(picker, parent, cancel, callback, context, 0);
}
static void TestFolder(GtkFileDialog *picker, GtkWindow *parent, GCancellable *cancel,
                       GAsyncReadyCallback callback, gpointer context)
{
    TestStart(picker, parent, cancel, callback, context, 1);
}
static GFile *TestFinish(GtkFileDialog *picker, GAsyncResult *result, GError **error)
{
    (void)picker;
    return g_task_propagate_pointer(G_TASK(result), error);
}
#define gtk_file_dialog_open TestOpen
#define gtk_file_dialog_select_folder TestFolder
#define gtk_file_dialog_open_finish TestFinish
#define gtk_file_dialog_select_folder_finish TestFinish
#define UmiGtk4NewProjectDialogCreate FixtureNewProjectCreate
#define UmiGtk4BuildSettingsDialogCreate FixtureBuildSettingsCreate
#define UmiGtk4DeveloperDialogDestroy FixtureDialogDestroy
/* The public header was read before the fixture renaming macros. Its include
 * guard correctly prevents a second declaration pass in the included source.
 * Declare the renamed cleanup function here through the same macro so calls
 * made before its definition have the exact public signature. This preserves
 * the real controller and the existing operating-system picker substitution. */
void UmiGtk4DeveloperDialogDestroy(UmiGtk4DeveloperDialog *dialog);
#include "../../adapters/gtk4/developer_dialog_gtk4.c"
#undef gtk_file_dialog_open
#undef gtk_file_dialog_select_folder
#undef gtk_file_dialog_open_finish
#undef gtk_file_dialog_select_folder_finish
#undef UmiGtk4NewProjectDialogCreate
#undef UmiGtk4BuildSettingsDialogCreate
#undef UmiGtk4DeveloperDialogDestroy
#define CHECK(test)                                                                                          \
    do                                                                                                       \
    {                                                                                                        \
        if (!(test))                                                                                         \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #test);                                       \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
#ifdef _WIN32
#define ROOT "C:/umicom-launch/project"
#define SELECTED "C:/umicom-launch/selected.exe"
#else
#define ROOT "/umicom-launch/project"
#define SELECTED "/umicom-launch/selected"
#endif
typedef struct Fixture
{
    UmiGtk4DeveloperDialog *dialog;
    GtkWidget *accept, *review, *entry;
    unsigned calls, notifications;
    const char *mode;
    UmiBuildProfile adopted;
} Fixture;
static void Dispose(Fixture *f)
{
    UmiGtk4DeveloperDialog *dialog = f->dialog;
    f->dialog = NULL;
    FixtureDialogDestroy(dialog);
}
static UmiStatus Accept(const UmiBuildProfile *profile, int trusted, void *context)
{
    (void)trusted;
    Fixture *f = context;
    ++f->calls;
    f->adopted = *profile;
    return UMI_STATUS_OK;
}
/* The rendered-child traversal omitted controls inside collapsed panels. Use the bounded Framework logical tree and preserve this earlier finder for review. */
#if 0
static GtkWidget *FindControl(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = FindControl(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
#endif
static GtkWidget *FindControl(GtkWidget *root, const char *id)
{
    return umi_gtk4_automation_find_tagged_widget(root, id);
}
static void Notify(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object;
    (void)property;
    Fixture *f = context;
    ++f->notifications;
    if (strcmp(f->mode, "notify-destroy") == 0)
        Dispose(f);
    else if (strcmp(f->mode, "notify-edit") == 0 && f->notifications == 1U)
        gtk_editable_set_text(GTK_EDITABLE(f->entry), "edited/program");
    else
    {
        g_signal_emit_by_name(f->accept, "clicked");
        g_signal_emit_by_name(f->review, "clicked");
    }
}
static void EntryChanged(GtkEditable *entry, gpointer context) { Notify(G_OBJECT(entry), NULL, context); }
static void Complete(const char *path, int remote)
{
    GTask *task = pending;
    pending = NULL;
    g_signal_handlers_disconnect_by_data(g_task_get_cancellable(task), task);
    if (path == NULL)
        g_task_return_new_error(task, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED, "Fixture dismissed");
    else
        g_task_return_pointer(task, remote ? g_file_new_for_uri(path) : g_file_new_for_path(path),
                              g_object_unref);
    g_object_unref(task);
}
static void Drain(Fixture *f)
{
    gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (f->dialog != NULL && f->dialog->launchChoosing && g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
}
int main(int argc, char **argv)
{
    int failed = 0;
    Fixture f = {0};
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *detail = NULL, *pick = NULL, *folder = NULL, *working = NULL, *arguments = NULL;
    gpointer rootWeak = NULL;
    char *expectedPath = NULL;
    CHECK(argc == 2);
    f.mode = argv[1];
    if (!gtk_init_check())
        return 77;
    GFile *expectedFile = g_file_new_for_path(SELECTED);
    expectedPath = g_file_get_path(expectedFile);
    g_object_unref(expectedFile);
    CHECK(expectedPath != NULL);
    UmiBuildProfile profile;
    umi_build_profile_init(&profile);
    strcpy(profile.source_directory, ROOT);
    strcpy(profile.run_program, "bin/notes");
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    CHECK(FixtureBuildSettingsCreate(parent, &profile, 0, Accept, &f, &f.dialog) == UMI_STATUS_OK);
    form = g_object_ref(f.dialog->window);
    rootWeak = f.dialog->root;
    g_object_add_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
    GtkWidget **controls[] = {&f.accept, &f.review, &f.entry, &detail, &pick, &folder, &working, &arguments};
    const char *ids[] = {"developer.dialog.accept",       "developer.launch.review",
                         "developer.build.field.8",       "developer.launch.detail",
                         "developer.launch.program",      "developer.launch.folder",
                         "developer.build.run-directory", "developer.build.arguments"};
    for (size_t i = 0U; i < 8U; ++i)
    {
        *controls[i] = FindControl(GTK_WIDGET(form), ids[i]);
        CHECK(*controls[i] != NULL);
        g_object_ref(*controls[i]);
    }
    CHECK(requests == 0U && f.calls == 0U);
    if (strncmp(f.mode, "review-", 7U) == 0 || strncmp(f.mode, "notify-", 7U) == 0)
    {
        if (strcmp(f.mode, "review-bare") == 0)
            gtk_editable_set_text(GTK_EDITABLE(f.entry), "notes");
        if (strcmp(f.mode, "review-invalid") == 0)
            gtk_editable_set_text(GTK_EDITABLE(arguments), "\"bad");
        if (strncmp(f.mode, "notify-", 7U) == 0)
            g_signal_connect(detail, "notify::label", G_CALLBACK(Notify), &f);
        g_signal_emit_by_name(f.review, "clicked");
        CHECK(f.calls == 0U && requests == 0U);
        if (strcmp(f.mode, "notify-destroy") == 0)
            CHECK(f.dialog == NULL);
        else if (strcmp(f.mode, "notify-edit") == 0)
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "changed during review") != NULL);
        else if (strcmp(f.mode, "review-invalid") == 0)
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "Cannot describe") != NULL);
        else
        {
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "Native Debug program:") != NULL);
            if (strcmp(f.mode, "review-bare") == 0)
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "operating-system program lookup") !=
                      NULL);
            if (strcmp(f.mode, "review-invalidate") == 0)
            {
                gtk_editable_set_text(GTK_EDITABLE(working), "sample data");
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "inputs changed") != NULL);
            }
        }
        goto cleanup;
    }
    CHECK(strcmp(f.mode, "program") == 0 || strcmp(f.mode, "folder") == 0 ||
          strcmp(f.mode, "pending-destroy") == 0 || strcmp(f.mode, "pending-hide") == 0 ||
          strcmp(f.mode, "pending-parent") == 0 || strcmp(f.mode, "stale") == 0 ||
          strcmp(f.mode, "entry-reentry") == 0 || strcmp(f.mode, "cancel") == 0 ||
          strcmp(f.mode, "remote") == 0);
    GtkWidget *choose = strcmp(f.mode, "folder") == 0 ? folder : pick;
    g_signal_emit_by_name(choose, "clicked");
    CHECK(requests == 1U && pending != NULL);
    CHECK(requestedFolder == (strcmp(f.mode, "folder") == 0));
    g_signal_emit_by_name(choose, "clicked");
    g_signal_emit_by_name(f.accept, "clicked");
    CHECK(requests == 1U && f.calls == 0U);
    if (strcmp(f.mode, "pending-destroy") == 0)
    {
        Dispose(&f);
        goto cleanup;
    }
    if (strcmp(f.mode, "pending-hide") == 0)
    {
        gtk_widget_set_visible(GTK_WIDGET(form), FALSE);
        goto cleanup;
    }
    if (strcmp(f.mode, "pending-parent") == 0)
    {
        gtk_window_destroy(parent);
        goto cleanup;
    }
    if (strcmp(f.mode, "stale") == 0)
        gtk_editable_set_text(GTK_EDITABLE(f.entry), "edited/program");
    if (strcmp(f.mode, "entry-reentry") == 0)
        g_signal_connect(f.entry, "changed", G_CALLBACK(EntryChanged), &f);
    Complete(strcmp(f.mode, "cancel") == 0   ? NULL
             : strcmp(f.mode, "remote") == 0 ? "https://example.invalid/program"
                                             : SELECTED,
             strcmp(f.mode, "remote") == 0);
    Drain(&f);
    CHECK(f.dialog != NULL && !f.dialog->launchChoosing && f.calls == 0U);
    if (strcmp(f.mode, "stale") == 0)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(f.entry)), "edited/program") == 0);
    else if (strcmp(f.mode, "cancel") == 0 || strcmp(f.mode, "remote") == 0)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(f.entry)), "bin/notes") == 0);
    else
    {
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(strcmp(f.mode, "folder") == 0 ? working : f.entry)),
                     expectedPath) == 0);
        if (strcmp(f.mode, "entry-reentry") == 0)
            CHECK(f.notifications != 0U);
        else
        {
            g_signal_emit_by_name(f.accept, "clicked");
            CHECK(f.calls == 1U);
            UmiBuildProfile expected = profile;
            strcpy(strcmp(f.mode, "folder") == 0 ? expected.run_working_directory : expected.run_program,
                   expectedPath);
            CHECK(umi_build_profile_equal(&expected, &f.adopted));
        }
    }
cleanup:
    if (detail != NULL)
        g_signal_handlers_disconnect_by_data(detail, &f);
    if (f.entry != NULL)
        g_signal_handlers_disconnect_by_data(f.entry, &f);
    Dispose(&f);
    gint64 deadline = g_get_monotonic_time() + 5 * G_TIME_SPAN_SECOND;
    while (rootWeak != NULL && g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    if (rootWeak != NULL)
    {
        g_object_remove_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
        failed = 1;
    }
    if (pending != NULL)
    {
        Complete(NULL, 0);
        failed = 1;
    }
    if (f.mode != NULL && strcmp(f.mode, "program") != 0 && strcmp(f.mode, "folder") != 0 && f.calls != 0U)
        failed = 1;
    unsigned oldCalls = f.calls, oldRequests = requests;
    if (pick != NULL)
        g_signal_emit_by_name(pick, "clicked");
    if (f.review != NULL)
        g_signal_emit_by_name(f.review, "clicked");
    if (f.calls != oldCalls || requests != oldRequests)
        failed = 1;
    GtkWidget *retained[] = {f.accept, f.review, f.entry, detail, pick, folder, working, arguments};
    for (size_t i = 0U; i < 8U; ++i)
        if (retained[i] != NULL)
            g_object_unref(retained[i]);
    if (form != NULL)
        g_object_unref(form);
    if (parent != NULL)
    {
        gtk_window_destroy(parent);
        g_object_unref(parent);
    }
    g_free(expectedPath);
    return failed;
}
