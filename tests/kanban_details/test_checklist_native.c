/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_checklist_native.c
 *
 * PURPOSE:
 *   Exercise acceptance rows, retained drafts and interrupted native checklist loads.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
typedef struct Observer { GtkWidget *panel, *form; const char *mode; unsigned calls; } Observer;
static void Observe(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; Observer *o = data;
    if (++o->calls != 1U) return;
    if (!strcmp(o->mode, "load-retire")) UmiKanbanGtk4Retire(o->panel);
    else if (!strcmp(o->mode, "load-hide")) {
        gtk_widget_set_visible(o->form, FALSE); gtk_widget_set_visible(o->form, TRUE);
    } else Click(o->panel, "Apply checklist");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.description, "Private description retained");
    edit->card.check_count = 1U; strcpy(edit->card.checks[0].text, "Existing item"); edit->card.checks[0].complete = 1;
    Apply(board, edit, 1U); Create(edit, "other"); Apply(board, edit, 1U);
    if (!strcmp(name, "done")) Done(board, edit, "card");
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-card-checklist-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-card-checklist"); REQUIRE(gtk_widget_get_mapped(form));
    uint64_t revision = umi_kanban_board_revision(board);
    if (strcmp(name, "no-selection")) Click(panel, "card");
    Observer observer = {panel, form, name, 0U}; gulong handler = 0U;
    GtkWidget *status = Find(panel, "kanban-checklist-status");
    if (!strcmp(name, "load-retire") || !strcmp(name, "load-hide"))
        handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &observer);
    Click(panel, "Load selected checklist");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); handler = 0U; }
    Set(panel, "kanban-checklist-text-0", !strcmp(name, "clear") ? "" : "Reviewed acceptance");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(panel, "kanban-checklist-complete-0")), TRUE);
    if (strcmp(name, "clear")) Set(panel, "kanban-checklist-text-2", "Pending acceptance");
    if (!strcmp(name, "stale")) {
        Create(edit, "external"); Apply(board, edit, 1U); revision++;
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    } else if (!strcmp(name, "selection-roundtrip")) { Click(panel, "other"); Click(panel, "card"); }
    else if (!strcmp(name, "busy")) UmiKanbanGtk4SetBusy(panel, true);
    else if (!strcmp(name, "hidden")) gtk_widget_set_visible(form, FALSE);
    else if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(panel);
    else if (!strcmp(name, "reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &observer);
    Click(panel, "Apply checklist");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); }
    bool accepted = !strcmp(name, "normal") || !strcmp(name, "clear") || !strcmp(name, "reentry");
    REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
    UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
    REQUIRE(!strcmp(row.card.description, "Private description retained"));
    if (accepted) {
        REQUIRE(row.card.check_count == (!strcmp(name, "clear") ? 0U : 2U));
        if (row.card.check_count) REQUIRE(!strcmp(row.card.checks[0].text, "Reviewed acceptance") &&
            row.card.checks[0].complete == 1 && !strcmp(row.card.checks[1].text, "Pending acceptance") &&
            row.card.checks[1].complete == 0);
        Click(panel, "Apply checklist"); REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
    } else REQUIRE(row.card.check_count == 1U && !strcmp(row.card.checks[0].text, "Existing item"));
    UmiKanbanGtk4Retire(panel); gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
