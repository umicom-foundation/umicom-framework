/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_gtk4/test_row_authority.c
 *
 * PURPOSE:
 *   Reject detached row actions and serialize native selection publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "../kanban_details/native_fixture.h"

static GObject *Controller(GtkWidget *widget, GType type)
{
    REQUIRE(widget); GListModel *model = gtk_widget_observe_controllers(widget); GObject *found = NULL;
    for (guint i = 0U; i < g_list_model_get_n_items(model); ++i) {
        GObject *item = g_list_model_get_item(model, i);
        if (G_TYPE_CHECK_INSTANCE_TYPE(item, type)) { found = item; break; }
        g_object_unref(item);
    }
    g_object_unref(model); REQUIRE(found); return found;
}
static GdkContentProvider *Prepare(GObject *source, GValue *value)
{
    GdkContentProvider *provider = NULL; g_signal_emit_by_name(source, "prepare", 0.0, 0.0, &provider);
    if (provider && value) {
        g_value_init(value, G_TYPE_STRING); GError *error = NULL;
        REQUIRE(gdk_content_provider_get_value(provider, value, &error) && !error);
    }
    return provider;
}
static bool DropValue(GObject *target, GValue *value)
{
    gboolean accepted = FALSE; g_signal_emit_by_name(target, "drop", value, 0.0, 0.0, &accepted); return accepted;
}
typedef struct SelectionObserver { GtkWidget *panel; UmiKanbanBoard **board; const char *mode; unsigned calls; } SelectionObserver;
static void SelectionChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; SelectionObserver *o = data;
    if (++o->calls != 1U) return;
    if (!strcmp(o->mode, "selection-retire")) {
        UmiKanbanGtk4Retire(o->panel); umi_kanban_board_destroy(*o->board); *o->board = NULL;
    } else if (!strcmp(o->mode, "selection-refresh")) {
        REQUIRE(UmiKanbanGtk4Refresh(o->panel) == UMI_STATUS_OK);
    } else {
        Click(o->panel, "two");
        Click(o->panel, "Add card");
        Click(o->panel, "Rename selected");
    }
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"retained-row", "retained-source", "detached-controller", "retained-target",
        "late-end", "refresh-cancel", "selection-reentry", "selection-feedback", "selection-refresh", "selection-retire"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized); if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "one"); Apply(board, edit, 1U); Create(edit, "two"); Apply(board, edit, 1U);
    uint64_t revision = umi_kanban_board_revision(board);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window); Drain();
    GtkWidget *row = Find(panel, "one"), *column = Find(panel, "doing"); REQUIRE(row && column);
    g_object_ref(row); g_object_ref(column);
    GObject *source = Controller(row, GTK_TYPE_DRAG_SOURCE), *target = Controller(column, GTK_TYPE_DROP_TARGET);
    if (!strncmp(name, "selection-", 10U)) {
        GtkWidget *signal = Find(panel, !strcmp(name, "selection-feedback") ? "kanban-status" : "kanban-title");
        SelectionObserver observer = {panel, &board, name, 0U};
        gulong handler = g_signal_connect(signal, !strcmp(name, "selection-feedback") ? "notify::label" : "notify::text",
            G_CALLBACK(SelectionChanged), &observer);
        Click(panel, "one"); g_signal_handler_disconnect(signal, handler); REQUIRE(observer.calls > 0U);
        if (board) {
            REQUIRE(umi_kanban_board_count(board) == 2U && umi_kanban_board_revision(board) == revision);
            Set(panel, "kanban-title", "Changed by explicit owner action"); Click(panel, "Rename selected");
            UmiKanbanCardSnapshot snapshot;
            REQUIRE(umi_kanban_board_find(board, "two", &snapshot) == UMI_STATUS_OK && !strcmp(snapshot.card.title, "two"));
            REQUIRE(umi_kanban_board_find(board, "one", &snapshot) == UMI_STATUS_OK);
            if (!strcmp(name, "selection-refresh")) {
                REQUIRE(!strcmp(snapshot.card.title, "one") && umi_kanban_board_revision(board) == revision);
            } else REQUIRE(!strcmp(snapshot.card.title, "Changed by explicit owner action") &&
                umi_kanban_board_revision(board) == revision + 1U);
        } else {
            Click(panel, "Add card"); REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_UNAVAILABLE);
        }
    } else if (!strcmp(name, "retained-row")) {
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
        Set(panel, "kanban-title", "Keep this draft"); g_signal_emit_by_name(row, "clicked");
        REQUIRE(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(panel, "kanban-title"))), "Keep this draft"));
        Click(panel, "Rename selected"); REQUIRE(umi_kanban_board_revision(board) == revision);
    } else if (!strcmp(name, "retained-source") || !strcmp(name, "detached-controller")) {
        if (!strcmp(name, "retained-source")) REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
        else gtk_widget_remove_controller(row, GTK_EVENT_CONTROLLER(source));
        GdkContentProvider *provider = Prepare(source, NULL); REQUIRE(provider == NULL);
        REQUIRE(umi_kanban_board_revision(board) == revision);
    } else {
        GValue value = G_VALUE_INIT; GdkContentProvider *provider = NULL;
        if (!strcmp(name, "refresh-cancel")) { provider = Prepare(source, &value); REQUIRE(provider); }
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
        GObject *fresh_source = Controller(Find(panel, "one"), GTK_TYPE_DRAG_SOURCE);
        GObject *fresh_target = Controller(Find(panel, "doing"), GTK_TYPE_DROP_TARGET);
        if (strcmp(name, "refresh-cancel")) { provider = Prepare(fresh_source, &value); REQUIRE(provider); }
        if (!strcmp(name, "retained-target")) REQUIRE(!DropValue(target, &value));
        if (!strcmp(name, "late-end")) g_signal_emit_by_name(source, "drag-end", NULL, TRUE);
        bool accepted = DropValue(fresh_target, &value);
        REQUIRE(accepted == (strcmp(name, "refresh-cancel") != 0));
        REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
        UmiKanbanCardSnapshot snapshot; REQUIRE(umi_kanban_board_find(board, "one", &snapshot) == UMI_STATUS_OK);
        REQUIRE(!strcmp(snapshot.column_id, accepted ? "doing" : "todo"));
        g_value_unset(&value); g_object_unref(provider); g_object_unref(fresh_source); g_object_unref(fresh_target);
    }
    g_object_unref(source); g_object_unref(target); g_object_unref(row); g_object_unref(column);
    UmiKanbanGtk4Retire(panel); gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
