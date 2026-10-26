/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_deadline_native.c
 *
 * PURPOSE:
 *   Exercise canonical UTC input, explicit clearing and interrupted deadline forms.
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
    } else Click(o->panel, "Apply deadline");
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"normal", "clear", "leap", "invalid-leap", "offset", "fraction", "junk", "pre-epoch",
        "epoch", "first-second", "after-2038", "maximum", "unsupported", "unsupported-clear",
        "stale", "no-selection", "busy", "hidden", "retired", "selection-roundtrip", "load-hide", "load-retire", "reentry"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized);
    if (!gtk_init_check()) return 77;
    bool unsupported = !strcmp(name, "unsupported") || !strcmp(name, "unsupported-clear");
    uint64_t original = unsupported ? UINT64_MAX : 1U;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Create(edit, "card"); strcpy(edit->card.description, "Private body"); edit->card.due_unix_seconds = original;
    Apply(board, edit, 1U); Create(edit, "other"); Apply(board, edit, 1U);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-card-deadline-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-card-deadline"); REQUIRE(gtk_widget_get_mapped(form));
    uint64_t revision = umi_kanban_board_revision(board);
    if (strcmp(name, "no-selection")) Click(panel, "card");
    GtkWidget *status = Find(panel, "kanban-deadline-status");
    GtkWidget *enabled = Find(panel, "kanban-deadline-enabled"); REQUIRE(GTK_IS_CHECK_BUTTON(enabled));
    Observation o = {panel, form, name, 0U}; gulong handler = 0U;
    if (!strcmp(name, "load-hide") || !strcmp(name, "load-retire"))
        handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &o);
    Click(panel, "Load selected deadline");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(o.calls > 0U); handler = 0U; }
    if (unsupported) {
        REQUIRE(gtk_check_button_get_active(GTK_CHECK_BUTTON(enabled)));
        REQUIRE(strstr(gtk_label_get_text(GTK_LABEL(status)), "preserved"));
    }
    const char *text = !strcmp(name, "leap") ? "2000-02-29T00:00:00Z" :
        !strcmp(name, "invalid-leap") ? "2100-02-29T00:00:00Z" :
        !strcmp(name, "offset") ? "2000-01-01T00:00:00+00:00" :
        !strcmp(name, "fraction") ? "2000-01-01T00:00:00.0Z" :
        !strcmp(name, "junk") ? "2000-01-01T00:00:00Zjunk" :
        !strcmp(name, "pre-epoch") ? "1969-12-31T23:59:59Z" :
        !strcmp(name, "epoch") ? "1970-01-01T00:00:00Z" :
        !strcmp(name, "first-second") ? "1970-01-01T00:00:01Z" :
        !strcmp(name, "after-2038") ? "2038-01-19T03:14:08Z" :
        !strcmp(name, "maximum") ? "9999-12-31T23:59:59Z" : "2000-01-01T00:00:00Z";
    if (!unsupported) Set(panel, "kanban-deadline-value", text);
    bool clear = !strcmp(name, "clear") || !strcmp(name, "unsupported-clear");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(enabled), !clear);
    if (!strcmp(name, "stale")) {
        Create(edit, "external"); Apply(board, edit, 1U); ++revision;
        REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    } else if (!strcmp(name, "selection-roundtrip")) { Click(panel, "other"); Click(panel, "card"); }
    else if (!strcmp(name, "busy")) UmiKanbanGtk4SetBusy(panel, true);
    else if (!strcmp(name, "hidden")) gtk_widget_set_visible(form, FALSE);
    else if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(panel);
    else if (!strcmp(name, "reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Observe), &o);
    Click(panel, "Apply deadline");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(o.calls > 0U); }
    bool accepted = !strcmp(name, "normal") || clear || !strcmp(name, "leap") || !strcmp(name, "first-second") ||
        !strcmp(name, "after-2038") || !strcmp(name, "maximum") || !strcmp(name, "reentry");
    REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
    UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
    REQUIRE(!strcmp(row.card.description, "Private body"));
    uint64_t expected = !accepted ? original : clear ? 0U : !strcmp(name, "leap") ? 951782400U :
        !strcmp(name, "first-second") ? 1U : !strcmp(name, "after-2038") ? UINT64_C(2147483648) :
        !strcmp(name, "maximum") ? UINT64_C(253402300799) : 946684800U;
    REQUIRE(row.card.due_unix_seconds == expected);
    if (accepted) {
        GtkWidget *card = Find(panel, "card"); REQUIRE(card);
        GtkWidget *summary = Find(card, "kanban-card-deadline-summary");
        if (expected) REQUIRE(GTK_IS_LABEL(summary) && strstr(gtk_label_get_text(GTK_LABEL(summary)), text));
        else REQUIRE(summary == NULL);
        Click(panel, "Apply deadline"); REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
    }
    UmiKanbanGtk4Retire(panel); gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
