/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/installed_files/test_gtk4.c
 * PURPOSE: Exercise installed-file selection and retained native controls without launching a program.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/developer_dialog.h"
#include <glib/gstdio.h>
#include "fixture.h"
#define CHECK(expression)                                                                                    \
    do                                                                                                       \
    {                                                                                                        \
        if (!(expression))                                                                                   \
        {                                                                                                    \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #expression);                                 \
            failed = 1;                                                                                      \
            goto cleanup;                                                                                    \
        }                                                                                                    \
    } while (0)
typedef struct Probe
{
    UmiGtk4DeveloperDialog *dialog;
    GtkWidget *read, *use, *accept, *picker;
    unsigned calls, notifications;
    UmiBuildProfile profile;
} Probe;
static GtkWidget *Find(GtkWidget *root, const char *id)
{
    const char *tag = g_object_get_data(G_OBJECT(root), "umicom-automation-id");
    if (tag != NULL && strcmp(tag, id) == 0)
        return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
         child = gtk_widget_get_next_sibling(child))
    {
        GtkWidget *found = Find(child, id);
        if (found != NULL)
            return found;
    }
    return NULL;
}
static GtkWindow *Form(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i)
    {
        GtkWindow *window = g_list_model_get_item(windows, i);
        if (gtk_window_get_transient_for(window) == parent && gtk_widget_get_visible(GTK_WIDGET(window)))
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static void Dispose(Probe *probe)
{
    UmiGtk4DeveloperDialog *dialog = probe->dialog;
    probe->dialog = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog);
}
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    (void)trusted;
    Probe *probe = context;
    probe->profile = *profile;
    ++probe->calls;
    return UMI_STATUS_OK;
}
/* Drain the UI context with a deadline. The result must be observable through
 * the form; a delay by itself is never evidence that a worker completed. */
static int WaitReady(GtkWidget *read)
{
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (!gtk_widget_get_sensitive(read) && g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    return gtk_widget_get_sensitive(read);
}
static void DestroyOnModel(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    if (gtk_drop_down_get_model(GTK_DROP_DOWN(object)) != NULL)
        Dispose(context);
}
static void Reenter(GObject *object, GParamSpec *property, gpointer context)
{
    (void)object;
    (void)property;
    Probe *probe = context;
    ++probe->notifications;
    g_signal_emit_by_name(probe->read, "clicked");
    g_signal_emit_by_name(probe->use, "clicked");
    g_signal_emit_by_name(probe->accept, "clicked");
}
static void ReenterEntry(GtkEditable *editable, gpointer context)
{
    Reenter(G_OBJECT(editable), NULL, context);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!gtk_init_check())
        return 77;
    const char *name = argv[1];
    int failed = 0, retired = 0;
    Probe probe = {0};
    InstalledFixture fixture;
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *program = NULL, *build = NULL, *detail = NULL;
    gpointer rootWeak = NULL;
    char *scratch = g_dir_make_tmp("umicom-installed-XXXXXX", NULL);
    CHECK(scratch != NULL);
    CHECK(InstalledFixtureCreate(scratch, &fixture) == UMI_STATUS_OK);
    probe.profile = fixture.profile;
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &probe.profile, 0, Applied, &probe, &probe.dialog) ==
          UMI_STATUS_OK);
    form = Form(parent);
    CHECK(form != NULL);
    rootWeak = gtk_window_get_child(form);
    g_object_add_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
    const char *ids[] = {"developer.installed.read",  "developer.installed.program",
                         "developer.dialog.accept",   "developer.installed.choices",
                         "developer.build.field.8",   "developer.build.field.2",
                         "developer.installed.detail"};
    GtkWidget **controls[] = {&probe.read, &probe.use, &probe.accept, &probe.picker,
                              &program,    &build,     &detail};
    for (size_t i = 0U; i < sizeof(ids) / sizeof(ids[0]); ++i)
    {
        *controls[i] = Find(GTK_WIDGET(form), ids[i]);
        CHECK(*controls[i] != NULL);
        g_object_ref(*controls[i]);
    }
    CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL && probe.calls == 0U);
    if (strcmp(name, "no-auto-read") == 0)
        goto cleanup;
    if (strcmp(name, "model-destroy") == 0)
        g_signal_connect(probe.picker, "notify::model", G_CALLBACK(DestroyOnModel), &probe);
    if (strcmp(name, "reentrant-model") == 0)
        g_signal_connect(probe.picker, "notify::model", G_CALLBACK(Reenter), &probe);
    g_signal_emit_by_name(probe.read, "clicked");
    if (strcmp(name, "pending-destroy") == 0)
    {
        Dispose(&probe);
        goto cleanup;
    }
    if (strcmp(name, "pending-hide") == 0)
    {
        gtk_widget_set_visible(GTK_WIDGET(form), FALSE);
        goto cleanup;
    }
    if (strcmp(name, "changed-input") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(build), "different");
        gtk_editable_set_text(GTK_EDITABLE(build), "build");
    }
    if (strcmp(name, "model-destroy") == 0)
    {
        gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
        while (probe.dialog != NULL && g_get_monotonic_time() < deadline)
        {
            g_main_context_iteration(NULL, FALSE);
            g_usleep(1000U);
        }
        CHECK(probe.dialog == NULL);
        goto cleanup;
    }
    CHECK(WaitReady(probe.read));
    CHECK(probe.calls == 0U);
    if (strcmp(name, "changed-input") == 0)
    {
        g_signal_emit_by_name(probe.use, "clicked");
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "previous-program") == 0);
        goto cleanup;
    }
    GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker));
    CHECK(model != NULL && g_list_model_get_n_items(model) == 2U);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(probe.picker), strcmp(name, "data-selection") == 0 ? 1U : 0U);
    if (strcmp(name, "read") == 0)
        goto cleanup;
    if (strcmp(name, "reentrant-entry") == 0)
        g_signal_connect(program, "changed", G_CALLBACK(ReenterEntry), &probe);
    if (strcmp(name, "removed-file") == 0)
        CHECK(umi_fs_remove_tree(fixture.program) == UMI_STATUS_OK);
    g_signal_emit_by_name(probe.use, "clicked");
    if (strcmp(name, "selection-edit") == 0)
        gtk_editable_set_text(GTK_EDITABLE(program), "user-edited-program");
    CHECK(WaitReady(probe.read));
    CHECK(probe.calls == 0U);
    if (strcmp(name, "selection-edit") == 0)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "user-edited-program") == 0);
    else if (strcmp(name, "removed-file") == 0 || strcmp(name, "data-selection") == 0)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "previous-program") == 0);
    else
        CHECK(umi_path_equal(gtk_editable_get_text(GTK_EDITABLE(program)), fixture.program));
    if (strcmp(name, "apply") == 0)
    {
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 1U && umi_path_equal(probe.profile.run_program, fixture.program));
    }
cleanup:
    if (probe.picker != NULL)
        g_signal_handlers_disconnect_by_data(probe.picker, &probe);
    if (program != NULL)
        g_signal_handlers_disconnect_by_data(program, &probe);
    Dispose(&probe);
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (rootWeak != NULL && g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    retired = rootWeak == NULL;
    if (!retired)
    {
        g_object_remove_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
        failed = 1;
    }
    unsigned accepted = probe.calls;
    if (probe.read != NULL)
        g_signal_emit_by_name(probe.read, "clicked");
    if (probe.use != NULL)
        g_signal_emit_by_name(probe.use, "clicked");
    if (probe.calls != accepted)
        failed = 1;
    GtkWidget *retained[] = {probe.read, probe.use, probe.accept, probe.picker, program, build, detail};
    for (size_t i = 0U; i < sizeof(retained) / sizeof(retained[0]); ++i)
        if (retained[i] != NULL)
            g_object_unref(retained[i]);
    if (form != NULL)
        g_object_unref(form);
    if (parent != NULL)
    {
        gtk_window_destroy(parent);
        g_object_unref(parent);
    }
    if (scratch != NULL && retired && umi_fs_remove_tree(scratch) != UMI_STATUS_OK)
        failed = 1;
    g_free(scratch);
    return failed;
}
