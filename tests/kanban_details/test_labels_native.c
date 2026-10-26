/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_labels_native.c
 *
 * PURPOSE:
 *   Exercise visible label editing and interrupted or stale native drafts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
typedef struct Observation { GtkWidget *panel, *form; const char *mode; unsigned calls; } Observation;
static void Observe(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; Observation *o = data;
    if (++o->calls != 1U) return;
    if (!strcmp(o->mode, "load-retire")) UmiKanbanGtk4Retire(o->panel);
    else if (!strcmp(o->mode, "load-hide")) {
        gtk_widget_set_visible(o->form, FALSE); gtk_widget_set_visible(o->form, TRUE);
    } else Click(o->panel, "Apply labels");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"normal", "clear", "reentry", "no-selection", "stale", "selection-roundtrip", "busy", "hidden", "retired", "load-retire", "load-hide", "duplicate", "unicode-overflow"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized);
    if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.description, "Private body");
    edit->card.label_count = 1U; strcpy(edit->card.labels[0], "original"); Apply(board, edit, 1U);
    Create(edit, "other"); Apply(board, edit, 1U);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-card-labels-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-card-labels"); REQUIRE(gtk_widget_get_mapped(form));
    uint64_t revision = umi_kanban_board_revision(board);
    if (strcmp(name, "no-selection")) Click(panel, "card");
    Observation o = {panel, form, name, 0U}; gulong handler = 0U;
    GtkWidget *status = Find(panel, "kanban-labels-status");
    if (!strcmp(name, "load-retire") || !strcmp(name, "load-hide"))
        handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &o);
    Click(panel, "Load selected labels");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(o.calls > 0U); handler = 0U; }
    Set(panel, "kanban-label-text-0", !strcmp(name, "clear") ? "" : "release");
    if (strcmp(name, "clear")) Set(panel, "kanban-label-text-2", !strcmp(name, "duplicate") ? "release" : "review needed");
    if (!strcmp(name, "unicode-overflow")) {
        char text[64]; for (size_t i = 0U; i < 21U; ++i) memcpy(text + i * 3U, "\xe7\xa7\x81", 3U);
        text[63] = '\0';
        /* 22 characters fit GTK's character limit but exceed the byte limit. */
        char too_long[67]; memcpy(too_long, text, 63U); memcpy(too_long + 63U, "\xe7\xa7\x81", 4U);
        Set(panel, "kanban-label-text-0", too_long);
    } else if (!strcmp(name, "stale")) {
        Create(edit, "external"); Apply(board, edit, 1U); ++revision;
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    } else if (!strcmp(name, "selection-roundtrip")) { Click(panel, "other"); Click(panel, "card"); }
    else if (!strcmp(name, "busy")) UmiKanbanGtk4SetBusy(panel, true);
    else if (!strcmp(name, "hidden")) gtk_widget_set_visible(form, FALSE);
    else if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(panel);
    else if (!strcmp(name, "reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &o);
    Click(panel, "Apply labels");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(o.calls > 0U); }
    bool accepted = !strcmp(name, "normal") || !strcmp(name, "clear") || !strcmp(name, "reentry");
    REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
    UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
    REQUIRE(!strcmp(row.card.description, "Private body"));
    if (accepted) {
        REQUIRE(row.card.label_count == (!strcmp(name, "clear") ? 0U : 2U));
        if (row.card.label_count) REQUIRE(!strcmp(row.card.labels[0], "release") && !strcmp(row.card.labels[1], "review needed"));
        Click(panel, "Apply labels"); REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
    } else REQUIRE(row.card.label_count == 1U && !strcmp(row.card.labels[0], "original"));
    UmiKanbanGtk4Retire(panel); gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
