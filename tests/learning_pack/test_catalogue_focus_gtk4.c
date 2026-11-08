/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/learning_pack/test_catalogue_focus_gtk4.c
 *
 * PURPOSE:
 *   Verify native focus ownership before catalogue loading and retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/learning_catalogue.h"
#include "umicom/ui/gtk4/automation.h"
#include "../support/main_context.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
    failed = 1; goto cleanup; } } while (0)

typedef struct FocusObserver {
    GtkWidget *panel;
    const char *directory;
    unsigned calls;
    int retire;
    UmiStatus reentry_status;
} FocusObserver;

static void FocusChanged(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)spec;
    FocusObserver *observer = data;
    if (gtk_root_get_focus(GTK_ROOT(object)) != NULL || observer->calls != 0U) return;
    ++observer->calls;
    /* Native focus notifications are synchronous. A nested load must see busy;
     * retirement must be visible before the outer call can schedule its worker. */
    observer->reentry_status = UmiLearningCatalogueGtk4Load(observer->panel, observer->directory);
    if (observer->retire) UmiLearningCatalogueGtk4Retire(observer->panel);
}
static int FinishReads(void)
{
    gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiLearningCatalogueGtk4PendingReads() != 0U && g_get_monotonic_time() < deadline) {
        UmiTestDispatchSlice(NULL, deadline, 64U);
        g_usleep(1000U);
    }
    return UmiLearningCatalogueGtk4PendingReads() == 0U;
}
int main(int argc, char **argv)
{
    if (argc != 2) return 2;
    g_test_init(&argc, &argv, NULL);
    if (!gtk_init_check()) return 77;
    const char *mode = argv[1];
    int retire = !strcmp(mode, "retire") || !strcmp(mode, "retire-reader") ||
        !strcmp(mode, "unrelated-retire");
    int unrelated = !strcmp(mode, "unrelated-load") || !strcmp(mode, "unrelated-retire");
    int reenter = !strcmp(mode, "reentrant-load") || !strcmp(mode, "reentrant-retire");
    int unrooted = !strcmp(mode, "unrooted");
    if (!retire && !unrelated && !reenter && !unrooted && strcmp(mode, "load")) return 2;
    int failed = 0;
    GtkWidget *panel = NULL, *focus = NULL;
    GtkWindow *window = NULL;
    gulong handler = 0U;
    GError *error = NULL;
    /* An existing but empty library makes failure deterministic without relying
     * on an installation, a current working folder or previously generated data. */
    char *directory = g_dir_make_tmp("umicom-catalogue-focus-XXXXXX", &error);
    CHECK(directory != NULL);
    panel = UmiLearningCatalogueGtk4Create((UmiVersion){0U, 9U, 0U});
    CHECK(panel != NULL);
    g_object_ref_sink(panel);
    FocusObserver observer = {panel, directory, 0U, !strcmp(mode, "reentrant-retire"), UMI_STATUS_OK};
    if (!unrooted) {
        window = GTK_WINDOW(gtk_window_new());
        g_object_ref(window);
        GtkWidget *layout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        GtkWidget *outside = gtk_entry_new();
        gtk_box_append(GTK_BOX(layout), panel);
        gtk_box_append(GTK_BOX(layout), outside);
        gtk_window_set_child(window, layout);
        gtk_window_set_default_size(window, 900, 900);
        gtk_window_present(window);
        UmiTestDispatchSlice(NULL, g_get_monotonic_time() + 50000, 32U);
        GtkWidget *target = unrelated ? outside : umi_gtk4_automation_find_tagged_widget(panel,
            !strcmp(mode, "retire-reader") ? "learning.catalogue.content" : "learning.catalogue.path");
        CHECK(target != NULL && gtk_widget_grab_focus(target));
        focus = gtk_root_get_focus(GTK_ROOT(window));
        CHECK(focus != NULL && (focus == target || gtk_widget_is_ancestor(focus, target)));
        g_object_ref(focus);
        if (reenter) handler = g_signal_connect(window, "notify::focus-widget",
            G_CALLBACK(FocusChanged), &observer);
    }
    UmiLearningCatalogueGtkState state;
    if (retire) {
        UmiLearningCatalogueGtk4Retire(panel);
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK && state.retired);
        CHECK(UmiLearningCatalogueGtk4Load(panel, directory) == UMI_STATUS_INVALID_STATE);
    } else {
        UmiStatus status = UmiLearningCatalogueGtk4Load(panel, directory);
        CHECK(status == (observer.retire ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK));
        CHECK(UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK);
        CHECK(state.busy == !observer.retire && state.retired == observer.retire);
        CHECK(UmiLearningCatalogueGtk4PendingReads() == (observer.retire ? 0U : 1U));
    }
    if (handler != 0U) { g_signal_handler_disconnect(window, handler); handler = 0U; }
    if (window != NULL)
        CHECK(gtk_root_get_focus(GTK_ROOT(window)) == (unrelated ? focus : NULL));
    if (reenter) CHECK(observer.calls == 1U && observer.reentry_status == UMI_STATUS_BUSY);
    CHECK(FinishReads());
    CHECK(UmiLearningCatalogueGtk4CopyState(panel, &state) == UMI_STATUS_OK);
    CHECK(!state.busy && state.pack_count == 0U && !state.reader.has_document);
    if (!retire && !observer.retire) CHECK(state.last_status == UMI_STATUS_NOT_FOUND);
cleanup:
    if (handler != 0U) g_signal_handler_disconnect(window, handler);
    if (panel != NULL) UmiLearningCatalogueGtk4Retire(panel);
    if (!FinishReads()) failed = 1;
    if (window != NULL) { gtk_window_set_focus(window, NULL); gtk_window_destroy(window); }
    g_clear_object(&focus);
    g_clear_object(&window);
    g_clear_object(&panel);
    if (directory != NULL && g_rmdir(directory) != 0) failed = 1;
    g_free(directory);
    if (error != NULL) fprintf(stderr, "%s\n", error->message);
    g_clear_error(&error);
    return failed;
}
