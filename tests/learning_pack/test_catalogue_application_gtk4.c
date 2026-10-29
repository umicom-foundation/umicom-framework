/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_application_gtk4.c
 *
 * PURPOSE:
 *   Verify learning-window retirement and application lifetime during local reads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/learning_catalogue.h"
#include "umicom/platform/rooted_files.h"
#include <stdio.h>
#include <string.h>

typedef struct WindowCase {
    const char *name;
    const char *directory;
    GApplication *application;
    GtkWindow *parent;
    GtkWindow *window;
    GtkWidget *panel;
    int failed;
    int activated;
    int shutdown_seen;
    guint timeout;
    char *copy_path;
} WindowCase;

#define REQUIRE(context, condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    (context)->failed = 1; goto close_windows; } } while (0)

static int Drain(void)
{
    gint64 until = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < until) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
/* A destroyed window loses its application association. Use the test-owned
 * application for timeout reporting; retain the former lookup for review. */
#if 0
static gboolean Timeout(gpointer data)
{
    WindowCase *context = data;
    context->timeout = 0U;
    context->failed = 1;
    fprintf(stderr, "Learning read did not release the application within 20 seconds\n");
    if (context->parent != NULL) {
        GtkApplication *application = gtk_window_get_application(context->parent);
        if (application != NULL) g_application_quit(G_APPLICATION(application));
    }
    return G_SOURCE_REMOVE;
}
#endif
static gboolean Timeout(gpointer data)
{
    WindowCase *context = data;
    context->timeout = 0U;
    context->failed = 1;
    fprintf(stderr, "Learning read did not release the application within 20 seconds\n");
    if (context->application != NULL) g_application_quit(context->application);
    return G_SOURCE_REMOVE;
}
static void Shutdown(GApplication *application, gpointer data)
{
    (void)application;
    WindowCase *context = data;
    context->shutdown_seen = 1;
    if (UmiLearningCatalogueGtk4PendingReads() != 0U) context->failed = 1;
}
/* Complete setup while a real application window owns the context, then leave
 * the explicit copy pending. Only run() may finish that job after close. */
static int PrepareCopy(WindowCase *context)
{
    if (UmiLearningCatalogueGtk4Load(context->panel, context->directory) != UMI_STATUS_OK || !Drain())
        return 0;
    if (UmiLearningCatalogueGtk4Open(context->panel, 0U) != UMI_STATUS_OK || !Drain()) return 0;
    char *uuid = g_uuid_string_random();
    context->copy_path = g_strdup_printf("copied-%s.c", uuid);
    g_free(uuid);
    return UmiLearningCatalogueGtk4CopyExample(context->panel, context->directory,
        context->copy_path) == UMI_STATUS_OK;
}

static void Activate(GApplication *application, gpointer data)
{
    WindowCase *context = data;
    context->activated = 1;
    context->application = application;
    context->parent = GTK_WINDOW(gtk_application_window_new(GTK_APPLICATION(application)));
    g_object_ref(context->parent);
    context->window = UmiLearningCatalogueGtk4Present(context->parent, (UmiVersion){0U, 9U, 0U});
    REQUIRE(context, context->window != NULL);
    g_object_ref(context->window);
    context->panel = gtk_window_get_child(context->window);
    REQUIRE(context, context->panel != NULL);
    g_object_ref(context->panel);
    REQUIRE(context, gtk_window_get_transient_for(context->window) == context->parent);
    REQUIRE(context, gtk_window_get_application(context->window) == GTK_APPLICATION(application));
    REQUIRE(context, gtk_window_get_destroy_with_parent(context->window));
    if (strcmp(context->name, "copy-application-hold") == 0) {
        REQUIRE(context, PrepareCopy(context));
        REQUIRE(context, UmiLearningCatalogueGtk4PendingReads() == 1U);
    } else if (strcmp(context->name, "close-pending") == 0 ||
        strcmp(context->name, "parent-pending") == 0 ||
        strcmp(context->name, "application-hold") == 0) {
        REQUIRE(context, UmiLearningCatalogueGtk4Load(context->panel, context->directory) == UMI_STATUS_OK);
        /* Completion cannot run until this activation callback yields. Closing
         * both windows must leave the accepted read's application hold active. */
        REQUIRE(context, UmiLearningCatalogueGtk4PendingReads() == 1U);
    } else REQUIRE(context, strcmp(context->name, "close-idle") == 0);
    /* Preserve the earlier teardown for review. The fixture owns an extra
     * parent reference; release it so GTK can emit parent destruction and
     * retire the transient child while that child and its panel stay retained. */
#if 0
    if (strcmp(context->name, "parent-pending") == 0)
        gtk_window_destroy(context->parent);
    else
        gtk_window_close(context->window);
#endif
    if (strcmp(context->name, "parent-pending") == 0) {
        gtk_window_destroy(context->parent);
        g_clear_object(&context->parent);
    } else
        gtk_window_close(context->window);
    UmiLearningCatalogueGtkState state;
    REQUIRE(context, UmiLearningCatalogueGtk4CopyState(context->panel, &state) == UMI_STATUS_OK);
    REQUIRE(context, state.retired);
    REQUIRE(context, UmiLearningCatalogueGtk4Load(context->panel, context->directory) == UMI_STATUS_INVALID_STATE);
close_windows:
    if (context->window != NULL) gtk_window_destroy(context->window);
    if (context->parent != NULL) gtk_window_destroy(context->parent);
    /* Only application-hold relies on run() alone. The other pending cases
     * exercise explicit context draining after close or parent destruction. */
    /* Copy and read holds must each survive the last window closing. */
#if 0
    if (strcmp(context->name, "application-hold") != 0 && !Drain()) context->failed = 1;
#endif
    if (strcmp(context->name, "application-hold") != 0 &&
        strcmp(context->name, "copy-application-hold") != 0 && !Drain()) context->failed = 1;
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    static const char text[] = "Read an owned local learning resource.\n";
    char digest[UMI_SHA256_HEX_CAPACITY], manifest[1400];
    if (UmiSha256Buffer(text, sizeof(text) - 1U, digest) != UMI_STATUS_OK) return 1;
    int length = snprintf(manifest, sizeof(manifest),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"window.fixture\","
        "\"title\":\"Window guide\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"guide\",\"title\":\"Guide\",\"topic\":\"ownership\",\"kind\":\"guide\","
        "\"path\":\"guide.txt\",\"sha256\":\"%s\"}]}", digest);
    if (length < 0 || (size_t)length >= sizeof(manifest)) return 1;
    if (UmiRootedFileWrite(argv[2], "guide.txt", text, sizeof(text) - 1U) != UMI_STATUS_OK ||
        UmiRootedFileWrite(argv[2], "learning-pack.json", manifest, (size_t)length) != UMI_STATUS_OK)
        return 1;
    if (strcmp(argv[1], "copy-application-hold") == 0) {
        /* The same verified bytes are an explicit example in this fixture.
         * Rewriting the owned manifest does not change production pack rules. */
        char *kind = strstr(manifest, "\"kind\":\"guide\"");
        if (kind == NULL) return 1;
        char copy_manifest[1600];
        size_t prefix = (size_t)(kind - manifest);
        memcpy(copy_manifest, manifest, prefix);
        int appended = snprintf(copy_manifest + prefix, sizeof(copy_manifest) - prefix,
            "\"kind\":\"example\"%s", kind + strlen("\"kind\":\"guide\""));
        if (appended < 0 || (size_t)appended >= sizeof(copy_manifest) - prefix ||
            UmiRootedFileWrite(argv[2], "learning-pack.json", copy_manifest,
                prefix + (size_t)appended) != UMI_STATUS_OK) return 1;
    }
    WindowCase context = {0};
    context.name = argv[1];
    context.directory = argv[2];
    GtkApplication *application = gtk_application_new(
        "org.umicom.framework.learning-window-test", G_APPLICATION_NON_UNIQUE);
    g_signal_connect(application, "activate", G_CALLBACK(Activate), &context);
    g_signal_connect(application, "shutdown", G_CALLBACK(Shutdown), &context);
    context.timeout = g_timeout_add_seconds(20U, Timeout, &context);
    int result = g_application_run(G_APPLICATION(application), 0, NULL);
    if (context.timeout != 0U) g_source_remove(context.timeout);
    if (!context.activated || !context.shutdown_seen || UmiLearningCatalogueGtk4PendingReads() != 0U)
        context.failed = 1;
    if (context.panel != NULL) {
        UmiLearningCatalogueGtkState state;
        if (UmiLearningCatalogueGtk4CopyState(context.panel, &state) != UMI_STATUS_OK ||
            !state.retired || state.busy || state.has_copy_result ||
            state.pack_count != (strcmp(context.name, "copy-application-hold") == 0 ? 1U : 0U))
            context.failed = 1;
        UmiLearningCatalogueGtk4Retire(context.panel);
    }
    /* Failure cleanup also preserves the event loop until all accepted jobs
     * finish; it cannot turn the earlier shutdown observation into a pass. */
    if (!Drain()) context.failed = 1;
    if (context.copy_path != NULL) {
        unsigned char *copied = NULL;
        size_t bytes = 0U;
        if (UmiRootedFileRead(context.directory, context.copy_path, 1024U, &copied, &bytes) != UMI_STATUS_OK ||
            bytes != sizeof(text) - 1U || memcmp(copied, text, bytes) != 0) context.failed = 1;
        UmiRootedFileFree(copied);
        g_free(context.copy_path);
    }
    g_clear_object(&context.panel);
    g_clear_object(&context.window);
    g_clear_object(&context.parent);
    g_object_unref(application);
    return result != 0 || context.failed ? 1 : 0;
}
