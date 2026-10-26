/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/kanban_completion.c
 *
 * PURPOSE:
 *   Separate worker-owned review-file I/O from explicit GTK-thread board completion.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/kanban_completion.h"
#include "umicom/kanban/completion_storage.h"
#include "umicom/platform/path.h"
#include "umicom/ui/gtk4/automation.h"
#include "focus_handoff_private.h"
#ifdef UMICOM_KANBAN_SAVED_REPORT
#include "umicom/ui/gtk4/ctest_report.h"
#endif
#include <string.h>

#define COMPLETION_KEY "umicom-kanban-completion"
typedef struct CompletionPanel {
    GtkWidget *root, *path, *status, *approval, *exception, *fields[5];
    GtkTextBuffer *preview;
#ifdef UMICOM_KANBAN_SAVED_REPORT
    GtkWidget *report_browser;
#endif
    UmiKanbanCompletionPlan *accepted;
    UmiKanbanCompletionCapture capture;
    UmiKanbanCompletionApply apply;
    gpointer context;
    GDestroyNotify destroy;
    gboolean pending, busy, retired, abandoned, applied, accepted_ready;
} CompletionPanel;
typedef struct CompletionJob {
    GtkWidget *root;
    char path[UMI_PATH_CAPACITY];
    gboolean save;
    UmiStatus status;
    UmiKanbanCompletionPlan *plan;
} CompletionJob;
static CompletionPanel *State(GtkWidget *root)
{
    return GTK_IS_WIDGET(root) ? g_object_get_data(G_OBJECT(root), COMPLETION_KEY) : NULL;
}
static gboolean Live(CompletionPanel *s)
{
    return s && !s->retired && !s->busy && gtk_widget_get_mapped(s->root);
}
static gboolean Active(CompletionPanel *s, GtkWidget *button)
{
    return Live(s) && !s->pending && gtk_widget_get_mapped(button) &&
        gtk_widget_is_ancestor(button, s->root);
}
static void Message(CompletionPanel *s, const char *text)
{
    if (Live(s)) gtk_label_set_text(GTK_LABEL(s->status), text);
}
static void FreePanel(gpointer data)
{
    CompletionPanel *s = data;
    UmiKanbanCompletionPlanDestroy(s->accepted);
#ifdef UMICOM_KANBAN_SAVED_REPORT
    g_clear_object(&s->report_browser);
#endif
    if (s->destroy) s->destroy(s->context);
    for (size_t i = 0U; i < 5U; ++i) g_object_unref(s->fields[i]);
    g_object_unref(s->path); g_object_unref(s->status); g_object_unref(s->approval);
    g_object_unref(s->exception); g_object_unref(s->preview); g_free(s);
}
static gboolean Copy(GtkWidget *entry, char *out, size_t capacity)
{
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry));
    size_t size = strlen(text);
    if (size >= capacity || !g_utf8_validate(text, -1, NULL)) return FALSE;
    memcpy(out, text, size + 1U); return TRUE;
}
static gboolean ReadReview(CompletionPanel *s, UmiKanbanCompletionReview *review)
{
    memset(review, 0, sizeof *review);
    review->decision = gtk_check_button_get_active(GTK_CHECK_BUTTON(s->exception))
        ? UMI_KANBAN_COMPLETION_EXCEPTION : UMI_KANBAN_COMPLETION_REVIEWED;
    return Copy(s->fields[0], review->reviewer, sizeof review->reviewer) &&
        Copy(s->fields[1], review->source_identity, sizeof review->source_identity) &&
        Copy(s->fields[2], review->configuration, sizeof review->configuration) &&
        Copy(s->fields[3], review->evidence_sha256, sizeof review->evidence_sha256) &&
        Copy(s->fields[4], review->rationale, sizeof review->rationale);
}
static GString *Describe(const UmiKanbanCompletionPlan *plan, const char *path)
{
    UmiKanbanCompletionPlanInfo info;
    if (UmiKanbanCompletionPlanRead(plan, &info) != UMI_STATUS_OK) return NULL;
    GString *text = g_string_new("PRIVATE completion review\n");
    g_string_append_printf(text, "Retained file: %s\nBoard: %s\nRevision: %" G_GUINT64_FORMAT
        "\nBoard SHA-256: %s\nReviewer: %s\nSource: %s\nConfiguration: %s\nDestination: %s\n",
        path, info.board_id, (guint64)info.reviewed_revision, info.board_sha256,
        info.reviewer, info.source_identity, info.configuration, info.done_column);
    for (size_t i = 0U; i < info.card_count; ++i) {
        const UmiKanbanCompletionRecord *r = UmiKanbanCompletionPlanAt(plan, i);
        g_string_append_printf(text, "\nCard: %s\nDecision: %s\nEvidence SHA-256: %s\nRationale: %s\n",
            r->card_id, r->decision == UMI_KANBAN_COMPLETION_EXCEPTION ? "Explicit exception" : "Reviewed",
            r->evidence_sha256[0] ? r->evidence_sha256 : "(not supplied)", r->rationale);
    }
    return text;
}
static void JobFree(gpointer data)
{
    CompletionJob *j = data;
    UmiKanbanCompletionPlanDestroy(j->plan); g_object_unref(j->root); g_free(j);
}
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    CompletionJob *j = data;
    j->status = j->save ? UmiKanbanCompletionPlanWriteNew(j->path, j->plan)
        : UmiKanbanCompletionPlanLoadFile(j->path, &j->plan);
    g_task_return_boolean(task, TRUE);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer context)
{
    (void)source; (void)context;
    CompletionJob *j = g_task_get_task_data(G_TASK(result));
    CompletionPanel *s = State(j->root);
    if (s && !s->abandoned && Live(s)) {
        if (j->status == UMI_STATUS_OK) {
            GString *description = Describe(j->plan, j->path);
            if (description) {
                /* Adopt the complete owned candidate before observable setters.
                 * Keep pending true across the whole publication so callbacks
                 * cannot start another request or apply an unseen decision. */
                UmiKanbanCompletionPlanDestroy(s->accepted);
                s->accepted = j->plan; j->plan = NULL; s->applied = FALSE;
                s->accepted_ready = FALSE;
                gtk_check_button_set_active(GTK_CHECK_BUTTON(s->approval), FALSE);
                if (Live(s) && !s->abandoned)
                    gtk_text_buffer_set_text(s->preview, description->str, (gint)description->len);
                g_string_free(description, TRUE);
                /* An observer can hide/retire during either setter. Never let
                 * a partly displayed candidate authorize an Apply after remap. */
                s->accepted_ready = Live(s) && !s->abandoned;
                /* Preserve the initial final-message publication for review.
                 * A status observer can also hide/remap the panel; readiness
                 * must be rechecked after every observable publication step. */
#if 0
                if (s->accepted_ready)
                    Message(s, "Review the retained decision below. Approve and Apply separately; no board has changed.");
#endif
                if (s->accepted_ready) {
                    Message(s, "Review the retained decision below. Approve and Apply separately; no board has changed.");
                    s->accepted_ready = Live(s) && !s->abandoned;
                }
            }
        } else {
            char text[256];
            g_snprintf(text, sizeof text, "Review file refused: %s. Previous accepted review is retained; existing files are never replaced.",
                umi_status_text(j->status));
            Message(s, text);
        }
    }
    if (s) s->pending = FALSE;
    /* The ready callback runs after the worker returns. Release GTK-owning
     * job data here: GTask finalization itself can occur on a worker thread. */
    g_task_set_task_data(G_TASK(result), NULL, NULL);
}
static void Dispatch(GtkButton *button, gpointer data)
{
    CompletionPanel *s = State(data);
    if (!Active(s, GTK_WIDGET(button))) return;
    GtkWidget *root = g_object_ref(s->root);
    s->pending = TRUE; s->abandoned = FALSE;
    CompletionJob *j = g_new0(CompletionJob, 1); j->root = g_object_ref(root);
    j->save = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "completion-save")) != 0;
    UmiStatus status = Copy(s->path, j->path, sizeof j->path) ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
    UmiKanbanCompletionReview review;
    if (status == UMI_STATUS_OK && j->save) {
        status = ReadReview(s, &review) ? UMI_STATUS_OK : UMI_STATUS_CAPACITY_EXCEEDED;
        if (status == UMI_STATUS_OK) status = s->capture(s->context, &review, &j->plan);
        if (status == UMI_STATUS_OK && !j->plan) status = UMI_STATUS_INVALID_STATE;
    }
    if (status == UMI_STATUS_OK && Live(s) && !s->abandoned) {
        Message(s, j->save ? "Saving a new private completion review..." : "Reading a private completion review...");
        if (Live(s) && !s->abandoned) {
            GTask *task = g_task_new(NULL, NULL, Complete, NULL);
            g_task_set_task_data(task, j, JobFree); g_task_run_in_thread(task, Worker);
            g_object_unref(task); j = NULL;
        }
    } else {
        char text[192];
        g_snprintf(text, sizeof text, "Review capture refused: %s. Check the selected card, evidence and current board.",
            umi_status_text(status));
        Message(s, text);
    }
    if (j) { s->pending = FALSE; JobFree(j); }
    g_object_unref(root);
}
static void ApplyClicked(GtkButton *button, gpointer data)
{
    CompletionPanel *s = State(data);
    if (!Active(s, GTK_WIDGET(button)) || !s->accepted || !s->accepted_ready || s->applied ||
        !gtk_check_button_get_active(GTK_CHECK_BUTTON(s->approval))) return;
    GtkWidget *root = g_object_ref(s->root);
    s->pending = TRUE;
    UmiStatus status = s->apply(s->context, s->accepted);
    if (status == UMI_STATUS_OK) s->applied = TRUE;
    if (Live(s)) {
        gtk_check_button_set_active(GTK_CHECK_BUTTON(s->approval), FALSE);
        Message(s, status == UMI_STATUS_OK
            ? "Reviewed moves applied. Save the board through its host; retain the review as historical evidence."
            : "Completion refused. Recheck the board, criteria and dependencies; the retained review file is unchanged.");
    }
    s->pending = FALSE; g_object_unref(root);
}
static void Discard(GtkButton *button, gpointer data)
{
    CompletionPanel *s = State(data);
    if (!Active(s, GTK_WIDGET(button))) return;
    GtkWidget *root = g_object_ref(s->root); s->pending = TRUE;
    UmiKanbanCompletionPlanDestroy(s->accepted); s->accepted = NULL; s->applied = FALSE; s->accepted_ready = FALSE;
    gtk_check_button_set_active(GTK_CHECK_BUTTON(s->approval), FALSE);
    if (Live(s)) gtk_text_buffer_set_text(s->preview, "", 0);
    Message(s, "Displayed review discarded. Its retained file remains unchanged.");
    s->pending = FALSE; g_object_unref(root);
}
static void Unmap(GtkWidget *widget, gpointer context)
{
    (void)widget; CompletionPanel *s = context;
    if (s->pending) s->abandoned = TRUE;
}
void UmiKanbanCompletionGtk4SetBusy(GtkWidget *root, gboolean busy)
{
    CompletionPanel *s = State(root); if (!s || s->retired) return;
    g_object_ref(root); s->busy = busy;
    if (busy) { s->abandoned = TRUE; UmiGtk4ReleaseSubtreeFocus(root); }
    gtk_widget_set_sensitive(root, !s->busy && !s->retired); g_object_unref(root);
}
void UmiKanbanCompletionGtk4Retire(GtkWidget *root)
{
    CompletionPanel *s = State(root); if (!s || s->retired) return;
    g_object_ref(root); s->retired = TRUE; s->abandoned = TRUE;
    UmiGtk4ReleaseSubtreeFocus(root); gtk_widget_set_sensitive(root, FALSE); g_object_unref(root);
}
gboolean UmiKanbanCompletionGtk4Pending(GtkWidget *root)
{
    CompletionPanel *s = State(root); return s && s->pending;
}
static GtkWidget *Entry(CompletionPanel *s, const char *label, const char *tag, int maximum)
{
    GtkWidget *caption = gtk_label_new(label); gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
    gtk_box_append(GTK_BOX(s->root), caption);
    GtkWidget *entry = gtk_entry_new(); g_object_ref_sink(entry);
    gtk_entry_set_max_length(GTK_ENTRY(entry), maximum);
    (void)umi_gtk4_automation_tag_widget(entry, tag); gtk_box_append(GTK_BOX(s->root), entry); return entry;
}
static void Button(CompletionPanel *s, GtkWidget *row, const char *label, const char *tag,
    GCallback callback, gboolean save)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button, tag);
    g_object_set_data(G_OBJECT(button), "completion-save", GINT_TO_POINTER(save));
    g_signal_connect_object(button, "clicked", callback, s->root, 0);
    gtk_box_append(GTK_BOX(row), button);
}
#ifdef UMICOM_KANBAN_SAVED_REPORT
#include "kanban_completion_report.inc"
#endif
GtkWidget *UmiKanbanCompletionGtk4Create(UmiKanbanCompletionCapture capture,
    UmiKanbanCompletionApply apply, gpointer context, GDestroyNotify destroy)
{
    if (!capture || !apply) return NULL;
    CompletionPanel *s = g_new0(CompletionPanel, 1);
    s->capture = capture; s->apply = apply; s->context = context; s->destroy = destroy;
    s->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    s->fields[0] = Entry(s, "Reviewer identifier", "kanban.completion.reviewer", 63);
    s->fields[1] = Entry(s, "Reviewed source identity", "kanban.completion.source", 255);
    s->fields[2] = Entry(s, "Reviewed configuration", "kanban.completion.configuration", 127);
    s->fields[3] = Entry(s, "Evidence SHA-256 (lowercase)", "kanban.completion.digest", 64);
    s->fields[4] = Entry(s, "Acceptance rationale or exception reason", "kanban.completion.rationale", 1023);
    s->exception = gtk_check_button_new_with_label("Record an explicit review exception");
    g_object_ref_sink(s->exception);
    (void)umi_gtk4_automation_tag_widget(s->exception, "kanban.completion.exception");
    gtk_box_append(GTK_BOX(s->root), s->exception);
    s->path = Entry(s, "New or existing private .umicompletion path", "kanban.completion.path", UMI_PATH_CAPACITY - 1);
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    Button(s, row, "Save selected card review", "kanban.completion.save", G_CALLBACK(Dispatch), TRUE);
    Button(s, row, "Load retained review", "kanban.completion.load", G_CALLBACK(Dispatch), FALSE);
    gtk_box_append(GTK_BOX(s->root), row);
    GtkWidget *view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE); gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    (void)umi_gtk4_automation_tag_widget(view, "kanban.completion.preview");
    s->preview = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
    GtkWidget *scroll = gtk_scrolled_window_new(); gtk_widget_set_size_request(scroll, -1, 180);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view); gtk_box_append(GTK_BOX(s->root), scroll);
    s->approval = gtk_check_button_new_with_label("I reviewed this retained decision and approve these Done moves");
    g_object_ref_sink(s->approval);
    (void)umi_gtk4_automation_tag_widget(s->approval, "kanban.completion.approval");
    gtk_box_append(GTK_BOX(s->root), s->approval);
    row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    Button(s, row, "Apply reviewed completion", "kanban.completion.apply", G_CALLBACK(ApplyClicked), FALSE);
    Button(s, row, "Discard displayed review", "kanban.completion.discard", G_CALLBACK(Discard), FALSE);
    gtk_box_append(GTK_BOX(s->root), row);
#ifdef UMICOM_KANBAN_SAVED_REPORT
    CreateReportEvidence(s);
#endif
    s->status = gtk_label_new("Select a card in Testing, review its acceptance, then retain the private decision before applying.");
    g_object_ref_sink(s->status); gtk_label_set_wrap(GTK_LABEL(s->status), TRUE);
    (void)umi_gtk4_automation_tag_widget(s->status, "kanban.completion.status");
    gtk_box_append(GTK_BOX(s->root), s->status);
    g_object_set_data_full(G_OBJECT(s->root), COMPLETION_KEY, s, FreePanel);
    g_signal_connect(s->root, "unmap", G_CALLBACK(Unmap), s); return s->root;
}
