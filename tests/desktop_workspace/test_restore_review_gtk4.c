/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_workspace/test_restore_review_gtk4.c
 * PURPOSE: Exercise the real checkpoint review window with private temporary storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/desktop_workspace/gtk4.h"
#include "umicom/desktop_workspace/workspace.h"
#include "umicom/test_runtime/check.h"
#include <glib/gstdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) UMI_TEST_REQUIRE(value)
#define OK(value) CHECK((value) == UMI_STATUS_OK)

static GtkWidget *Find(GtkWidget *root, const char *id)
{
    if (!strcmp(gtk_widget_get_name(root), id) ||
        !g_strcmp0(g_object_get_data(G_OBJECT(root), "umicom-automation-id"), id)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, id); if (found) return found;
    }
    return NULL;
}
static int Wait(GtkWindow *window)
{
    const gint64 deadline = g_get_monotonic_time() + 10000000;
    while (g_object_get_data(G_OBJECT(window), "umicom.desktop-workspace.busy")) {
        while (g_main_context_iteration(NULL, FALSE)) {}
        if (g_get_monotonic_time() >= deadline) return 0;
        g_usleep(1000U);
    }
    return 1;
}
static GtkWindow *ReviewWindow(void)
{
    GListModel *windows = gtk_window_get_toplevels();
    for (guint index = 0U; index < g_list_model_get_n_items(windows); ++index) {
        GtkWindow *window = g_list_model_get_item(windows, index);
        if (!strcmp(gtk_widget_get_name(GTK_WIDGET(window)), "umicom.workspace.restore-review")) return window;
        g_object_unref(window);
    }
    return NULL;
}
static int Contains(GtkWidget *view, const char *text)
{
    if (!GTK_IS_TEXT_VIEW(view) || gtk_text_view_get_editable(GTK_TEXT_VIEW(view))) return 0;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
    GtkTextIter first, last; gtk_text_buffer_get_bounds(buffer, &first, &last);
    char *contents = gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
    const int found = strstr(contents, text) != NULL; g_free(contents); return found;
}
int main(int argc, char **argv)
{
    const char *cases[] = {"evidence", "apply", "cancel", "retained-control", "parent-close", "dirty-draft", "detached-review", "history-selection", "history-draft", "history-close"};
    if (argc != 2) return 2;
    int known = 0;
    for (size_t index = 0U; index < sizeof cases / sizeof cases[0]; ++index)
        if (!strcmp(argv[1], cases[index])) known = 1;
    if (!known) return 2;
    if (!gtk_init_check()) return 77;
    GError *error = NULL;
    char *parent = g_dir_make_tmp("umicom-restore-review-XXXXXX", &error);
    CHECK(parent != NULL); g_clear_error(&error);
    char *directory = g_build_filename(parent, "workspace", NULL);
    UmiDesktopWorkspace *workspace = NULL;
    UmiStatus status = UmiDesktopWorkspaceOpenDirectory(directory, &workspace);
    if (status == UMI_STATUS_UNAVAILABLE) { g_free(directory); g_free(parent); return 77; }
    OK(status);
    UmiDesktopWorkspaceSnapshot *draft = malloc(sizeof(*draft)); CHECK(draft != NULL);
    OK(UmiDesktopWorkspaceRead(workspace, draft));
    OK(UmiDesktopWorkspacePutNote(draft, "note", "Workshop", "Earlier note café\nComplete second line."));
    draft->fontPoints = 17U; OK(UmiDesktopWorkspaceCommit(workspace, draft->revision, draft));
    OK(UmiDesktopWorkspaceRead(workspace, draft));
    OK(UmiDesktopWorkspacePutNote(draft, "note", "Revised workshop", "Current saved note"));
    draft->fontPoints = 20U; OK(UmiDesktopWorkspaceCommit(workspace, draft->revision, draft));
    OK(UmiDesktopWorkspaceCloseClean(workspace)); UmiDesktopWorkspaceDestroy(workspace); workspace = NULL;

    GtkWindow *window = UmiDesktopWorkspaceGtkCreate(NULL, directory); CHECK(window != NULL); g_object_ref(window);
    GtkWidget *open = Find(GTK_WIDGET(window), "umicom.workspace.open");
    GtkWidget *preview = Find(GTK_WIDGET(window), "umicom.workspace.preview");
    GtkWidget *checkpoint = Find(GTK_WIDGET(window), "umicom.workspace.checkpoint");
    CHECK(open && preview && checkpoint);
    g_signal_emit_by_name(open, "clicked"); CHECK(Wait(window));
    GtkWidget *history = Find(GTK_WIDGET(window), "umicom.workspace.history");
    GtkWidget *refresh = Find(GTK_WIDGET(window), "umicom.workspace.history-refresh");
    CHECK(GTK_IS_DROP_DOWN(history) && GTK_IS_BUTTON(refresh));
    CHECK(g_list_model_get_n_items(gtk_drop_down_get_model(GTK_DROP_DOWN(history))) == 3U);
    if (!strcmp(argv[1], "history-selection")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(history), 1U);
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(checkpoint)), "2"));
        g_signal_emit_by_name(refresh, "clicked"); CHECK(Wait(window));
        CHECK(gtk_drop_down_get_selected(GTK_DROP_DOWN(history)) == 1U);
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(checkpoint)), "2"));
    }
    if (!strcmp(argv[1], "history-draft") || !strcmp(argv[1], "history-close")) {
        GtkWidget *title = Find(GTK_WIDGET(window), "umicom.workspace.title"); CHECK(title != NULL);
        gtk_editable_set_text(GTK_EDITABLE(title), "Unsaved while refreshing");
        g_signal_emit_by_name(refresh, "clicked");
        if (!strcmp(argv[1], "history-close")) {
            gboolean handled = FALSE; g_signal_emit_by_name(window, "close-request", &handled); CHECK(handled);
        }
        CHECK(Wait(window));
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(title)), "Unsaved while refreshing"));
        CHECK(gtk_window_get_child(window) != NULL && !gtk_widget_get_sensitive(preview));
        if (!strcmp(argv[1], "history-close")) {
            /* A close during inspection still asks about the dirty draft. */
            GListModel *windows = gtk_window_get_toplevels(); CHECK(g_list_model_get_n_items(windows) == 2U);
            for (guint i = 0U; i < g_list_model_get_n_items(windows); ++i) {
                GtkWindow *child = g_list_model_get_item(windows, i);
                if (child != window) { gtk_window_destroy(child); g_object_unref(child); break; }
                g_object_unref(child);
            }
        }
        gtk_window_destroy(window); g_object_unref(window); free(draft); g_free(directory); g_free(parent); return 0;
    }
    gtk_editable_set_text(GTK_EDITABLE(checkpoint), "2");
    g_signal_emit_by_name(preview, "clicked"); CHECK(Wait(window));
    GtkWindow *review = ReviewWindow(); CHECK(review != NULL);
    CHECK(Contains(Find(GTK_WIDGET(review), "umicom.comparison.left"), "Current saved note"));
    CHECK(Contains(Find(GTK_WIDGET(review), "umicom.comparison.right"), "Earlier note café\nComplete second line."));
    CHECK(Contains(Find(GTK_WIDGET(review), "umicom.comparison.right"), "Font: 17 pt"));
    GtkWidget *apply = Find(GTK_WIDGET(review), "umicom.workspace.restore-apply");
    GtkWidget *cancel = Find(GTK_WIDGET(review), "umicom.workspace.restore-cancel");
    CHECK(apply && cancel); g_object_ref(apply);
    if (!strcmp(argv[1], "apply")) {
        g_signal_emit_by_name(apply, "clicked"); CHECK(Wait(window));
        /* A retained button cannot repeat a consumed restoration. */
        g_signal_emit_by_name(apply, "clicked"); CHECK(Wait(window));
    } else if (!strcmp(argv[1], "retained-control")) {
        gtk_window_destroy(review); g_signal_emit_by_name(apply, "clicked"); CHECK(Wait(window));
    } else if (!strcmp(argv[1], "detached-review")) {
        gtk_window_set_child(review, NULL); g_signal_emit_by_name(apply, "clicked"); CHECK(Wait(window));
    } else if (!strcmp(argv[1], "parent-close")) {
        gtk_window_destroy(window); g_signal_emit_by_name(apply, "clicked");
    } else if (!strcmp(argv[1], "dirty-draft")) {
        /* A programmatic editor change must be guarded even while the review
         * is modal. Normal pointer input is blocked by GTK's modal window. */
        GtkWidget *title = Find(GTK_WIDGET(window), "umicom.workspace.title"); CHECK(title != NULL);
        gtk_editable_set_text(GTK_EDITABLE(title), "Unsaved edit behind review");
        g_signal_emit_by_name(apply, "clicked"); CHECK(Wait(window));
        CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(title)), "Unsaved edit behind review"));
        g_signal_emit_by_name(cancel, "clicked");
    } else g_signal_emit_by_name(cancel, "clicked");

    gtk_window_destroy(review); g_object_unref(review); g_object_unref(apply);
    gtk_window_destroy(window); g_object_unref(window);
    while (g_main_context_iteration(NULL, FALSE)) {}
    OK(UmiDesktopWorkspaceOpenDirectory(directory, &workspace));
    OK(UmiDesktopWorkspaceRead(workspace, draft));
    const int applied = !strcmp(argv[1], "apply");
    CHECK(draft->revision == (applied ? 4U : 3U));
    CHECK(!strcmp(draft->notes[0].body, applied ? "Earlier note café\nComplete second line." : "Current saved note"));
    OK(UmiDesktopWorkspaceCloseClean(workspace)); UmiDesktopWorkspaceDestroy(workspace);
    free(draft); g_free(directory); g_free(parent); return EXIT_SUCCESS;
}
