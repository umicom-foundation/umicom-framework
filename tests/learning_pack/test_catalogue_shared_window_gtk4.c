/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_shared_window_gtk4.c
 *
 * PURPOSE:
 *   Verify catalogue reuse, independent parents and closure during host callbacks.
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

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    failed = 1; goto cleanup; } } while (0)

static int Alive(GtkWindow *window)
{
    GList *windows = gtk_window_list_toplevels();
    int found = g_list_find(windows, window) != NULL;
    g_list_free(windows);
    return found;
}
static void Close(GtkWindow *window)
{
    if (window != NULL && Alive(window)) gtk_window_destroy(window);
}
static int Drain(void)
{
    gint64 limit = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < limit) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
static int Fixture(const char *directory)
{
    static const char bytes[] = "A reusable lesson with verified bytes.\n";
    char digest[UMI_SHA256_HEX_CAPACITY], manifest[1800];
    if (UmiSha256Buffer(bytes, sizeof(bytes) - 1U, digest) != UMI_STATUS_OK) return 0;
    int length = snprintf(manifest, sizeof(manifest),
        "{\"schema\":\"umicom.learning.pack\",\"version\":1,\"id\":\"shared.fixture\","
        "\"title\":\"Shared guide\",\"product\":\"framework\",\"locale\":\"en\",\"revision\":1,"
        "\"compatibility\":{\"minimum\":[0,9,0],\"maximum\":[0,9,0]},\"resources\":["
        "{\"id\":\"first\",\"title\":\"First\",\"topic\":\"ownership\",\"kind\":\"guide\","
        "\"path\":\"first.txt\",\"sha256\":\"%s\"},"
        "{\"id\":\"second\",\"title\":\"Second\",\"topic\":\"ownership\",\"kind\":\"lesson\","
        "\"path\":\"second.txt\",\"sha256\":\"%s\"}]}", digest, digest);
    return length > 0 && (size_t)length < sizeof(manifest) &&
        UmiRootedFileWrite(directory, "first.txt", bytes, sizeof(bytes) - 1U) == UMI_STATUS_OK &&
        UmiRootedFileWrite(directory, "second.txt", bytes, sizeof(bytes) - 1U) == UMI_STATUS_OK &&
        UmiRootedFileWrite(directory, "learning-pack.json", manifest, (size_t)length) == UMI_STATUS_OK;
}
typedef struct CallbackCase {
    const char *name;
    GtkWindow *parent;
    unsigned calls;
    int reentrant_refused;
} CallbackCase;
static void WindowAdded(GtkApplication *application, GtkWindow *window, gpointer data)
{
    (void)application;
    CallbackCase *context = data;
    if (window == context->parent) return;
    ++context->calls;
    if (strcmp(context->name, "callback-child") == 0) gtk_window_destroy(window);
    else if (strcmp(context->name, "callback-parent") == 0) gtk_window_destroy(context->parent);
    else {
        /* A repeated action from a host's window-added callback must neither
         * recurse indefinitely nor create a second catalogue. */
        context->reentrant_refused =
            UmiLearningCatalogueGtk4PresentShared(context->parent, (UmiVersion){0U, 9U, 0U}) == NULL;
    }
}
int main(int argc, char **argv)
{
    if (argc != 3) return 2;
    if (!gtk_init_check()) return 77;
    int failed = 0;
    GtkApplication *application = NULL;
    GtkWindow *parent = NULL, *other = NULL, *first = NULL, *second = NULL;
    GtkWidget *panel = NULL;
    GError *error = NULL;
    gulong callback = 0U;
    const UmiVersion host = {0U, 9U, 0U};
    UmiLearningCatalogueGtkState before, after;
    CallbackCase context = {argv[1], NULL, 0U, 0};
    CHECK(Fixture(argv[2]));
    application = gtk_application_new("org.umicom.framework.shared-learning-test",
        G_APPLICATION_NON_UNIQUE);
    CHECK(g_application_register(G_APPLICATION(application), NULL, &error));
    parent = GTK_WINDOW(gtk_application_window_new(application));
    g_object_ref(parent);
    context.parent = parent;
    if (g_str_has_prefix(argv[1], "callback-")) {
        callback = g_signal_connect(application, "window-added", G_CALLBACK(WindowAdded), &context);
        first = UmiLearningCatalogueGtk4PresentShared(parent, host);
        if (first != NULL) g_object_ref(first);
        CHECK(context.calls == 1U);
        if (strcmp(argv[1], "callback-reentrant") == 0) {
            CHECK(first != NULL && context.reentrant_refused);
            CHECK(UmiLearningCatalogueGtk4PresentShared(parent, host) == first);
            CHECK(context.calls == 1U);
        } else CHECK(first == NULL);
        goto cleanup;
    }
    if (strcmp(argv[1], "destroyed-parent") == 0) {
        gtk_window_destroy(parent);
        CHECK(UmiLearningCatalogueGtk4Present(parent, host) == NULL);
        CHECK(UmiLearningCatalogueGtk4PresentShared(parent, host) == NULL);
        goto cleanup;
    }
    first = UmiLearningCatalogueGtk4PresentShared(parent, host);
    CHECK(first != NULL);
    g_object_ref(first);
    panel = gtk_window_get_child(first);
    CHECK(panel != NULL);
    g_object_ref(panel);
    CHECK(UmiLearningCatalogueGtk4Load(panel, argv[2]) == UMI_STATUS_OK);
    if (strcmp(argv[1], "pending-reuse") == 0) {
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 1U);
        CHECK(UmiLearningCatalogueGtk4PresentShared(parent, host) == first);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == 1U);
    }
    CHECK(Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &before) == UMI_STATUS_OK);
    CHECK(before.last_status == UMI_STATUS_OK && before.pack_count == 1U);
    CHECK(UmiLearningCatalogueGtk4Open(panel, 0U) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4Open(panel, 1U) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4Back(panel) == UMI_STATUS_OK && Drain());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &before) == UMI_STATUS_OK);
    CHECK(before.reader.history_count == 2U && before.reader.history_index == 0U);
    if (strcmp(argv[1], "reuse") == 0 || strcmp(argv[1], "pending-reuse") == 0) {
        CHECK(UmiLearningCatalogueGtk4PresentShared(parent, host) == first);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
        CHECK(after.pack_count == before.pack_count && after.reader.revision == before.reader.revision);
        CHECK(after.reader.history_count == 2U && after.reader.can_go_forward);
    } else if (strcmp(argv[1], "host-version") == 0) {
        CHECK(UmiLearningCatalogueGtk4PresentShared(parent, (UmiVersion){0U, 9U, 1U}) == NULL);
        CHECK(UmiLearningCatalogueGtk4PresentShared(parent, host) == first);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK);
        CHECK(after.reader.revision == before.reader.revision && !after.retired);
    } else if (strcmp(argv[1], "parent-separation") == 0) {
        other = GTK_WINDOW(gtk_application_window_new(application));
        g_object_ref(other);
        second = UmiLearningCatalogueGtk4PresentShared(other, host);
        CHECK(second != NULL);
        g_object_ref(second);
        CHECK(second != first && gtk_window_get_transient_for(second) == other);
        CHECK(UmiLearningCatalogueGtk4CopyState(gtk_window_get_child(second), &after) == UMI_STATUS_OK);
        CHECK(after.pack_count == 0U && !after.reader.has_document);
        CHECK(UmiLearningCatalogueGtk4PresentShared(parent, host) == first);
    } else if (strcmp(argv[1], "independent-present") == 0) {
        second = UmiLearningCatalogueGtk4Present(parent, host);
        CHECK(second != NULL);
        g_object_ref(second);
        CHECK(second != first && UmiLearningCatalogueGtk4PresentShared(parent, host) == first);
    } else if (strcmp(argv[1], "retired-replacement") == 0 ||
        strcmp(argv[1], "closed-retained") == 0) {
        if (strcmp(argv[1], "retired-replacement") == 0) UmiLearningCatalogueGtk4Retire(panel);
        else gtk_window_destroy(first);
        second = UmiLearningCatalogueGtk4PresentShared(parent, host);
        CHECK(second != NULL);
        g_object_ref(second);
        CHECK(second != first && !Alive(first));
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &after) == UMI_STATUS_OK && after.retired);
        CHECK(UmiLearningCatalogueGtk4CopyState(gtk_window_get_child(second), &after) == UMI_STATUS_OK);
        CHECK(!after.retired && after.pack_count == 0U && !after.reader.has_document);
    } else CHECK(0 && "Unknown shared-window test");
cleanup:
    if (callback != 0U) g_signal_handler_disconnect(application, callback);
    Close(first); Close(second); Close(parent); Close(other);
    if (!Drain()) failed = 1;
    g_clear_object(&panel);
    g_clear_object(&first); g_clear_object(&second);
    g_clear_object(&parent); g_clear_object(&other);
    g_clear_object(&application);
    if (error != NULL) fprintf(stderr, "%s\n", error->message);
    g_clear_error(&error);
    return failed;
}
