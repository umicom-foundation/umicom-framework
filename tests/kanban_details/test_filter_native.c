/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_filter_native.c
 *
 * PURPOSE:
 *   Check native filtering without changing records, occupancy rules or action authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
#include "umicom/kanban/developers.h"
#include "umicom/kanban/column_policy.h"

typedef struct FilterObserver { GtkWidget *panel; unsigned calls; } FilterObserver;
static void Feedback(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; FilterObserver *o = data;
    if (++o->calls == 1U) { Click(o->panel, "Show all active cards"); Click(o->panel, "Apply view filter"); }
}
static void Developer(UmiKanbanBoard *board, UmiKanbanEdit *edit, const char *id, const char *name)
{
    Action(edit, UMI_KANBAN_PUT_DEVELOPER, id, NULL); strcpy(edit->card.title, name); Apply(board, edit, 1U);
}
static void CheckView(GtkWidget *panel, bool one, bool two, bool done)
{
    REQUIRE((Find(panel, "one") != NULL) == one);
    REQUIRE((Find(panel, "two") != NULL) == two);
    REQUIRE((Find(panel, "finished") != NULL) == done);
    REQUIRE(Find(panel, "archived") == NULL);
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"text", "text-case", "description", "label", "label-case", "developer",
        "developer-case", "developer-id", "developer-unicode", "developer-unknown", "developer-ambiguous",
        "priority", "urgent", "blocked", "overdue", "due-now", "invalid-cutoff", "offset-cutoff",
        "combined", "clear", "clear-draft", "selection", "feedback-reentry", "busy", "hidden", "retired",
        "label-overflow", "text-overflow", "invalid-priority", "refresh"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized); if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    Developer(board, edit, "person.alice", !strcmp(name, "developer-unicode") ? "Zo\xC3\xAB" : "Alice");
    Developer(board, edit, "person.bob", "Bob");
    if (!strcmp(name, "developer-ambiguous")) Developer(board, edit, "person.duplicate", "ALICE");
    Create(edit, "one"); strcpy(edit->card.description, "Private Needle in description");
    strcpy(edit->card.assignee, "person.alice"); edit->card.priority = UMI_KANBAN_HIGH;
    edit->card.label_count = 1U; strcpy(edit->card.labels[0], "work");
    edit->card.due_unix_seconds = 946684800U; Apply(board, edit, 1U);
    Create(edit, "two"); strcpy(edit->card.assignee, "person.bob");
    strcpy(edit->card.blocked_reason, "Waiting for owner"); edit->card.due_unix_seconds = 946771200U;
    edit->card.label_count = 1U; strcpy(edit->card.labels[0], "other"); Apply(board, edit, 1U);
    Create(edit, "finished"); edit->card.due_unix_seconds = 1U; Apply(board, edit, 1U); Done(board, edit, "finished");
    Create(edit, "archived"); edit->card.due_unix_seconds = 1U; Apply(board, edit, 1U);
    Action(edit, UMI_KANBAN_ARCHIVE_CARD, "archived", NULL); Apply(board, edit, 1U);
    REQUIRE(UmiKanbanColumnPolicyApply(board, umi_kanban_board_revision(board), "todo", "owner", 2U) == UMI_STATUS_OK);
    uint64_t revision = umi_kanban_board_revision(board); size_t activity = umi_kanban_board_activity_count(board);
    size_t size = 0U; REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *before = malloc(size), *after = malloc(size); REQUIRE(before && after);
    REQUIRE(umi_kanban_board_encode(board, revision, before, size, &size) == UMI_STATUS_OK);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-view-filter-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-view-filter"); REQUIRE(gtk_widget_get_mapped(form));
    CheckView(panel, true, true, true);
    bool want_one = true, want_two = false, want_done = false;
    GtkWidget *retained = NULL;
    if (!strcmp(name, "selection")) { Click(panel, "one"); retained = g_object_ref(Find(panel, "one")); }
    if (!strncmp(name, "developer", 9U)) {
        const char *developer = !strcmp(name, "developer-case") ? "aLiCe" :
            !strcmp(name, "developer-id") ? "person.alice" :
            !strcmp(name, "developer-unicode") ? "zo\xC3\xAB" :
            !strcmp(name, "developer-unknown") ? "missing" : "Alice";
        Set(panel, "kanban-filter-developer", developer);
        if (!strcmp(name, "developer-unknown") || !strcmp(name, "developer-ambiguous")) want_two = want_done = true;
    } else if (!strcmp(name, "label") || !strcmp(name, "label-case") || !strcmp(name, "label-overflow")) {
        Set(panel, "kanban-filter-label", !strcmp(name, "label-case") ? "WORK" : "work");
        if (!strcmp(name, "label-case")) want_one = false;
        if (!strcmp(name, "label-overflow")) {
            char value[67]; for (size_t i = 0U; i < 22U; ++i) memcpy(value + i * 3U, "\xE6\x96\x87", 3U); value[66] = '\0';
            Set(panel, "kanban-filter-label", value); want_two = want_done = true;
        }
    } else if (!strcmp(name, "priority") || !strcmp(name, "urgent") || !strcmp(name, "invalid-priority")) {
        gtk_drop_down_set_selected(GTK_DROP_DOWN(Find(panel, "kanban-filter-priority")),
            !strcmp(name, "priority") ? 2U : !strcmp(name, "urgent") ? 3U : GTK_INVALID_LIST_POSITION);
        if (!strcmp(name, "urgent")) want_one = false;
        if (!strcmp(name, "invalid-priority")) want_two = want_done = true;
    } else if (!strcmp(name, "blocked")) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(panel, "kanban-filter-blocked")), TRUE);
        want_one = false; want_two = true;
    } else if (!strcmp(name, "overdue") || !strcmp(name, "due-now") ||
               !strcmp(name, "invalid-cutoff") || !strcmp(name, "offset-cutoff")) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Find(panel, "kanban-filter-overdue")), TRUE);
        Set(panel, "kanban-filter-time", !strcmp(name, "due-now") ? "2000-01-01T00:00:00Z" :
            !strcmp(name, "invalid-cutoff") ? "2100-02-29T00:00:00Z" :
            !strcmp(name, "offset-cutoff") ? "2000-01-02T00:00:00+00:00" : "2000-01-02T00:00:00Z");
        if (!strcmp(name, "due-now")) want_one = false;
        if (!strcmp(name, "invalid-cutoff") || !strcmp(name, "offset-cutoff")) want_two = want_done = true;
    } else {
        Set(panel, "kanban-filter-text", !strcmp(name, "text") ? "one" : !strcmp(name, "text-case") ? "needle" : "Needle");
        if (!strcmp(name, "text-case")) want_one = false;
        if (!strcmp(name, "combined")) { Set(panel, "kanban-filter-label", "other"); want_one = false; }
        if (!strcmp(name, "text-overflow")) {
            char value[259]; for (size_t i = 0U; i < 86U; ++i) memcpy(value + i * 3U, "\xE6\x96\x87", 3U); value[258] = '\0';
            Set(panel, "kanban-filter-text", value); want_two = want_done = true;
        }
    }
    if (!strcmp(name, "busy")) { UmiKanbanGtk4SetBusy(panel, true); want_two = want_done = true; }
    if (!strcmp(name, "hidden")) { gtk_widget_set_visible(form, FALSE); want_two = want_done = true; }
    if (!strcmp(name, "retired")) { UmiKanbanGtk4Retire(panel); want_two = want_done = true; }
    FilterObserver observer = {panel, 0U}; gulong handler = 0U; GtkWidget *status = Find(panel, "kanban-filter-status");
    if (!strcmp(name, "feedback-reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Feedback), &observer);
    Click(panel, "Apply view filter");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); }
    if (!strcmp(name, "clear") || !strcmp(name, "clear-draft")) {
        CheckView(panel, true, false, false); Click(panel, "Show all active cards"); want_two = want_done = true;
        REQUIRE(!strcmp(gtk_editable_get_text(GTK_EDITABLE(Find(panel, "kanban-filter-text"))), "Needle"));
        if (!strcmp(name, "clear-draft")) { Click(panel, "Apply view filter"); want_two = want_done = false; }
    }
    if (retained) {
        Set(panel, "kanban-title", "Unapproved rename"); g_signal_emit_by_name(retained, "clicked");
        Click(panel, "Rename selected"); g_object_unref(retained);
    }
    CheckView(panel, want_one, want_two, want_done);
    GtkWidget *limit = Find(Find(panel, "todo"), "kanban-list-limit"); REQUIRE(GTK_IS_LABEL(limit));
    char occupancy[64]; g_snprintf(occupancy, sizeof occupancy, "%u shown / 2 active / limit 2", (unsigned)want_one + (unsigned)want_two);
    REQUIRE(!strcmp(gtk_label_get_text(GTK_LABEL(limit)), occupancy));
    REQUIRE(umi_kanban_board_revision(board) == revision && umi_kanban_board_activity_count(board) == activity);
    size_t after_size = 0U; REQUIRE(umi_kanban_board_encode(board, revision, after, size, &after_size) == UMI_STATUS_OK);
    REQUIRE(after_size == size && !memcmp(before, after, size));
    if (!strcmp(name, "refresh")) {
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "two", &row) == UMI_STATUS_OK);
        Action(edit, UMI_KANBAN_UPDATE_CARD, "two", NULL); edit->card = row.card; strcpy(edit->card.description, "Another Needle");
        Apply(board, edit, 1U); REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK); CheckView(panel, true, true, false);
        REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
    }
    free(before); free(after); UmiKanbanGtk4Retire(panel);
    gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
