/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_native.c
 *
 * PURPOSE:
 *   Exercise visible card detail drafts, revision checks and native retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../kanban/fixture.h"
#include "../support/main_context.h"
#include "umicom/ui/gtk4/kanban.h"
/* Earlier local helpers are retained for review; shared fixtures now keep
 * card-detail and checklist tests under the same dispatch and lookup rules. */
#if 0
static GtkWidget *Find(GtkWidget *root, const char *text)
{
    if (!strcmp(gtk_widget_get_name(root), text) ||
        (GTK_IS_BUTTON(root) && !g_strcmp0(gtk_button_get_label(GTK_BUTTON(root)), text))) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child; child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *found = Find(child, text); if (found) return found;
    }
    return NULL;
}
static void Click(GtkWidget *root, const char *text)
{ GtkWidget *button = Find(root, text); REQUIRE(GTK_IS_BUTTON(button)); g_signal_emit_by_name(button, "clicked"); }
static void Set(GtkWidget *root, const char *name, const char *value)
{ GtkWidget *entry = Find(root, name); REQUIRE(GTK_IS_EDITABLE(entry)); gtk_editable_set_text(GTK_EDITABLE(entry), value); }
static void Drain(void)
{
    for (unsigned i = 0U; i < 30U; ++i) {
        (void)UmiTestDispatchSlice(NULL, g_get_monotonic_time() + 1000, 32U); g_usleep(1000U);
    }
}
#endif
#include "native_fixture.h"
typedef struct Observer { GtkWidget *panel, *form; const char *mode; unsigned calls; } Observer;
static void Observe(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; Observer *o = data;
    if (++o->calls != 1U) return;
    if (!strcmp(o->mode, "load-retire")) UmiKanbanGtk4Retire(o->panel);
    else if (!strcmp(o->mode, "load-hide")) {
        gtk_widget_set_visible(o->form, FALSE); gtk_widget_set_visible(o->form, TRUE);
    } else Click(o->panel, "Apply card details");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.description, "Existing private description");
    strcpy(edit->card.assignee, "owner"); Apply(board, edit, 1U);
    Create(edit, "other"); Apply(board, edit, 1U);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-card-details-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-card-details"); REQUIRE(gtk_widget_get_mapped(form));
    uint64_t revision = umi_kanban_board_revision(board);
    Observer observer = {panel, form, name, 0U}; gulong handler = 0U;
    GtkWidget *status = Find(panel, "kanban-details-status");
    if (!strcmp(name, "load-retire") || !strcmp(name, "load-hide"))
        handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &observer);
    if (strcmp(name, "no-selection")) Click(panel, "card");
    Click(panel, "Load selected card details");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); handler = 0U; }
    Set(panel, "kanban-details-title", "Changed private title");
    Set(panel, "kanban-details-blocker", "Awaiting review");
    GtkWidget *text = Find(panel, "kanban-details-description"); REQUIRE(GTK_IS_TEXT_VIEW(text));
    const char *description = "Private details kept out of the public website";
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)), description, -1);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(panel, "kanban-details-priority")), (guint)UMI_KANBAN_URGENT);
    if (!strcmp(name, "stale")) {
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = row.card; strcpy(edit->card.title, "New external title");
        Apply(board, edit, 1U); REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK); revision++;
    } else if (!strcmp(name, "other-selection")) Click(panel, "other");
    else if (!strcmp(name, "selection-roundtrip")) { Click(panel, "other"); Click(panel, "card"); }
    else if (!strcmp(name, "hidden")) gtk_widget_set_visible(form, FALSE);
    else if (!strcmp(name, "busy")) UmiKanbanGtk4SetBusy(panel, true);
    else if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(panel);
    else if (!strcmp(name, "empty-title")) Set(panel, "kanban-details-title", "");
    else if (!strcmp(name, "oversized")) {
        char oversized[2049]; memset(oversized, 'x', sizeof oversized - 1U); oversized[2048] = '\0';
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)), oversized, -1);
    } else if (!strcmp(name, "reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &observer);
    Click(panel, "Apply card details");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); }
    bool accepted = !strcmp(name, "normal") || !strcmp(name, "reentry") || !strcmp(name, "replay");
    REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
    UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
    REQUIRE(!strcmp(row.card.assignee, "owner") && !strcmp(row.column_id, "todo"));
    if (accepted) {
        REQUIRE(!strcmp(row.card.title, "Changed private title") && !strcmp(row.card.description, description));
        REQUIRE(row.card.priority == UMI_KANBAN_URGENT && !strcmp(row.card.blocked_reason, "Awaiting review"));
        Click(panel, "Apply card details"); REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
    } else REQUIRE(!strcmp(row.card.title, !strcmp(name, "stale") ? "New external title" : "card"));
    UmiKanbanGtk4Retire(panel); gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
