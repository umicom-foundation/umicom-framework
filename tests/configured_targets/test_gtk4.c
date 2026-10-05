/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/configured_targets/test_gtk4.c
 * PURPOSE: Exercise native target discovery, explicit query writes and retained-control lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/developer_dialog.h"
#include <glib/gstdio.h>
#include "fixture.h"
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


/* Two distinct targets deliberately reverse their alphabetical order. Picking
 * visible row zero after filtering must still choose the second source row. */
static UmiStatus ProjectionFixture(const char *root, const char *build)
{
    char sourceJson[UMI_BUILD_PATH_CAPACITY*2U], buildJson[UMI_BUILD_PATH_CAPACITY*2U];
    if (!FixtureQuote(root,sourceJson,sizeof(sourceJson)) || !FixtureQuote(build,buildJson,sizeof(buildJson)))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    char *model=g_strdup_printf("{\"kind\":\"codemodel\",\"version\":{\"major\":2,\"minor\":0},\"paths\":{\"source\":\"%s\",\"build\":\"%s\"},\"configurations\":[{\"name\":\"Debug\",\"targets\":[{\"name\":\"notes\",\"id\":\"notes::test\",\"jsonFile\":\"target.json\"},{\"name\":\"alpha\",\"id\":\"alpha::test\",\"jsonFile\":\"alpha.json\"}]}]}",sourceJson,buildJson);
    UmiStatus status=FixtureFile(build,".cmake/api/v1/reply/model.json",model);
    g_free(model);
    if(status==UMI_STATUS_OK)status=FixtureFile(build,".cmake/api/v1/reply/alpha.json",
        "{\"name\":\"alpha\",\"id\":\"alpha::test\",\"type\":\"EXECUTABLE\",\"nameOnDisk\":\"alpha.exe\",\"artifacts\":[{\"path\":\"bin/alpha.exe\"}]}");
    return status;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    if (!gtk_init_check())
        return 77;
    const char *mode = argv[1];
    int failed = 0;
    Probe probe = {0};
    GtkWindow *parent = NULL, *form = NULL;
    GtkWidget *request = NULL, *buildUse = NULL, *detail = NULL, *program = NULL, *buildField = NULL,
              *cancel = NULL;
    char *scratch = NULL, *query = NULL;
    GtkWidget *cacheButton = NULL;
    char build[UMI_BUILD_PATH_CAPACITY];
    gpointer rootWeak = NULL;
    scratch = g_dir_make_tmp("umicom-targets-XXXXXX", NULL);
    CHECK(scratch != NULL);
    CHECK(FixtureReply(scratch, "valid", build) == UMI_STATUS_OK);
    if (strncmp(mode, "projection-", 11U) == 0) CHECK(ProjectionFixture(scratch, build) == UMI_STATUS_OK);
    query =
        g_build_filename(build, ".cmake", "api", "v1", "query", "client-umicom-studio", "codemodel-v2", NULL);
    umi_build_profile_init(&probe.profile);
    CHECK(g_strlcpy(probe.profile.source_directory, scratch, sizeof(probe.profile.source_directory)) <
          sizeof(probe.profile.source_directory));
    strcpy(probe.profile.build_directory, build);
    strcpy(probe.profile.run_program, "original-program");
    parent = GTK_WINDOW(gtk_window_new());
    g_object_ref(parent);
    CHECK(UmiGtk4BuildSettingsDialogCreate(parent, &probe.profile, 0, Applied, &probe, &probe.dialog) ==
          UMI_STATUS_OK);
    form = Form(parent);
    CHECK(form != NULL);
    rootWeak = gtk_window_get_child(form);
    g_object_add_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
    const char *ids[] = {"developer.targets.read",    "developer.targets.program", "developer.dialog.accept",
                         "developer.targets.choices", "developer.targets.request", "developer.targets.build",
                         "developer.targets.detail",  "developer.build.field.8",   "developer.build.field.2",
                         "developer.dialog.cancel"};
    GtkWidget **controls[] = {&probe.read, &probe.use, &probe.accept, &probe.picker, &request,
                              &buildUse,   &detail,    &program,      &buildField,   &cancel};
    for (size_t i = 0U; i < sizeof(ids) / sizeof(ids[0]); ++i)
    {
        *controls[i] = Find(GTK_WIDGET(form), ids[i]);
        CHECK(*controls[i] != NULL);
        g_object_ref(*controls[i]);
    }
    CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL && probe.calls == 0U);
    CHECK(!g_file_test(query, G_FILE_TEST_EXISTS));
    if (strncmp(mode, "cache-", 6U) == 0)
    {
        cacheButton = Find(GTK_WIDGET(form), "developer.build.inspect-cache");
        CHECK(cacheButton != NULL);
        g_object_ref(cacheButton);
        char *cache = g_strdup_printf("CMAKE_HOME_DIRECTORY:INTERNAL=%s%s\nCMAKE_CACHEFILE_DIR:INTERNAL=%s\nCMAKE_GENERATOR:INTERNAL=Ninja\nCMAKE_C_COMPILER:FILEPATH=clang\nCMAKE_BUILD_TYPE:STRING=Debug\n",
            scratch, strcmp(mode, "cache-mismatch") == 0 ? "/other" : "", build);
        UmiStatus written = strcmp(mode, "cache-missing") == 0 ? UMI_STATUS_OK : FixtureFile(build, "CMakeCache.txt", cache);
        g_free(cache);
        CHECK(written == UMI_STATUS_OK);
        g_signal_emit_by_name(cacheButton, "clicked");
        /* All read operations share the busy gate. Applying while inspection
         * is pending must not publish the form's mutable fields. */
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 0U);
        if (strcmp(mode, "cache-close") == 0) { Dispose(&probe); goto cleanup; }
        if (strcmp(mode, "cache-hide") == 0) { g_signal_emit_by_name(cancel, "clicked"); goto cleanup; }
        if (strcmp(mode, "cache-changed") == 0)
            gtk_editable_set_text(GTK_EDITABLE(buildField), "changed-folder");
        CHECK(WaitReady(probe.read));
        CHECK(probe.calls == 0U && gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL);
        const char *message = gtk_label_get_text(GTK_LABEL(detail));
        CHECK(strstr(message, strcmp(mode, "cache-mismatch") == 0 ? "PROJECT MISMATCH"
            : strcmp(mode, "cache-missing") == 0 ? "inspection failed"
            : strcmp(mode, "cache-changed") == 0 ? "changed"
            : "Project and build-folder paths match") != NULL);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "original-program") == 0);
        goto cleanup;
    }
    if (strcmp(mode, "no-auto-read") == 0)
        goto cleanup;
    if (strncmp(mode, "query", 5U) == 0)
    {
        if (strcmp(mode, "query-missing") == 0)
        {
            char *missing = g_build_filename(scratch, "absent", NULL);
            gtk_editable_set_text(GTK_EDITABLE(buildField), missing);
            g_free(missing);
        }
        if (strcmp(mode, "query-empty") == 0)
            gtk_editable_set_text(GTK_EDITABLE(buildField), "");
        g_signal_emit_by_name(request, "clicked");
        CHECK(WaitReady(probe.read));
        if (strcmp(mode, "query-missing") == 0 || strcmp(mode, "query-empty") == 0)
        {
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)),
                         strcmp(mode, "query-empty") == 0 ? "Invalid argument" : "Not found") != NULL);
            CHECK(!g_file_test(query, G_FILE_TEST_EXISTS));
            goto cleanup;
        }
        CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), ": OK.") != NULL);
        CHECK(g_file_test(query, G_FILE_TEST_IS_REGULAR));
        char *contents = NULL;
        gsize size = 0U;
        CHECK(g_file_get_contents(query, &contents, &size, NULL));
        g_free(contents);
        CHECK(size == 0U && probe.calls == 0U);
        if (strcmp(mode, "query-conflict") == 0)
            CHECK(g_file_set_contents(query, "retain this", -1, NULL));
        if (strcmp(mode, "query-existing") == 0 || strcmp(mode, "query-conflict") == 0)
        {
            g_signal_emit_by_name(request, "clicked");
            CHECK(WaitReady(probe.read));
            CHECK(g_file_get_contents(query, &contents, &size, NULL));
            int preserved =
                strcmp(mode, "query-conflict") == 0 ? strcmp(contents, "retain this") == 0 : size == 0U;
            g_free(contents);
            CHECK(preserved);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)),
                         strcmp(mode, "query-conflict") == 0 ? "Invalid state" : ": OK.") != NULL);
        }
        goto cleanup;
    }
    if (strcmp(mode, "model-destroy") == 0)
        g_signal_connect(probe.picker, "notify::model", G_CALLBACK(DestroyOnModel), &probe);
    if (strcmp(mode, "reentrant-model") == 0)
        g_signal_connect(probe.picker, "notify::model", G_CALLBACK(Reenter), &probe);
    g_signal_emit_by_name(probe.read, "clicked");
    if (strcmp(mode, "pending-destroy") == 0)
    {
        Dispose(&probe);
        goto cleanup;
    }
    if (strcmp(mode, "pending-hide") == 0)
    {
        g_signal_emit_by_name(cancel, "clicked");
        goto cleanup;
    }
    if (strcmp(mode, "pending-parent") == 0)
    {
        gtk_window_destroy(parent);
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
    if (strncmp(mode, "projection-", 11U) == 0)
    {
        GtkWidget *filter = Find(GTK_WIDGET(form), "developer.targets.filter");
        CHECK(filter != NULL);
        GtkWidget *search = Find(filter, "choices.query"), *order = Find(filter, "choices.order"), *apply = Find(filter, "choices.apply");
        CHECK(search && order && apply);
        gtk_editable_set_text(GTK_EDITABLE(search), !strcmp(mode, "projection-hidden") ? "missing" : "alpha");
        gtk_drop_down_set_selected(GTK_DROP_DOWN(order), 1U);
        g_signal_emit_by_name(apply, "clicked");
        GListModel *filtered = gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker));
        if (!strcmp(mode, "projection-hidden"))
        {
            CHECK(filtered == NULL);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "No configured target") != NULL);
        }
        else
        {
            CHECK(filtered != NULL && g_list_model_get_n_items(filtered) == 1U);
            gtk_drop_down_set_selected(GTK_DROP_DOWN(probe.picker), 0U);
            CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "alpha.exe") != NULL);
        }
        if (!strcmp(mode, "projection-invalidated")) gtk_editable_set_text(GTK_EDITABLE(buildField), "other-build");
        g_signal_emit_by_name(!strcmp(mode, "projection-build") ? buildUse : probe.use, "clicked");
        if (!strcmp(mode, "projection-program"))
        {
            char expected[UMI_BUILD_PATH_CAPACITY];
            CHECK(umi_path_join(build, "bin/alpha.exe", expected, sizeof(expected)) == UMI_STATUS_OK);
            CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), expected));
        }
        else CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "original-program"));
        if (!strcmp(mode, "projection-build"))
            CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(GTK_WIDGET(form), "developer.build.field.7"))), "alpha"));
        CHECK(probe.calls == 0U);
        goto cleanup;
    }
    GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker));
    CHECK(model != NULL && g_list_model_get_n_items(model) == 1U && probe.calls == 0U);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(probe.picker), 0U);
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(detail)), "notes.exe") != NULL);
    if (strcmp(mode, "changed-input") == 0)
        gtk_editable_set_text(GTK_EDITABLE(buildField), "different-build");
    if (strcmp(mode, "refresh-failure") == 0)
    {
        CHECK(FixtureReply(scratch, "syntax", build) == UMI_STATUS_OK);
        g_signal_emit_by_name(probe.read, "clicked");
        CHECK(WaitReady(probe.read));
        CHECK(gtk_drop_down_get_model(GTK_DROP_DOWN(probe.picker)) == NULL);
    }
    if (strcmp(mode, "reentrant-entry") == 0)
        g_signal_connect(program, "changed", G_CALLBACK(ReenterEntry), &probe);
    if (strcmp(mode, "read") != 0 && strcmp(mode, "reentrant-model") != 0)
        g_signal_emit_by_name(strcmp(mode, "build") == 0 ? buildUse : probe.use, "clicked");
    CHECK(probe.calls == 0U);
    if (strcmp(mode, "reentrant-model") == 0 || strcmp(mode, "reentrant-entry") == 0)
        CHECK(probe.notifications != 0U);
    if (strcmp(mode, "changed-input") == 0 || strcmp(mode, "refresh-failure") == 0)
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "original-program") == 0);
    if (strcmp(mode, "program") == 0 || strcmp(mode, "apply") == 0 || strcmp(mode, "reentrant-entry") == 0)
    {
        char expected[UMI_BUILD_PATH_CAPACITY];
        CHECK(umi_path_join(build, "bin/notes.exe", expected, sizeof(expected)) == UMI_STATUS_OK);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), expected) == 0);
    }
    if (strcmp(mode, "build") == 0)
    {
        GtkWidget *target = Find(GTK_WIDGET(form), "developer.build.field.7");
        CHECK(target != NULL);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(target)), "notes") == 0);
        CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(program)), "original-program") == 0);
    }
    if (strcmp(mode, "apply") == 0)
    {
        g_signal_emit_by_name(probe.accept, "clicked");
        CHECK(probe.calls == 1U);
        CHECK(strstr(probe.profile.run_program, "notes.exe") != NULL);
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
    int retired = rootWeak == NULL;
    if (!retired)
    {
        g_object_remove_weak_pointer(G_OBJECT(rootWeak), &rootWeak);
        failed = 1;
    }
    unsigned accepted = probe.calls;
    if (probe.use != NULL)
        g_signal_emit_by_name(probe.use, "clicked");
    if (request != NULL)
        g_signal_emit_by_name(request, "clicked");
    if (probe.calls != accepted || probe.calls > (strcmp(mode, "apply") == 0 ? 1U : 0U))
        failed = 1;
    GtkWidget *retained[] = {probe.read, probe.use, probe.accept, probe.picker, request,
                             buildUse,   detail,    program,      buildField,   cancel};
    for (size_t i = 0U; i < sizeof(retained) / sizeof(retained[0]); ++i)
        if (retained[i] != NULL)
            g_object_unref(retained[i]);
    if (cacheButton != NULL) {
        g_signal_emit_by_name(cacheButton, "clicked");
        if (probe.calls != accepted) failed = 1;
        g_object_unref(cacheButton);
    }
    if (form != NULL)
        g_object_unref(form);
    if (parent != NULL)
    {
        gtk_window_destroy(parent);
        g_object_unref(parent);
    }
    /* Only remove our unique directory after its worker has actually retired. */
    if (scratch != NULL && retired && umi_fs_remove_tree(scratch) != UMI_STATUS_OK)
        failed = 1;
    g_free(query);
    g_free(scratch);
    return failed;
}
