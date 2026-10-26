/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/kanban_completion/test_native.c
 *
 * PURPOSE:
 *   Check retained-review admission, explicit approval, interruption and immutable worker inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "native_fixture.h"
typedef struct ReviewContext {
    UmiKanbanBoard *board;
    GtkWidget *panel;
    unsigned captures, applies, destroyed;
    bool retire_capture, deny, reenter;
} ReviewContext;
static UmiStatus CaptureReview(gpointer data, const UmiKanbanCompletionReview *review,
    UmiKanbanCompletionPlan **out)
{
    ReviewContext *c = data;
    ++c->captures;
    UmiKanbanCompletionRecord record = {0};
    strcpy(record.card_id, "alpha"); record.decision = review->decision;
    memcpy(record.evidence_sha256, review->evidence_sha256, sizeof record.evidence_sha256);
    memcpy(record.rationale, review->rationale, sizeof record.rationale);
    if (c->retire_capture) UmiKanbanCompletionGtk4Retire(c->panel);
    return UmiKanbanCompletionPlanCreate(c->board, umi_kanban_board_revision(c->board),
        "done", review->reviewer, review->source_identity, review->configuration, &record, 1U, out);
}
static UmiStatus ApplyReview(gpointer data, const UmiKanbanCompletionPlan *plan)
{
    ReviewContext *c = data;
    ++c->applies;
    if (c->reenter) ClickReview(c->panel, "apply");
    if (c->deny) return UMI_STATUS_PERMISSION_DENIED;
    return UmiKanbanCompletionPlanApply(plan, c->board, umi_kanban_board_revision(c->board));
}
static void DestroyReview(gpointer data) { ++((ReviewContext *)data)->destroyed; }
typedef struct Interruption {
    GtkWidget *panel;
    unsigned calls;
} Interruption;
static void InterruptPreview(GtkTextBuffer *buffer, gpointer data)
{
    Interruption *i = data;
    if (i->calls || gtk_text_buffer_get_char_count(buffer) == 0) return;
    ++i->calls;
    gtk_widget_set_visible(i->panel, FALSE);
    gtk_widget_set_visible(i->panel, TRUE);
}
static void InterruptApproval(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)spec;
    Interruption *i = data;
    if (i->calls || gtk_check_button_get_active(GTK_CHECK_BUTTON(object))) return;
    ++i->calls;
    gtk_widget_set_visible(i->panel, FALSE);
    gtk_widget_set_visible(i->panel, TRUE);
}
static void InterruptStatus(GObject *object, GParamSpec *spec, gpointer data)
{
    (void)spec;
    Interruption *i = data;
    if (i->calls || !strstr(gtk_label_get_text(GTK_LABEL(object)), "Review the retained")) return;
    ++i->calls;
    gtk_widget_set_visible(i->panel, FALSE);
    gtk_widget_set_visible(i->panel, TRUE);
}
int main(int argc, char **argv)
{
    const char *const cases[] = {"normal", "no-approval", "load-only", "stale", "denied",
        "busy", "retired", "capture-retire", "close-load", "hide-show", "captured-path",
        "captured-fields", "existing", "invalid-file", "discard", "exception", "reentry",
        "replay", "retained", "preview-interruption", "approval-interruption",
        "status-interruption", "busy-load", "null"};
    REQUIRE(argc == 2 && Known(argv[1], cases, sizeof cases / sizeof cases[0]));
    const char *name = argv[1];
    if (!gtk_init_check()) return 77;
    ReviewContext context = {0}; context.board = Ready();
    context.panel = UmiKanbanCompletionGtk4Create(CaptureReview, ApplyReview, &context, DestroyReview);
    REQUIRE(context.panel); g_object_ref_sink(context.panel);
    if (!strcmp(name, "null")) {
        REQUIRE(UmiKanbanCompletionGtk4Create(NULL, ApplyReview, NULL, NULL) == NULL);
        REQUIRE(UmiKanbanCompletionGtk4Create(CaptureReview, NULL, NULL, NULL) == NULL);
        REQUIRE(!UmiKanbanCompletionGtk4Pending(NULL));
    }
    GtkWindow *window = GTK_WINDOW(gtk_window_new());
    gtk_window_set_child(window, context.panel); gtk_window_present(window); NativeDrain();
    char directory[UMI_PATH_CAPACITY], file[UMI_PATH_CAPACITY], second[UMI_PATH_CAPACITY];
    FixtureDirectory(directory); FixturePath(file, directory, "review.umicompletion");
    FixturePath(second, directory, "other.umicompletion");
    FillReview(context.panel, file);
    if (!strcmp(name, "exception")) {
        Field(context.panel, "digest", "");
        gtk_check_button_set_active(GTK_CHECK_BUTTON(Control(context.panel, "exception")), TRUE);
    }
    if (!strcmp(name, "busy")) UmiKanbanCompletionGtk4SetBusy(context.panel, TRUE);
    if (!strcmp(name, "retired")) UmiKanbanCompletionGtk4Retire(context.panel);
    context.retire_capture = !strcmp(name, "capture-retire");
    GtkWidget *apply = g_object_ref(Control(context.panel, "apply"));
    GtkTextBuffer *preview = g_object_ref(Preview(context.panel));
    ClickReview(context.panel, "save");
    if (!strcmp(name, "captured-path")) Field(context.panel, "path", second);
    if (!strcmp(name, "captured-fields")) Field(context.panel, "rationale", "Later unreviewed text");
    NativeWait(context.panel);
    bool admitted = strcmp(name, "busy") && strcmp(name, "retired") && strcmp(name, "capture-retire");
    REQUIRE(context.captures == ((!strcmp(name, "busy") || !strcmp(name, "retired")) ? 0U : 1U));
    REQUIRE((gtk_text_buffer_get_char_count(preview) > 0) == admitted);
    REQUIRE(context.applies == 0U);
    UmiKanbanCompletionPlan *saved = NULL;
    UmiStatus loaded = UmiKanbanCompletionPlanLoadFile(file, &saved);
    REQUIRE((loaded == UMI_STATUS_OK) == admitted);
    if (admitted) {
        const UmiKanbanCompletionRecord *record = UmiKanbanCompletionPlanAt(saved, 0U);
        REQUIRE(record && strstr(record->rationale, "Synthetic reviewed") != NULL);
        REQUIRE(record->decision == (!strcmp(name, "exception")
            ? UMI_KANBAN_COMPLETION_EXCEPTION : UMI_KANBAN_COMPLETION_REVIEWED));
    }
    UmiKanbanCompletionPlanDestroy(saved);
    if (!strcmp(name, "captured-path")) {
        UmiKanbanCompletionPlan *unexpected = NULL;
        REQUIRE(UmiKanbanCompletionPlanLoadFile(second, &unexpected) == UMI_STATUS_NOT_FOUND);
        REQUIRE(unexpected == NULL);
    }
    bool closed = false;
    if (admitted) {
        Field(context.panel, "path", file);
        if (!strcmp(name, "existing") || !strcmp(name, "invalid-file")) {
            if (!strcmp(name, "invalid-file")) Field(context.panel, "path", second);
            ClickReview(context.panel, !strcmp(name, "existing") ? "save" : "load");
            NativeWait(context.panel);
            REQUIRE(gtk_text_buffer_get_char_count(preview) > 0);
        }
        if (!strcmp(name, "load-only") || !strcmp(name, "close-load") ||
            !strcmp(name, "hide-show") || !strcmp(name, "busy-load") ||
            !strcmp(name, "preview-interruption") || !strcmp(name, "approval-interruption") ||
            !strcmp(name, "status-interruption")) {
            ClickReview(context.panel, "discard");
            REQUIRE(gtk_text_buffer_get_char_count(preview) == 0);
            Interruption interruption = {context.panel, 0U};
            gulong handler = 0U;
            GObject *observed = NULL;
            if (!strcmp(name, "preview-interruption")) {
                observed = G_OBJECT(preview);
                handler = g_signal_connect(preview, "changed", G_CALLBACK(InterruptPreview), &interruption);
            } else if (!strcmp(name, "approval-interruption")) {
                Approve(context.panel);
                observed = G_OBJECT(Control(context.panel, "approval"));
                handler = g_signal_connect(observed, "notify::active", G_CALLBACK(InterruptApproval), &interruption);
            } else if (!strcmp(name, "status-interruption")) {
                observed = G_OBJECT(Control(context.panel, "status"));
                handler = g_signal_connect(observed, "notify::label", G_CALLBACK(InterruptStatus), &interruption);
            }
            ClickReview(context.panel, "load");
            if (!strcmp(name, "close-load")) { gtk_window_destroy(window); closed = true; }
            if (!strcmp(name, "hide-show")) {
                gtk_widget_set_visible(context.panel, FALSE);
                gtk_widget_set_visible(context.panel, TRUE);
            }
            if (!strcmp(name, "busy-load")) UmiKanbanCompletionGtk4SetBusy(context.panel, TRUE);
            NativeWait(context.panel);
            if (handler) {
                REQUIRE(interruption.calls == 1U);
                g_signal_handler_disconnect(observed, handler);
            }
            if (!strcmp(name, "busy-load")) UmiKanbanCompletionGtk4SetBusy(context.panel, FALSE);
        }
        if (!strcmp(name, "stale")) Update(context.board, "beta", "A newer board revision");
        if (!strcmp(name, "discard")) ClickReview(context.panel, "discard");
        if (!strcmp(name, "retained")) { UmiKanbanCompletionGtk4Retire(context.panel); }
        context.deny = !strcmp(name, "denied"); context.reenter = !strcmp(name, "reentry");
        if (!closed && strcmp(name, "no-approval") && strcmp(name, "load-only")) Approve(context.panel);
        g_signal_emit_by_name(apply, "clicked");
    }
    bool attempted = admitted && strcmp(name, "no-approval") && strcmp(name, "load-only") &&
        strcmp(name, "close-load") && strcmp(name, "hide-show") && strcmp(name, "busy-load") &&
        strcmp(name, "preview-interruption") && strcmp(name, "approval-interruption") &&
        strcmp(name, "status-interruption") && strcmp(name, "discard") && strcmp(name, "retained");
    bool changed = attempted && strcmp(name, "stale") && strcmp(name, "denied");
    REQUIRE(context.applies == (attempted ? 1U : 0U));
    ExpectColumn(context.board, "alpha", changed ? "done" : "testing");
    ExpectColumn(context.board, "beta", "testing");
    if (!strcmp(name, "replay")) {
        uint64_t revision = umi_kanban_board_revision(context.board);
        Approve(context.panel); g_signal_emit_by_name(apply, "clicked");
        REQUIRE(context.applies == 1U && umi_kanban_board_revision(context.board) == revision);
    }
    if (!closed) gtk_window_destroy(window);
    NativeWait(context.panel);
    g_object_unref(apply); g_object_unref(preview);
    UmiKanbanCompletionGtk4Retire(context.panel);
    g_object_unref(context.panel); NativeDrain();
    REQUIRE(context.destroyed == 1U);
    umi_kanban_board_destroy(context.board);
    return 0;
}
