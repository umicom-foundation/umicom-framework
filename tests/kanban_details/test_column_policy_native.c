/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_column_policy_native.c
 *
 * PURPOSE:
 *   Exercise explicit list-limit review and stale or reentrant native controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
#include "umicom/kanban/column_policy.h"
typedef struct Observation { GtkWidget *panel, *form, *picker; const char *mode; unsigned calls; } Observation;
static void Observe(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; Observation *o = data;
    if (++o->calls != 1U) return;
    if (!strcmp(o->mode, "load-retire")) UmiKanbanGtk4Retire(o->panel);
    else if (!strcmp(o->mode, "load-hide")) {
        gtk_widget_set_visible(o->form, FALSE); gtk_widget_set_visible(o->form, TRUE);
    } else if (!strcmp(o->mode, "load-selection")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(o->picker), 1U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(o->picker), 0U);
    } else Click(o->panel, "Apply list limit");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"normal", "unlimited", "other-list", "reentry", "not-loaded", "stale", "selection-roundtrip", "busy", "hidden", "retired", "load-retire", "load-hide", "load-selection", "below", "negative", "fraction", "spaces", "over-capacity", "overflow", "blank"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized);
    if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "one"); Apply(board, edit, 1U); Create(edit, "two"); Apply(board, edit, 1U);
    if (!strcmp(name, "unlimited")) REQUIRE(UmiKanbanColumnPolicyApply(board,
        umi_kanban_board_revision(board), "todo", "owner", 2U) == UMI_STATUS_OK);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-column-policy-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-column-policy"); REQUIRE(gtk_widget_get_mapped(form));
    GtkWidget *picker = Find(panel, "kanban-policy-column"); REQUIRE(GTK_IS_DROP_DOWN(picker));
    GtkWidget *status = Find(panel, "kanban-policy-status"); REQUIRE(GTK_IS_LABEL(status));
    uint64_t revision = umi_kanban_board_revision(board);
    Observation o = {panel, form, picker, name, 0U}; gulong handler = 0U;
    if (!strcmp(name, "load-retire") || !strcmp(name, "load-hide") || !strcmp(name, "load-selection"))
        handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &o);
    if (strcmp(name, "not-loaded")) Click(panel, "Load list limit");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(o.calls > 0U); handler = 0U; }
    const char *value = !strcmp(name, "below") ? "1" : !strcmp(name, "unlimited") ? "0" :
        !strcmp(name, "negative") ? "-1" : !strcmp(name, "fraction") ? "2.5" :
        !strcmp(name, "spaces") ? " 2" : !strcmp(name, "over-capacity") ? "17" :
        !strcmp(name, "overflow") ? "9999999999" : !strcmp(name, "blank") ? "" : "2";
    Set(panel, "kanban-policy-limit", value);
    if (!strcmp(name, "stale")) {
        Create(edit, "external"); Apply(board, edit, 1U); ++revision;
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    } else if (!strcmp(name, "selection-roundtrip")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), 1U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), 0U);
    } else if (!strcmp(name, "other-list")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), 1U);
        Click(panel, "Load list limit"); Set(panel, "kanban-policy-limit", "1");
    } else if (!strcmp(name, "busy")) UmiKanbanGtk4SetBusy(panel, true);
    else if (!strcmp(name, "hidden")) gtk_widget_set_visible(form, FALSE);
    else if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(panel);
    else if (!strcmp(name, "reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &o);
    Click(panel, "Apply list limit");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(o.calls > 0U); }
    bool accepted = !strcmp(name, "normal") || !strcmp(name, "unlimited") || !strcmp(name, "reentry") || !strcmp(name, "other-list");
    REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
    UmiKanbanColumnPolicySnapshot result;
    REQUIRE(UmiKanbanColumnPolicyRead(board, umi_kanban_board_revision(board), "todo", &result) == UMI_STATUS_OK);
    REQUIRE(result.active_cards == (!strcmp(name, "stale") ? 3U : 2U));
    REQUIRE(result.column.wip_limit == ((!strcmp(name, "normal") || !strcmp(name, "reentry")) ? 2U : 0U));
    if (!strcmp(name, "other-list")) {
        REQUIRE(UmiKanbanColumnPolicyRead(board, umi_kanban_board_revision(board), "doing", &result) == UMI_STATUS_OK);
        REQUIRE(result.column.wip_limit == 1U && result.active_cards == 0U);
    }
    if (accepted) { Click(panel, "Apply list limit"); REQUIRE(umi_kanban_board_revision(board) == revision + 1U); }
    UmiKanbanGtk4Retire(panel); gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
