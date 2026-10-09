/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_tool_catalogue_gtk4.c
 * PURPOSE: Exercise tool inventory publication, input changes and dialog ownership.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/platform/filesystem.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/developer_dialog.h"
#include <stdio.h>
#include <string.h>

#define CHECK(value)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (!(value))                                                                              \
        {                                                                                          \
            fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value);                            \
            failed = 1;                                                                            \
            goto cleanup;                                                                          \
        }                                                                                          \
    } while (0)
typedef struct ToolProbe
{
    UmiGtk4DeveloperDialog *dialog;
    GtkWidget *folder, *inspect;
    const char *mode, *initialFolder;
    unsigned applied, notifications;
} ToolProbe;
static void Dispose(ToolProbe *probe)
{
    UmiGtk4DeveloperDialog *dialog = probe->dialog;
    probe->dialog = NULL;
    UmiGtk4DeveloperDialogDestroy(dialog);
}
static UmiStatus Applied(const UmiBuildProfile *profile, int trusted, void *context)
{
    (void)profile;
    (void)trusted;
    ++((ToolProbe *)context)->applied;
    return UMI_STATUS_OK;
}
static GtkWindow *Form(GtkWindow *parent)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index)
    {
        GtkWindow *window = g_list_model_get_item(windows, index);
        if (gtk_window_get_transient_for(window) == parent &&
            gtk_widget_get_visible(GTK_WIDGET(window)))
            return window;
        g_object_unref(window);
    }
    return NULL;
}
static void Publication(GObject *object, GParamSpec *property, gpointer context)
{
    (void)property;
    ToolProbe *probe = context;
    const char *text = gtk_label_get_text(GTK_LABEL(object));
    if (strstr(text, "Captured tool files.") == NULL || probe->notifications != 0U)
        return;
    ++probe->notifications;
    /* Extensions can react synchronously to a label change. Closing or editing
     * at this exact point must invalidate the capture without using freed state. */
    if (strcmp(probe->mode, "notify-close") == 0)
        Dispose(probe);
    else
    {
        gtk_editable_set_text(GTK_EDITABLE(probe->folder), "relative/tools");
        if (strcmp(probe->mode, "notify-edit-back") == 0)
            gtk_editable_set_text(GTK_EDITABLE(probe->folder), probe->initialFolder);
        g_signal_emit_by_name(probe->inspect, "clicked");
    }
}
static int Released(GWeakRef *reference)
{
    GObject *object = g_weak_ref_get(reference);
    if (object == NULL)
        return 1;
    g_object_unref(object);
    return 0;
}
int main(int argc, char **argv)
{
    int failed = 0, controlsRetained = 0;
    ToolProbe probe = {0};
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *detail = NULL, *root = NULL;
    char *scratch = NULL;
    GWeakRef rootReference;
    g_weak_ref_init(&rootReference, NULL);
    CHECK(argc == 2);
    probe.mode = argv[1];
    if (!gtk_init_check())
    {
        g_weak_ref_clear(&rootReference);
        return 77;
    }
    scratch = g_dir_make_tmp("umicom-tool-form-XXXXXX", NULL);
    CHECK(scratch != NULL);
    probe.initialFolder = scratch;
    UmiBuildProfile profile, original;
    CHECK(umi_build_profile_set(&profile, "tools", scratch, "build") == UMI_STATUS_OK);
    CHECK(strlen(scratch) < sizeof profile.tool_directory);
    strcpy(profile.tool_directory, scratch);
    original = profile;
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    gtk_window_present(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &profile, 0, Applied, &probe, &probe.dialog) ==
          UMI_STATUS_OK);
    form = Form(parent);
    CHECK(form != NULL);
    root = gtk_window_get_child(form);
    g_weak_ref_set(&rootReference, root);
    probe.inspect = umi_gtk4_automation_find_tagged_widget(root, "developer.tools.inspect");
    probe.folder = umi_gtk4_automation_find_tagged_widget(root, "developer.build.field.11");
    detail = umi_gtk4_automation_find_tagged_widget(root, "developer.tools.detail");
    CHECK(GTK_IS_BUTTON(probe.inspect) && GTK_IS_ENTRY(probe.folder) && GTK_IS_LABEL(detail));
    g_object_ref(probe.inspect);
    g_object_ref(probe.folder);
    g_object_ref(detail);
    controlsRetained = 1;
    /* Mapping is asynchronous. In particular, the hide case must exercise a
     * real unmap notification rather than hiding a not-yet-mapped window. */
    gint64 mapDeadline = g_get_monotonic_time() + 10000000;
    while (!gtk_widget_get_mapped(GTK_WIDGET(form)))
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        CHECK(g_get_monotonic_time() < mapDeadline);
        g_usleep(1000U);
    }
    if (strncmp(probe.mode, "notify-", 7U) == 0)
        g_signal_connect(detail, "notify::label", G_CALLBACK(Publication), &probe);
    if (strcmp(probe.mode, "invalid") == 0)
        gtk_editable_set_text(GTK_EDITABLE(probe.folder), "relative/tools");
    if (strcmp(probe.mode, "retained") == 0)
        Dispose(&probe);
    g_signal_emit_by_name(probe.inspect, "clicked");
    if (strcmp(probe.mode, "edit") == 0 || strcmp(probe.mode, "edit-back") == 0)
    {
        gtk_editable_set_text(GTK_EDITABLE(probe.folder), "relative/tools");
        if (strcmp(probe.mode, "edit-back") == 0)
            gtk_editable_set_text(GTK_EDITABLE(probe.folder), scratch);
    }
    else if (strcmp(probe.mode, "close") == 0)
        Dispose(&probe);
    else if (strcmp(probe.mode, "hide") == 0)
    {
        gtk_widget_set_visible(GTK_WIDGET(form), FALSE);
        gtk_window_present(form);
    }
    else if (strcmp(probe.mode, "invalid") != 0 && strcmp(probe.mode, "retained") != 0 &&
             strcmp(probe.mode, "files") != 0 && strncmp(probe.mode, "notify-", 7U) != 0)
        CHECK(0 && "Unknown case");
    gint64 deadline = g_get_monotonic_time() + 10000000;
    for (;;)
    {
        while (g_main_context_iteration(NULL, FALSE))
        {
        }
        if (probe.dialog == NULL ? Released(&rootReference)
                                 : gtk_widget_get_sensitive(probe.inspect))
            break;
        CHECK(g_get_monotonic_time() < deadline);
        g_usleep(1000U);
    }
    CHECK(probe.applied == 0U && umi_build_profile_equal(&profile, &original));
    const char *text = gtk_label_get_text(GTK_LABEL(detail));
    if (strcmp(probe.mode, "files") == 0)
    {
        CHECK(strstr(text, "Captured tool files.") != NULL);
        CHECK(strstr(text, "CMake:") != NULL && strstr(text, "Clangd language server:") != NULL);
        CHECK(strstr(text, "CMake chooses the compiler") != NULL);
        CHECK(strstr(text, "project tools folder only") != NULL);
    }
    else if (strcmp(probe.mode, "invalid") == 0)
        CHECK(strstr(text, "Tool inspection:") != NULL);
    else if (strcmp(probe.mode, "edit") == 0 || strcmp(probe.mode, "edit-back") == 0 ||
             strcmp(probe.mode, "hide") == 0 || strcmp(probe.mode, "notify-edit") == 0 ||
             strcmp(probe.mode, "notify-edit-back") == 0)
        CHECK(strstr(text, "Inspect again for current locations.") != NULL);
    if (strncmp(probe.mode, "notify-", 7U) == 0)
        CHECK(probe.notifications == 1U);
    if (probe.dialog == NULL)
    {
        g_signal_emit_by_name(probe.inspect, "clicked");
        CHECK(probe.applied == 0U && Released(&rootReference));
    }
cleanup:
    if (detail != NULL)
        g_signal_handlers_disconnect_by_data(detail, &probe);
    Dispose(&probe);
    /* The worker owns only a short-lived reference. Drain its completion before
     * releasing test storage that notification handlers previously borrowed. */
    if (root != NULL)
    {
        gint64 until = g_get_monotonic_time() + 10000000;
        while (!Released(&rootReference) && g_get_monotonic_time() < until)
        {
            while (g_main_context_iteration(NULL, FALSE))
            {
            }
            g_usleep(1000U);
        }
        if (!Released(&rootReference))
            failed = 1;
    }
    g_weak_ref_clear(&rootReference);
    if (form != NULL)
        g_object_unref(form);
    /* Controls are retained only after every lookup succeeded above. */
    if (controlsRetained)
    {
        g_object_unref(probe.inspect);
        g_object_unref(probe.folder);
        g_object_unref(detail);
    }
    if (parent != NULL)
    {
        gtk_window_destroy(parent);
        g_object_unref(parent);
    }
    if (scratch != NULL && umi_fs_remove_tree(scratch) != UMI_STATUS_OK)
        failed = 1;
    g_free(scratch);
    return failed ? 1 : 0;
}
