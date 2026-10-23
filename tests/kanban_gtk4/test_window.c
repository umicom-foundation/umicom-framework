/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_window.c
 * PURPOSE: Exercise native board persistence, dirty-close refusal and save-copy conflict recovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/kanban_window.h"
#include "umicom/kanban/document.h"
#include "umicom/kanban/developers.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(c) do { if (!(c)) { fprintf(stderr, "%d: %s\n", __LINE__, #c); exit(1); } } while (0)
static GtkWidget *Find(GtkWidget *root, const char *text, bool by_name)
{
    if (by_name ? strcmp(gtk_widget_get_name(root), text) == 0 :
        GTK_IS_BUTTON(root) && g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), text) == 0) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, text, by_name); if (found != NULL) return found;
    }
    return NULL;
}
static void Set(GtkWidget *root, const char *name, const char *text)
{ GtkWidget *entry = Find(root, name, true); REQUIRE(GTK_IS_EDITABLE(entry)); gtk_editable_set_text(GTK_EDITABLE(entry), text); }
static void Click(GtkWidget *root, const char *label)
{ GtkWidget *button = Find(root, label, false); REQUIRE(button != NULL); g_signal_emit_by_name(button, "clicked"); }
static void Wait(GtkWidget *root)
{
    GtkWidget *controls = Find(root, "kanban-document-controls", true); REQUIRE(controls != NULL);
    gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (!gtk_widget_get_sensitive(controls) && g_get_monotonic_time() < deadline) {
        for (unsigned i = 0U; i < 32U && g_main_context_pending(NULL); ++i) (void)g_main_context_iteration(NULL, FALSE);
        g_usleep(1000U);
    }
    REQUIRE(gtk_widget_get_sensitive(controls));
}
typedef struct Reentry { GtkWidget *root; unsigned attempts; } Reentry;
static void Reenter(GObject *controls, GParamSpec *property, gpointer data)
{
    (void)property; Reentry *entry = data;
    if (gtk_widget_get_sensitive(GTK_WIDGET(controls))) {
        ++entry->attempts; Click(entry->root, "Close board");
    }
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 3);
    if (!gtk_init_check()) { fprintf(stderr, "GTK display unavailable; native document coverage skipped.\n"); return 77; }
    char *directory = g_build_filename(argv[2], "kanban-window-XXXXXX", NULL); REQUIRE(g_mkdtemp(directory) != NULL);
    GtkApplication *application = gtk_application_new("org.umicom.KanbanWindowTest", G_APPLICATION_NON_UNIQUE);
    GError *error = NULL; REQUIRE(g_application_register(G_APPLICATION(application), NULL, &error));
    GtkWindow *window = UmiKanbanGtk4CreateWindow(application); REQUIRE(window != NULL); g_object_ref(window);
    GtkWidget *root = GTK_WIDGET(window);
    Set(root, "kanban-document-directory", directory); Set(root, "kanban-document-filename", "board.umikb");
    char *filename = g_build_filename(directory, "board.umikb", NULL);
    if (strcmp(argv[1], "existing") == 0) {
        REQUIRE(g_file_set_contents(filename, "keep existing file", -1, NULL));
        Click(root, "Create board"); Wait(root);
        char *contents = NULL; REQUIRE(g_file_get_contents(filename, &contents, NULL, NULL));
        REQUIRE(strcmp(contents, "keep existing file") == 0); g_free(contents);
        REQUIRE(Find(root, "kanban-title", true) == NULL);
    } else if (strcmp(argv[1], "retained") == 0) {
        GtkWidget *button = Find(root, "Create board", false); REQUIRE(button != NULL); g_object_ref(button);
        gtk_window_destroy(window); g_signal_emit_by_name(button, "clicked");
        REQUIRE(!g_file_test(filename, G_FILE_TEST_EXISTS)); g_object_unref(button);
    } else {
        Reentry reentry = {root, 0U}; gulong handler = 0U;
        GtkWidget *controls = Find(root, "kanban-document-controls", true);
        if (strcmp(argv[1], "completion-reenter") == 0)
            handler = g_signal_connect(controls, "notify::sensitive", G_CALLBACK(Reenter), &reentry);
        Click(root, "Create board"); Wait(root); REQUIRE(Find(root, "kanban-title", true) != NULL);
        if (handler != 0U) { g_signal_handler_disconnect(controls, handler); REQUIRE(reentry.attempts == 1U); }
        Set(root, "kanban-title", "Saved native card"); Click(root, "Add card");
        Set(root, "kanban-developer", "Alex Morgan"); Click(root, "Assign name");
        if (strcmp(argv[1], "dirty-close") == 0) {
            gboolean refused = FALSE; g_signal_emit_by_name(window, "close-request", &refused); REQUIRE(refused);
            GtkWidget *status = Find(root, "kanban-document-status", true); REQUIRE(GTK_IS_LABEL(status));
            REQUIRE(strstr(gtk_label_get_text(GTK_LABEL(status)), "unsaved changes") != NULL);
        } else REQUIRE(strcmp(argv[1], "roundtrip") == 0 || strcmp(argv[1], "conflict-copy") == 0 || strcmp(argv[1], "completion-reenter") == 0);
        if (strcmp(argv[1], "conflict-copy") == 0) {
            REQUIRE(g_file_set_contents(filename, "external writer", -1, NULL));
            Click(root, "Save board"); Wait(root);
            GtkWidget *status = Find(root, "kanban-document-status", true);
            REQUIRE(strstr(gtk_label_get_text(GTK_LABEL(status)), "Operation refused") != NULL);
            Set(root, "kanban-document-filename", "recovered.umikb"); Click(root, "Save copy to new file"); Wait(root);
            char *contents = NULL; REQUIRE(g_file_get_contents(filename, &contents, NULL, NULL));
            REQUIRE(strcmp(contents, "external writer") == 0); g_free(contents);
            gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(root, "kanban-document-discard", true)), TRUE);
        } else { Click(root, "Save board"); Wait(root); }
        Click(root, "Close board"); Wait(root); REQUIRE(Find(root, "kanban-title", true) == NULL);
        UmiKanbanDocument *document = NULL;
        REQUIRE(umi_kanban_document_open(directory, strcmp(argv[1], "conflict-copy") == 0 ? "recovered.umikb" : "board.umikb", &document) == UMI_STATUS_OK);
        UmiKanbanBoard *board = umi_kanban_document_board(document);
        REQUIRE(umi_kanban_board_count(board) == 1U && umi_kanban_board_developer_count(board) == 1U);
        UmiKanbanDeveloper person;
        REQUIRE(umi_kanban_board_developer_at(board, umi_kanban_board_revision(board), 0U, &person) == UMI_STATUS_OK);
        REQUIRE(strcmp(person.name, "Alex Morgan") == 0);
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
    }
    gtk_window_destroy(window); g_object_unref(window); g_object_unref(application);
    /* Only names inside this freshly created qualification directory are removed.
     * A failed REQUIRE exits earlier, retaining evidence for diagnosis. */
    const char *files[] = {"board.umikb", "board.umikb.lock", "recovered.umikb", "recovered.umikb.lock"};
    for (size_t i = 0U; i < sizeof(files) / sizeof(files[0]); ++i) {
        char *file = g_build_filename(directory, files[i], NULL); (void)g_remove(file); g_free(file);
    }
    (void)g_rmdir(directory); g_free(filename); g_free(directory); return 0;
}
