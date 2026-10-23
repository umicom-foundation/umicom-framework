/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_retirement.c
 * PURPOSE: Verify forced native removal releases leases without resurrecting board controls.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/kanban_window.h"
#include "umicom/kanban/document.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); exit(1); } } while (0)
static GtkWidget *find(GtkWidget *root, const char *text)
{
    if (!strcmp(gtk_widget_get_name(root), text) ||
        (GTK_IS_BUTTON(root) && !g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), text))) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = find(child, text); if (match) return match;
    }
    return NULL;
}
static void click(GtkWidget *root, const char *label)
{
    GtkWidget *button = find(root, label); REQUIRE(button);
    g_signal_emit_by_name(button, "clicked");
}
static void set(GtkWidget *root, const char *name, const char *value)
{
    GtkWidget *entry = find(root, name); REQUIRE(GTK_IS_EDITABLE(entry));
    gtk_editable_set_text(GTK_EDITABLE(entry), value);
}
static void wait_idle(GtkWindow *window)
{
    gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (UmiKanbanGtk4WindowBusy(window) && g_get_monotonic_time() < deadline) {
        (void)g_main_context_iteration(NULL, FALSE); g_usleep(1000U);
    }
    REQUIRE(!UmiKanbanGtk4WindowBusy(window));
}
typedef struct Removal {
    GtkWindow *window;
    bool onCompletion, fired;
    unsigned lateStatus;
} Removal;
static void remove_window(GObject *object, GParamSpec *property, gpointer data)
{
    (void)property;
    Removal *removal = data;
    if (!removal->fired && (bool)gtk_widget_get_sensitive(GTK_WIDGET(object)) == removal->onCompletion) {
        removal->fired = true;
        gtk_window_destroy(removal->window);
    }
}
static void status_changed(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    Removal *removal = data;
    if (removal->fired) ++removal->lateStatus;
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 3);
    if (!gtk_init_check()) return 77;
    char *directory = g_build_filename(argv[2], "retirement-XXXXXX", NULL);
    REQUIRE(g_mkdtemp(directory));
    GtkApplication *app = gtk_application_new("org.umicom.KanbanRemovalTest", G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(app), NULL, NULL));
    GtkWindow *window = UmiKanbanGtk4CreateWindow(app); REQUIRE(window); g_object_ref(window);
    GtkWidget *root = GTK_WIDGET(window);
    set(root, "kanban-document-directory", directory);
    set(root, "kanban-document-filename", "board.umikb");
    GtkWidget *controls = g_object_ref(find(root, "kanban-document-controls"));
    GtkWidget *status = g_object_ref(find(root, "kanban-document-status"));
    GtkWidget *body = g_object_ref(gtk_widget_get_parent(controls));
    GtkWidget *retained = g_object_ref(find(root, "Create board"));
    char *file = g_build_filename(directory, "board.umikb", NULL);
    bool before = !strcmp(argv[1], "prepare");
    bool completion = !strcmp(argv[1], "complete");
    bool during = !strcmp(argv[1], "pending");
    bool idle = !strcmp(argv[1], "idle-dirty");
    bool saving = !strcmp(argv[1], "save-pending");
    REQUIRE(before || completion || during || idle || saving);
    Removal removal = {window, completion, false, 0U};
    gulong changed = g_signal_connect(status, "notify::label", G_CALLBACK(status_changed), &removal);
    gulong observing = 0U;
    if (before || completion)
        observing = g_signal_connect(controls, "notify::sensitive", G_CALLBACK(remove_window), &removal);
    click(root, "Create board");
    if (during) { removal.fired = true; gtk_window_destroy(window); }
    wait_idle(window);
    if (idle || saving) {
        set(root, "kanban-title", "Unsaved card");
        click(root, "Add card");
        if (saving) click(root, "Save board");
        removal.fired = true;
        gtk_window_destroy(window);
        wait_idle(window);
    }
    REQUIRE(removal.fired && removal.lateStatus == 0U);
    REQUIRE(find(body, "kanban-title") == NULL);
    g_signal_emit_by_name(retained, "clicked");
    REQUIRE(!UmiKanbanGtk4WindowBusy(window));
    if (before) REQUIRE(!g_file_test(file, G_FILE_TEST_EXISTS));
    else {
        /* Reopening while the destroyed window is still referenced proves the
         * lease was retired, rather than deferred until object finalisation. */
        UmiKanbanDocument *document = NULL;
        REQUIRE(umi_kanban_document_open(directory, "board.umikb", &document) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_count(umi_kanban_document_board(document)) == (saving ? 1U : 0U));
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
    }
    if (observing) g_signal_handler_disconnect(controls, observing);
    g_signal_handler_disconnect(status, changed);
    g_object_unref(retained); g_object_unref(body); g_object_unref(status); g_object_unref(controls);
    g_object_unref(window); g_object_unref(app);
    /* Remove only fixture names in the unique directory created above. */
    const char *names[] = {"board.umikb", "board.umikb.lock"};
    for (size_t i = 0U; i < sizeof(names) / sizeof(names[0]); ++i) {
        char *name = g_build_filename(directory, names[i], NULL);
        (void)g_remove(name); g_free(name);
    }
    (void)g_rmdir(directory); g_free(directory); g_free(file);
    return 0;
}
