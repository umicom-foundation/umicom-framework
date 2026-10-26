/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_window_owner.c
 *
 * PURPOSE:
 *   Check native window destruction and lease cleanup after the host releases every reference during file work.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../build_log/fixture.h"
#include "../kanban/fixture.h"
#include "../support/main_context.h"
#include "umicom/ui/gtk4/kanban_window.h"
#include "umicom/kanban/document.h"
typedef struct OwnerObservation { GThread *thread; unsigned finalized; } OwnerObservation;
static void Finalized(gpointer data, GObject *object)
{
    (void)object; OwnerObservation *owner = data;
    REQUIRE(g_thread_self() == owner->thread); ++owner->finalized;
}
static GtkWidget *Find(GtkWidget *root, const char *text)
{
    if (!strcmp(gtk_widget_get_name(root), text) ||
        (GTK_IS_BUTTON(root) && !g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), text))) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, text); if (found) return found;
    }
    return NULL;
}
static void Click(GtkWidget *root, const char *label)
{
    GtkWidget *button = Find(root, label); REQUIRE(GTK_IS_BUTTON(button));
    g_signal_emit_by_name(button, "clicked");
}
static void Field(GtkWidget *root, const char *name, const char *value)
{
    GtkWidget *entry = Find(root, name); REQUIRE(GTK_IS_EDITABLE(entry));
    gtk_editable_set_text(GTK_EDITABLE(entry), value);
}
static void Wait(GtkWindow *window)
{
    gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
    while (UmiKanbanGtk4WindowBusy(window) && g_get_monotonic_time() < end) {
        (void)UmiTestDispatchSlice(NULL, end, 32U); g_usleep(1000U);
    }
    REQUIRE(!UmiKanbanGtk4WindowBusy(window));
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2);
    bool create = !strcmp(argv[1], "create"), open = !strcmp(argv[1], "open");
    bool save = !strcmp(argv[1], "save"), missing = !strcmp(argv[1], "missing");
    REQUIRE(create || open || save || missing);
    if (!gtk_init_check()) return 77;
    char directory[UMI_PATH_CAPACITY]; FixtureDirectory(directory);
    if (open) {
        UmiKanbanBoard *board = Board(); UmiKanbanConfig config;
        REQUIRE(umi_kanban_board_config(board, &config) == UMI_STATUS_OK);
        UmiKanbanDocument *document = NULL;
        REQUIRE(umi_kanban_document_create(directory, "board.umikb", &config, &document) == UMI_STATUS_OK);
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
        umi_kanban_board_destroy(board);
    }
    GtkApplication *application = gtk_application_new("org.umicom.KanbanOwnerTest", G_APPLICATION_NON_UNIQUE);
    REQUIRE(g_application_register(G_APPLICATION(application), NULL, NULL));
    GtkWindow *window = UmiKanbanGtk4CreateWindow(application); REQUIRE(window); g_object_ref(window);
    OwnerObservation owner = {g_thread_self(), 0U};
    g_object_weak_ref(G_OBJECT(window), Finalized, &owner);
    GtkWidget *root = GTK_WIDGET(window);
    Field(root, "kanban-document-directory", directory);
    Field(root, "kanban-document-filename", "board.umikb");
    if (save) {
        Click(root, "Create board"); Wait(window);
        Field(root, "kanban-title", "Accepted save survives host removal");
        Click(root, "Add card");
    }
    Click(root, save ? "Save board" : create ? "Create board" : "Open board");
    REQUIRE(UmiKanbanGtk4WindowBusy(window));
    /* Force host removal to model application teardown, not a normal close
     * request. The admitted operation must complete and release its lease. */
    gtk_window_destroy(window); g_object_unref(window); window = NULL; root = NULL;
    gint64 end = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
    while (!owner.finalized && g_get_monotonic_time() < end) {
        (void)UmiTestDispatchSlice(NULL, end, 32U); g_usleep(1000U);
    }
    REQUIRE(owner.finalized == 1U);
    UmiKanbanDocument *document = NULL;
    UmiStatus status = umi_kanban_document_open(directory, "board.umikb", &document);
    if (missing) REQUIRE(status == UMI_STATUS_NOT_FOUND && document == NULL);
    else {
        REQUIRE(status == UMI_STATUS_OK);
        REQUIRE(umi_kanban_board_count(umi_kanban_document_board(document)) == (save ? 1U : 0U));
        REQUIRE(umi_kanban_document_close(&document, false) == UMI_STATUS_OK);
    }
    g_object_unref(application);
    return 0;
}
