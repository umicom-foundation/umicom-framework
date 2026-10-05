/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_presets/test_gtk4.c
 * PURPOSE: Exercise asynchronous preset reads and explicit settings acceptance.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/developer_dialog.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>
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
    int failed = 0;
    Probe probe = {0};
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *detail = NULL, *folder = NULL, *shared = NULL, *cancel = NULL, *entries[3] = {0};
    gpointer rootWeak = NULL;
    GtkWidget *readIncludes = NULL;
    char *includedFile = NULL;
    char *scratch = NULL, *projectFile = NULL, *userFile = NULL;
    const char *project = "{\"version\":6,\"include\":[\"extra.json\"],\"configurePresets\":[{\"name\":"
                          "\"configure-notes\",\"binaryDir\":\"${sourceDir}/"
                          "build\"},{\"name\":\"base\",\"hidden\":true}],\"buildPresets\":[{\"name\":\"build-"
                          "notes\"}],\"testPresets\":[{\"name\":\"disabled-test\",\"condition\":false}]}";
    const char *user =
        "{\"version\":3,\"testPresets\":[{\"name\":\"test-notes\",\"description\":\"My checks\"}]}";
    CHECK(argc == 2);
    const char *mode = argv[1];
    if (!gtk_init_check())
        return 77;
    scratch = g_dir_make_tmp("umicom-presets-XXXXXX", NULL);
    CHECK(scratch != NULL);
    projectFile = g_build_filename(scratch, "CMakePresets.json", NULL);
    userFile = g_build_filename(scratch, "CMakeUserPresets.json", NULL);
    CHECK(g_file_set_contents(projectFile, project, -1, NULL));
    CHECK(g_file_set_contents(userFile, user, -1, NULL));
    bool expandedCase = strncmp(mode, "includes-", 9U) == 0;
    if (expandedCase) {
        /* Explicit traversal reads a real included document. The direct-only
         * fixtures above remain unchanged for the existing action. */
        const char *expanded = "{\"version\":9,\"include\":[\"extra.json\"],"
            "\"configurePresets\":[{\"name\":\"derived\",\"inherits\":\"shared\"}],"
            "\"buildPresets\":[{\"name\":\"build-derived\",\"inherits\":\"shared-build\"}]}";
        CHECK(g_file_set_contents(projectFile, expanded, -1, NULL));
        includedFile = g_build_filename(scratch, "extra.json", NULL);
        if (strcmp(mode, "includes-missing") != 0)
            CHECK(g_file_set_contents(includedFile, "{\"version\":9,"
                "\"configurePresets\":[{\"name\":\"shared\",\"hidden\":true,\"binaryDir\":\"shared-build-folder\"}],"
                "\"buildPresets\":[{\"name\":\"shared-build\",\"hidden\":true,\"configurePreset\":\"derived\"}]}", -1, NULL));
    }

    umi_build_profile_init(&probe.profile);
    CHECK(g_strlcpy(probe.profile.source_directory, scratch, sizeof(probe.profile.source_directory)) <
          sizeof(probe.profile.source_directory));
    strcpy(probe.profile.configure_preset, "manual-configure");
    strcpy(probe.profile.build_preset, "manual-build");
    strcpy(probe.profile.test_preset, "manual-test");
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &probe.profile, 0, Applied, &probe, &probe.dialog) ==
          UMI_STATUS_OK);
    form = Form(parent);
    CHECK(form != NULL);
    rootWeak = gtk_window_get_child(form);
    g_object_add_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
    const char *ids[] = {
        "developer.presets.read",       "developer.presets.use",      "developer.dialog.accept",
        "developer.presets.choices",    "developer.presets.detail",   "developer.build.field.1",
        "developer.build.field.6",      "developer.dialog.cancel",    "developer.build.configure-preset",
        "developer.build.build-preset", "developer.build.test-preset"};
    GtkWidget **controls[] = {&probe.read, &probe.use, &probe.accept, &probe.picker, &detail,    &folder,
                              &shared,     &cancel,    &entries[0],   &entries[1],   &entries[2]};
    for (size_t i = 0U; i < sizeof(ids) / sizeof(ids[0]); ++i)
    {
        *controls[i] = Find(GTK_WIDGET(form), ids[i]);
        CHECK(*controls[i] != NULL);
        g_object_ref(*controls[i]);
    }
    readIncludes = Find(GTK_WIDGET(form), "developer.presets.read-includes");
    CHECK(readIncludes != NULL);
    g_object_ref(readIncludes);
    CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL);
    CHECK(!gtk_widget_get_sensitive(probe.use) && probe.calls == 0U);
    if (strcmp(mode, "no-auto-read") == 0)
        goto cleanup;
    if (strcmp(mode, "model-destroy") == 0)
        g_signal_connect(probe.picker, "notify::model", G_CALLBACK(DestroyOnModel), &probe);
    if (strcmp(mode, "reentrant-load") == 0)
        g_signal_connect(probe.picker, "notify::model", G_CALLBACK(Reenter), &probe);
    if (strcmp(mode, "missing") == 0)
    {
        CHECK(g_remove(projectFile) == 0);
        CHECK(g_remove(userFile) == 0);
    }
    if (strcmp(mode, "user-only") == 0)
        CHECK(g_remove(projectFile) == 0);
    if (strcmp(mode, "relative-folder") == 0)
        gtk_editable_set_text(GTK_EDITABLE(folder), "relative/project");
    /* The worker is started only by this explicit action. It does not invoke
     * CMake, follow the missing include, or apply any settings. */
    if (expandedCase) {
        g_signal_emit_by_name(readIncludes, "clicked");
        if (strcmp(mode, "includes-retained") == 0) { Dispose(&probe); goto cleanup; }
        if (strcmp(mode, "includes-cancel") == 0) gtk_editable_set_text(GTK_EDITABLE(folder), "changed/project");
        CHECK(WaitReady(probe.read) && gtk_widget_get_sensitive(readIncludes));
        CHECK(probe.calls == 0U);
        if (strcmp(mode, "includes-missing") == 0 || strcmp(mode, "includes-cancel") == 0) {
            CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL);
            CHECK(!gtk_widget_get_sensitive(probe.use));
            if (strcmp(mode, "includes-missing") == 0)
                CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "extra.json") != NULL);
            goto cleanup;
        }
        GListModel *expandedModel = gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker));
        CHECK(expandedModel != NULL && g_list_model_get_n_items(expandedModel) == 5U);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "3 files") != NULL);
        guint expandedIndex = strcmp(mode, "includes-origin") == 0 ? 2U : strcmp(mode, "includes-inherited") == 0 ? 1U : 0U;
        gtk_drop_down_set_selected(GTK_DROP_DOWN(probe.picker), expandedIndex);
        if (strcmp(mode, "includes-origin") == 0) {
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "extra.json") != NULL);
            g_signal_emit_by_name(probe.use, "clicked");
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[0])), "manual-configure") == 0);
        } else {
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "inherited values") != NULL);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), expandedIndex == 0U ? "shared-build-folder" : "derived") != NULL);
            g_signal_emit_by_name(probe.use, "clicked");
            CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[expandedIndex])), expandedIndex == 0U ? "derived" : "build-derived") == 0);
            CHECK(probe.calls == 0U);
        }
        goto cleanup;
    }
    g_signal_emit_by_name(probe.read, "clicked");
    if (strcmp(mode, "pending-destroy") == 0)
    {
        Dispose(&probe);
        goto cleanup;
    }
    if (strcmp(mode, "pending-parent") == 0)
    {
        gtk_window_destroy(parent);
        goto cleanup;
    }
    if (strcmp(mode, "pending-cancel") == 0)
    {
        g_signal_emit_by_name(cancel, "clicked");
        goto cleanup;
    }
    if (strcmp(mode, "model-destroy") == 0)
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
    if (strcmp(mode, "reentrant-load") == 0)
    {
        CHECK(probe.notifications != 0U);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[0])), "manual-configure") == 0);
        goto cleanup;
    }
    if (strcmp(mode, "missing") == 0 || strcmp(mode, "relative-folder") == 0)
    {
        CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL);
        CHECK(!gtk_widget_get_sensitive(probe.use));
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "not loaded") != NULL);
        goto cleanup;
    }
    GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker));
    CHECK(model != NULL);
    CHECK(g_list_model_get_n_items(model) == (strcmp(mode, "user-only") == 0 ? 1U : 5U));
    if (strcmp(mode, "user-only") != 0)
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "incomplete") != NULL);
    CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(probe.picker)) == GTK_INVALID_LIST_POSITION);
    if (strcmp(mode, "refresh-invalid") == 0)
    {
        CHECK(g_file_set_contents(userFile, "{", -1, NULL));
        g_signal_emit_by_name(probe.read, "clicked");
        CHECK(WaitReady(probe.read));
        CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL);
        CHECK(!gtk_widget_get_sensitive(probe.use));
        goto cleanup;
    }
    if (strcmp(mode, "reentrant-detail") == 0)
        g_signal_connect(detail, "notify::label", G_CALLBACK(Reenter), &probe);
    guint selected = strcmp(mode, "hidden") == 0 ? 1U : strcmp(mode, "false-condition") == 0 ? 3U : 0U;
    gtk_drop_down_set_selected(GTK_DROP_DOWN(probe.picker), selected);
    if (strcmp(mode, "reentrant-detail") == 0)
    {
        CHECK(probe.notifications == 1U && probe.calls == 0U);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[0])), "manual-configure") == 0);
        goto cleanup;
    }
    if (strcmp(mode, "legacy") == 0)
        gtk_editable_set_text(GTK_EDITABLE(shared), "shared-preset");
    if (strcmp(mode, "changed-folder") == 0)
        gtk_editable_set_text(GTK_EDITABLE(folder), "another/project");
    if (strcmp(mode, "changed-back") == 0) {
        gtk_editable_set_text(GTK_EDITABLE(folder), "another/project");
        gtk_editable_set_text(GTK_EDITABLE(folder), scratch);
    }
    if (strcmp(mode, "reentrant-entry") == 0)
        g_signal_connect(entries[0], "changed", G_CALLBACK(ReenterEntry), &probe);
    g_signal_emit_by_name(probe.use, "clicked");
    CHECK(probe.calls == 0U);
    if (strcmp(mode, "hidden") == 0 || strcmp(mode, "false-condition") == 0 || strcmp(mode, "legacy") == 0 ||
        strcmp(mode, "changed-folder") == 0 || strcmp(mode, "changed-back") == 0)
    {
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[0])), "manual-configure") == 0);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[1])), "manual-build") == 0);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[2])), "manual-test") == 0);
        goto cleanup;
    }
    if (strcmp(mode, "user-only") == 0)
    {
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[2])), "test-notes") == 0);
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "CMakeUserPresets.json") != NULL);
        goto cleanup;
    }
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[0])), "configure-notes") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[1])), "manual-build") == 0);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(entries[2])), "manual-test") == 0);
    if (strcmp(mode, "reentrant-entry") == 0)
    {
        CHECK(probe.notifications != 0U);
        goto cleanup;
    }
    CHECK(strcmp(mode, "stages-apply") == 0);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(probe.picker), 2U);
    g_signal_emit_by_name(probe.use, "clicked");
    gtk_drop_down_set_selected(GTK_DROP_DOWN(probe.picker), 4U);
    g_signal_emit_by_name(probe.use, "clicked");
    CHECK(probe.calls == 0U);
    g_signal_emit_by_name(probe.accept, "clicked");
    CHECK(probe.calls == 1U);
    CHECK(strcmp(probe.profile.configure_preset, "configure-notes") == 0);
    CHECK(strcmp(probe.profile.build_preset, "build-notes") == 0);
    CHECK(strcmp(probe.profile.test_preset, "test-notes") == 0);
cleanup:
    /* Release only the unique fixture's two known files. Wait for the owned
     * worker to retire before removing them, including after window teardown. */
    if (probe.picker != NULL)
        g_signal_handlers_disconnect_by_data(probe.picker, &probe);
    if (detail != NULL)
        g_signal_handlers_disconnect_by_data(detail, &probe);
    if (entries[0] != NULL)
        g_signal_handlers_disconnect_by_data(entries[0], &probe);
    Dispose(&probe);
    gint64 deadline = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (rootWeak != NULL && g_get_monotonic_time() < deadline)
    {
        g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    if (rootWeak != NULL)
    {
        fprintf(stderr, "Preset worker did not retire before the deadline\n");
        g_object_remove_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
        failed = 1;
    }
    /* Pending-close cases must also prove they never accepted settings. */
    if (probe.calls > (argc == 2 && strcmp(argv[1], "stages-apply") == 0 ? 1U : 0U))
        failed = 1;
    unsigned accepted = probe.calls;
    if (probe.read != NULL)
        g_signal_emit_by_name(probe.read, "clicked");
    if (probe.use != NULL)
        g_signal_emit_by_name(probe.use, "clicked");
    if (readIncludes != NULL) {
        g_signal_emit_by_name(readIncludes, "clicked");
        g_object_unref(readIncludes);
    }
    if (probe.calls != accepted)
        failed = 1;
    GtkWidget *retained[] = {probe.read, probe.use, probe.accept, probe.picker, detail,    folder,
                             shared,     cancel,    entries[0],   entries[1],   entries[2]};
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
    if (projectFile != NULL)
    {
        (void)g_remove(projectFile);
        g_free(projectFile);
    }
    if (userFile != NULL)
    {
        (void)g_remove(userFile);
        g_free(userFile);
    }
    if (includedFile != NULL) {
        (void)g_remove(includedFile);
        g_free(includedFile);
    }
    if (scratch != NULL)
    {
        (void)g_rmdir(scratch);
        g_free(scratch);
    }
    return failed;
}
