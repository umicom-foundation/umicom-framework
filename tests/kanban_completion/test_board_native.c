/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_board_native.c
 *
 * PURPOSE:
 *   Exercise the shared board host with saved reviews, stale selections and retained child controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
#include "umicom/ui/gtk4/kanban.h"
static GtkWidget *Named(GtkWidget *root, const char *name)
{
    if (!strcmp(gtk_widget_get_name(root), name)) return root;
    for (GtkWidget *child = gtk_widget_get_first_child(root); child;
         child = gtk_widget_get_next_sibling(child)) {
        GtkWidget *match = Named(child, name);
        if (match) return match;
    }
    return NULL;
}
static void Select(GtkWidget *root, const char *id)
{
    GtkWidget *card = Named(root, id); REQUIRE(GTK_IS_BUTTON(card));
    g_signal_emit_by_name(card, "clicked");
}
static void Block(UmiKanbanBoard *board)
{
    UmiKanbanCardSnapshot card;
    REQUIRE(umi_kanban_board_find(board, "alpha", &card) == UMI_STATUS_OK);
    UmiKanbanEdit *edit = Edits(1); edit->kind = UMI_KANBAN_UPDATE_CARD; edit->card = card.card;
    strcpy(edit->card.blocked_reason, "Synthetic unresolved acceptance condition");
    Apply(board, edit, 1U); free(edit);
}
int main(int argc, char **argv)
{
    const char *const cases[] = {"normal", "no-selection", "selection-change", "external",
        "external-refresh", "stale-selection", "new-selection", "busy", "retired",
        "busy-after-save", "retire-after-save", "blocked", "blocked-exception",
        "load-only", "different-board", "retained-child"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    if (!gtk_init_check()) return 77;
    UmiKanbanBoard *board = Ready();
    if (!strcmp(name, "blocked") || !strcmp(name, "blocked-exception")) Block(board);
    GtkWidget *root = UmiKanbanGtk4Create(board); REQUIRE(root); g_object_ref_sink(root);
    GtkWidget *expander = Named(root, "kanban-completion-review"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE);
    GtkWidget *review = gtk_expander_get_child(GTK_EXPANDER(expander)); REQUIRE(review);
    g_object_ref(review);
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_child(window, root); gtk_window_present(window); NativeDrain();
    char directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(file, directory, "board.umicompletion");
    FillReview(review, file);
    if (strcmp(name, "no-selection")) Select(root, "alpha");
    if (!strcmp(name, "blocked-exception")) {
        Field(review, "digest", "");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(review, "exception")), TRUE);
    }
    if (!strcmp(name, "stale-selection") || !strcmp(name, "new-selection")) {
        Update(board, "beta", "Changed selection context");
        REQUIRE(UmiKanbanGtk4Refresh(root) == UMI_STATUS_OK);
        if (!strcmp(name, "new-selection")) Select(root, "alpha");
    }
    if (!strcmp(name, "busy")) UmiKanbanGtk4SetBusy(root, true);
    if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(root);
    ClickReview(review, "save"); NativeWait(review);
    bool saved = strcmp(name, "no-selection") && strcmp(name, "stale-selection") &&
        strcmp(name, "busy") && strcmp(name, "retired") && strcmp(name, "blocked") &&
        strcmp(name, "blocked-exception");
    UmiKanbanCompletionPlan *plan = NULL;
    REQUIRE((UmiKanbanCompletionPlanLoadFile(file, &plan) == UMI_STATUS_OK) == saved);
    if (saved) {
        REQUIRE(!strcmp(UmiKanbanCompletionPlanAt(plan, 0U)->card_id, "alpha"));
        if (!strcmp(name, "selection-change")) Select(root, "beta");
        if (!strcmp(name, "external") || !strcmp(name, "external-refresh")) {
            Update(board, "beta", "Changed after acceptance");
            if (!strcmp(name, "external-refresh")) REQUIRE(UmiKanbanGtk4Refresh(root) == UMI_STATUS_OK);
        }
        if (!strcmp(name, "busy-after-save")) UmiKanbanGtk4SetBusy(root, true);
        if (!strcmp(name, "retire-after-save")) UmiKanbanGtk4Retire(root);
        if (!strcmp(name, "load-only")) {
            ClickReview(review, "discard"); ClickReview(review, "load"); NativeWait(review);
            REQUIRE(gtk_text_buffer_get_char_count(Preview(review)) > 0);
        }
        if (!strcmp(name, "different-board")) {
            /* Matching board IDs and revisions do not authorize different
             * reviewed content: completion compares the full archive hash. */
            UmiKanbanBoard *other = Ready();
            Update(other, "beta", "Other content");
            Update(board, "beta", "Original content also advanced");
            UmiKanbanCompletionRecord record; Record(&record, "alpha");
            UmiKanbanCompletionPlan *other_plan = NULL;
            REQUIRE(Capture(other, &record, 1U, &other_plan) == UMI_STATUS_OK);
            char other_file[UMI_PATH_CAPACITY]; FixturePath(other_file, directory, "other.umicompletion");
            REQUIRE(UmiKanbanCompletionPlanWriteNew(other_file, other_plan) == UMI_STATUS_OK);
            UmiKanbanCompletionPlanDestroy(other_plan); umi_kanban_board_destroy(other);
            REQUIRE(UmiKanbanGtk4Refresh(root) == UMI_STATUS_OK);
            Field(review, "path", other_file); ClickReview(review, "load"); NativeWait(review);
        }
    }
    UmiKanbanCompletionPlanDestroy(plan);
    bool retained = !strcmp(name, "retained-child");
    if (retained) {
        /* Retire before releasing the borrowed board. Retained child controls
         * must not reacquire a dead host or use its released model. */
        UmiKanbanGtk4Retire(root); gtk_window_destroy(window);
        g_object_unref(root); root = NULL;
    }
    if (saved && strcmp(name, "load-only")) { Approve(review); ClickReview(review, "apply"); }
    bool changed = saved && (!strcmp(name, "normal") || !strcmp(name, "selection-change") ||
                            !strcmp(name, "new-selection"));
    ExpectColumn(board, "alpha", changed ? "done" : "testing");
    ExpectColumn(board, "beta", "testing");
    if (!retained) { UmiKanbanGtk4Retire(root); gtk_window_destroy(window); g_object_unref(root); }
    umi_kanban_board_destroy(board);
    Approve(review); ClickReview(review, "apply");
    NativeWait(review); g_object_unref(review); NativeDrain();
    return 0;
}
