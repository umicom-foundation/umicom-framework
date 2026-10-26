/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_dependencies_native.c
 *
 * PURPOSE:
 *   Exercise explicit prerequisite selection and stale native relationship refusal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
#include "umicom/kanban/card_details.h"
static void Choose(GtkWidget *panel, const char *name, const char *id)
{
    GtkDropDown *select = GTK_DROP_DOWN(Find(panel, name));
    GListModel *model = gtk_drop_down_get_model(select);
    char suffix[80]; REQUIRE(snprintf(suffix, sizeof suffix, "[%s]", id) > 0);
    guint count = g_list_model_get_n_items(model);
    for (guint i = 1U; i < count; ++i) {
        GtkStringObject *item = g_list_model_get_item(model, i);
        bool match = g_str_has_suffix(gtk_string_object_get_string(item), suffix);
        g_object_unref(item);
        if (match) { gtk_drop_down_set_selected(select, i); return; }
    }
    REQUIRE(0);
}
typedef struct Observer { GtkWidget *panel, *form; const char *mode; unsigned calls; } Observer;
static void Observe(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; Observer *o = data;
    if (++o->calls != 1U) return;
    if (!strcmp(o->mode, "load-retire")) UmiKanbanGtk4Retire(o->panel);
    else if (!strcmp(o->mode, "load-hide")) {
        gtk_widget_set_visible(o->form, FALSE); gtk_widget_set_visible(o->form, TRUE);
    } else Click(o->panel, "Add prerequisite");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.description, "Private description retained"); Apply(board, edit, 1U);
    Create(edit, "required"); Apply(board, edit, 1U); Create(edit, "other"); Apply(board, edit, 1U);
    bool remove = !strcmp(name, "remove");
    if (remove) REQUIRE(UmiKanbanCardDependencyAdd(board, umi_kanban_board_revision(board), "card", "required", "owner") == UMI_STATUS_OK);
    if (!strcmp(name, "cycle"))
        REQUIRE(UmiKanbanCardDependencyAdd(board, umi_kanban_board_revision(board), "required", "card", "owner") == UMI_STATUS_OK);
    if (!strcmp(name, "done")) Done(board, edit, "card");
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-card-dependencies-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-card-dependencies"); REQUIRE(gtk_widget_get_mapped(form));
    uint64_t revision = umi_kanban_board_revision(board);
    if (strcmp(name, "no-selection")) Click(panel, "card");
    Observer observer = {panel, form, name, 0U}; gulong handler = 0U;
    GtkWidget *status = Find(panel, "kanban-dependency-status");
    if (!strcmp(name, "load-retire") || !strcmp(name, "load-hide"))
        handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &observer);
    Click(panel, "Load selected prerequisites");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); handler = 0U; }
    if (strcmp(name, "placeholder") && strcmp(name, "no-selection"))
        Choose(panel, remove ? "kanban-dependency-remove" : "kanban-dependency-add", "required");
    if (!strcmp(name, "stale")) {
        Create(edit, "external"); Apply(board, edit, 1U); revision++;
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    } else if (!strcmp(name, "selection-roundtrip")) { Click(panel, "other"); Click(panel, "card"); }
    else if (!strcmp(name, "hidden")) gtk_widget_set_visible(form, FALSE);
    else if (!strcmp(name, "busy")) UmiKanbanGtk4SetBusy(panel, true);
    else if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(panel);
    else if (!strcmp(name, "reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &observer);
    Click(panel, remove ? "Remove prerequisite" : "Add prerequisite");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); }
    bool accepted = !strcmp(name, "normal") || remove || !strcmp(name, "reentry");
    REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
    UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
    REQUIRE(!strcmp(row.card.description, "Private description retained"));
    REQUIRE(row.card.dependency_count == (accepted && !remove ? 1U : 0U));
    if (accepted && !remove) REQUIRE(!strcmp(row.card.dependencies[0], "required"));
    UmiKanbanGtk4Retire(panel); gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
