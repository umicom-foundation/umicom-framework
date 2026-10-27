/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_details/test_archive_native.c
 *
 * PURPOSE:
 *   Exercise reversible archive actions, stale reviews and retained native controls.
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

typedef struct Observe { GtkWidget *panel, *form, *picker; const char *mode; unsigned calls; } Observe;
static void Changed(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property; Observe *o = data;
    if (++o->calls != 1U) return;
    if (!strcmp(o->mode, "review-retire") || !strcmp(o->mode, "load-retire")) UmiKanbanGtk4Retire(o->panel);
    else if (!strcmp(o->mode, "review-hide")) {
        gtk_widget_set_visible(o->form, FALSE); gtk_widget_set_visible(o->form, TRUE);
    } else if (!strcmp(o->mode, "review-selection")) {
        guint saved = gtk_drop_down_get_selected(GTK_DROP_DOWN(o->picker));
        /* Re-enter through the explicit prompt, which works with a nonempty
         * dropdown on each supported GTK version. Preserve the earlier probe. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(o->picker), GTK_INVALID_LIST_POSITION);
#endif
        gtk_drop_down_set_selected(GTK_DROP_DOWN(o->picker), 0U);
        gtk_drop_down_set_selected(GTK_DROP_DOWN(o->picker), saved);
    } else Click(o->panel, "Apply reviewed archive change");
}
static void SelectCardChoice(GtkWidget *picker)
{
    GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(picker));
    for (guint i = 0U; i < g_list_model_get_n_items(model); ++i) {
        GtkStringObject *item = g_list_model_get_item(model, i);
        bool matches = strstr(gtk_string_object_get_string(item), " | card | ") != NULL;
        g_object_unref(item);
        if (matches) { gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), i); return; }
    }
    REQUIRE(false);
}
int main(int argc, char **argv)
{
    REQUIRE(argc == 2); const char *name = argv[1];
    const char *known[] = {"archive", "restore", "roundtrip", "done", "dependency", "restore-wip",
        "restore-dependency", "empty", "no-selection", "no-review", "stale", "stale-review",
        "selection-roundtrip", "busy", "hidden", "retired", "review-retire", "review-hide",
        "review-selection", "load-retire", "review-reentry", "apply-reentry", "unicode", "reload"};
    bool recognized = false;
    for (size_t i = 0U; i < sizeof known / sizeof known[0]; ++i) if (!strcmp(name, known[i])) recognized = true;
    REQUIRE(recognized); if (!gtk_init_check()) return 77;
    bool restoring = !strcmp(name, "restore") || !strcmp(name, "restore-wip") || !strcmp(name, "restore-dependency");
    UmiKanbanBoard *board = Board(); UmiKanbanEdit *edit = Edits(1U);
    bool empty = !strcmp(name, "empty");
    if (!empty) {
        Create(edit, "other"); Apply(board, edit, 1U);
        Create(edit, "card"); strcpy(edit->card.description, "Retain this private body");
        edit->card.due_unix_seconds = 946684800U; edit->card.label_count = 1U; strcpy(edit->card.labels[0], "review");
        if (!strcmp(name, "unicode")) strcpy(edit->card.title, "\xE6\x96\x87\xE4\xBB\xB6 <b>plain</b>");
        if (!strcmp(name, "restore-dependency")) {
            edit->card.dependency_count = 1U; strcpy(edit->card.dependencies[0], "other");
        }
        Apply(board, edit, 1U);
        if (!strcmp(name, "done")) Done(board, edit, "card");
        if (restoring) { Action(edit, UMI_KANBAN_ARCHIVE_CARD, "card", NULL); Apply(board, edit, 1U); }
        if (!strcmp(name, "restore-wip"))
            REQUIRE(UmiKanbanColumnPolicyApply(board, umi_kanban_board_revision(board), "todo", "owner", 1U) == UMI_STATUS_OK);
        if (!strcmp(name, "restore-dependency")) {
            Action(edit, UMI_KANBAN_ARCHIVE_CARD, "other", NULL); Apply(board, edit, 1U);
        }
        if (!strcmp(name, "dependency")) {
            UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "other", &row) == UMI_STATUS_OK);
            Action(edit, UMI_KANBAN_UPDATE_CARD, "other", NULL); edit->card = row.card;
            edit->card.dependency_count = 1U; strcpy(edit->card.dependencies[0], "card"); Apply(board, edit, 1U);
        }
    }
    UmiKanbanCardSnapshot original = {0};
    if (!empty) REQUIRE(umi_kanban_board_find(board, "card", &original) == UMI_STATUS_OK);
    GtkWidget *panel = UmiKanbanGtk4Create(board); REQUIRE(panel); g_object_ref_sink(panel);
    GtkWindow *window = GTK_WINDOW(gtk_window_new()); g_object_ref_sink(window);
    gtk_window_set_child(window, panel); gtk_window_present(window);
    GtkWidget *expander = Find(panel, "kanban-card-archive-expander"); REQUIRE(GTK_IS_EXPANDER(expander));
    gtk_expander_set_expanded(GTK_EXPANDER(expander), TRUE); Drain();
    GtkWidget *form = Find(panel, "kanban-card-archive"); REQUIRE(gtk_widget_get_mapped(form));
    GtkWidget *picker = Find(panel, "kanban-archive-card"); REQUIRE(GTK_IS_DROP_DOWN(picker));
    GtkWidget *status = Find(panel, "kanban-archive-status");
    Observe observer = {panel, form, picker, name, 0U}; gulong handler = 0U;
    if (!strcmp(name, "load-retire")) handler = g_signal_connect(picker, "notify::model", G_CALLBACK(Changed), &observer);
    Click(panel, "Load cards for archiving");
    if (handler) { g_signal_handler_disconnect(picker, handler); handler = 0U; REQUIRE(observer.calls > 0U); }
    /* The initial unconditional picker assertion is retained for review.
     * Retiring during model publication may leave GTK with its default row.
     * No edit is authorized; only a live completed Load promises a blank picker. */
#if 0
    REQUIRE(gtk_drop_down_get_selected(GTK_DROP_DOWN(picker)) == GTK_INVALID_LIST_POSITION);
#endif
    /* A completed load now selects a prompt rather than requesting an empty
     * selection from GTK. Retain the earlier assertion and check the actual
     * model row as well as its index so a real card cannot become the prompt. */
#if 0
    if (strcmp(name, "load-retire"))
        REQUIRE(gtk_drop_down_get_selected(GTK_DROP_DOWN(picker)) == GTK_INVALID_LIST_POSITION);
#endif
    if (strcmp(name, "load-retire")) {
        REQUIRE(gtk_drop_down_get_selected(GTK_DROP_DOWN(picker)) == 0U);
        GtkStringObject *prompt = gtk_drop_down_get_selected_item(GTK_DROP_DOWN(picker));
        REQUIRE(GTK_IS_STRING_OBJECT(prompt));
        REQUIRE(!strcmp(gtk_string_object_get_string(prompt), "Choose a card"));
    }
    if (!empty && strcmp(name, "no-selection") && strcmp(name, "load-retire")) SelectCardChoice(picker);
    if (!strcmp(name, "stale-review")) { Create(edit, "external"); Apply(board, edit, 1U); }
    if (!strcmp(name, "review-hide") || !strcmp(name, "review-retire") || !strcmp(name, "review-selection") ||
        !strcmp(name, "review-reentry"))
        handler = g_signal_connect(status, "notify::label", G_CALLBACK(Changed), &observer);
    if (strcmp(name, "no-review")) Click(panel, "Review archive change");
    if (handler) { g_signal_handler_disconnect(status, handler); handler = 0U; REQUIRE(observer.calls > 0U); }
    if (!strcmp(name, "stale")) {
        Create(edit, "external"); Apply(board, edit, 1U); REQUIRE(UmiKanbanGtk4Refresh(panel) == UMI_STATUS_OK);
    } else if (!strcmp(name, "selection-roundtrip")) {
        /* Selecting the prompt and returning to the same card must invalidate
         * the earlier review. Keep the original empty-selection probe visible. */
#if 0
        gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), GTK_INVALID_LIST_POSITION); SelectCardChoice(picker);
#endif
        gtk_drop_down_set_selected(GTK_DROP_DOWN(picker), 0U); SelectCardChoice(picker);
    } else if (!strcmp(name, "busy")) { UmiKanbanGtk4SetBusy(panel, true); UmiKanbanGtk4SetBusy(panel, false); }
    else if (!strcmp(name, "hidden")) { gtk_widget_set_visible(form, FALSE); gtk_widget_set_visible(form, TRUE); }
    else if (!strcmp(name, "retired")) UmiKanbanGtk4Retire(panel);
    else if (!strcmp(name, "reload")) { Click(panel, "Load cards for archiving"); SelectCardChoice(picker); }
    uint64_t revision = umi_kanban_board_revision(board); size_t activity = umi_kanban_board_activity_count(board);
    size_t size = 0U; REQUIRE(umi_kanban_board_encode(board, revision, NULL, 0U, &size) == UMI_STATUS_OK);
    unsigned char *before = malloc(size), *after = malloc(size); REQUIRE(before && after);
    REQUIRE(umi_kanban_board_encode(board, revision, before, size, &size) == UMI_STATUS_OK);
    if (!strcmp(name, "apply-reentry")) handler = g_signal_connect(status, "notify::label", G_CALLBACK(Changed), &observer);
    Click(panel, "Apply reviewed archive change");
    if (handler) { g_signal_handler_disconnect(status, handler); REQUIRE(observer.calls > 0U); }
    bool accepted = !strcmp(name, "archive") || !strcmp(name, "restore") || !strcmp(name, "done") ||
        !strcmp(name, "roundtrip") || !strcmp(name, "unicode") || !strcmp(name, "review-reentry") || !strcmp(name, "apply-reentry");
    REQUIRE(umi_kanban_board_revision(board) == revision + (accepted ? 1U : 0U));
    REQUIRE(umi_kanban_board_activity_count(board) == activity + (accepted ? 1U : 0U));
    size_t after_size = 0U;
    REQUIRE(umi_kanban_board_encode(board, umi_kanban_board_revision(board), after, size, &after_size) == UMI_STATUS_OK);
    REQUIRE(size == after_size);
    if (!accepted) REQUIRE(!memcmp(before, after, size));
    if (!empty) {
        UmiKanbanCardSnapshot row; REQUIRE(umi_kanban_board_find(board, "card", &row) == UMI_STATUS_OK);
        REQUIRE(!memcmp(&row.card, &original.card, sizeof row.card));
        REQUIRE(!strcmp(row.column_id, original.column_id));
        REQUIRE(row.archived == (accepted ? !original.archived : original.archived));
        if (accepted) {
            REQUIRE((Find(panel, "card") != NULL) == (row.archived == 0));
            UmiKanbanActivity event;
            REQUIRE(umi_kanban_board_activity_at(board, revision + 1U, activity, &event) == UMI_STATUS_OK);
            REQUIRE(!strcmp(event.actor, "local-owner"));
            REQUIRE(event.kind == (restoring ? UMI_KANBAN_RESTORE_CARD : UMI_KANBAN_ARCHIVE_CARD));
            Click(panel, "Apply reviewed archive change"); REQUIRE(umi_kanban_board_revision(board) == revision + 1U);
            if (!strcmp(name, "roundtrip")) {
                UmiKanbanBoard *copy = NULL; uint64_t saved = 0U;
                REQUIRE(umi_kanban_board_decode(after, after_size, &copy, &saved) == UMI_STATUS_OK);
                REQUIRE(saved == revision + 1U);
                REQUIRE(umi_kanban_board_find(copy, "card", &row) == UMI_STATUS_OK && row.archived);
                REQUIRE(!memcmp(&row.card, &original.card, sizeof row.card)); umi_kanban_board_destroy(copy);
            }
        }
    }
    free(before); free(after); UmiKanbanGtk4Retire(panel);
    gtk_window_destroy(window); g_object_unref(window); g_object_unref(panel);
    free(edit); umi_kanban_board_destroy(board); return 0;
}
