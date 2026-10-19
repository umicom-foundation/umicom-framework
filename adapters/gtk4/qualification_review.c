/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/qualification_review.c
 * Purpose: Render immutable qualification pages with explicit missing evidence and persistent blockers.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/qualification_review.h"
#include "umicom/distribution/runtime/qualification_report.h"
#include <inttypes.h>
#include <string.h>
#define REVIEW_PAGE_SIZE 64U
#define REVIEW_KEY "umicom-qualification-review"
typedef struct Review {
    UmiQualificationView *view;
    GtkWidget *summary, *identity, *blockers, *rows, *filter, *previous, *next, *page;
    size_t offset, visible, total;
} Review;
static Review *State(GtkWidget *widget)
{
    return widget != NULL && GTK_IS_BOX(widget) ? g_object_get_data(G_OBJECT(widget), REVIEW_KEY) : NULL;
}
static void Destroy(gpointer data)
{
    Review *state = data;
    UmiQualificationViewDestroy(state->view); g_free(state);
}
static GtkWidget *Label(const char *text)
{
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0F);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    return label;
}
/* Keep user-controlled names out of markup. A visible clipping notice avoids
 * presenting a shortened label as the exact executable/test identity. */
static char *DisplayName(const char *text)
{
    GString *escaped = g_string_new(NULL);
    size_t length = strlen(text), shown = length < 512U ? length : 512U;
    for (size_t i = 0U; i < shown; ++i) {
        unsigned char value = (unsigned char)text[i];
        if (value < 32U || value == 127U) g_string_append_printf(escaped, "\\x%02x", (unsigned)value);
        else g_string_append_c(escaped, (char)value);
    }
    if (shown != length) g_string_append(escaped, " … (display shortened)");
    char *valid = g_utf8_make_valid(escaped->str, (gssize)escaped->len);
    g_string_free(escaped, TRUE);
    return valid;
}
static void Render(Review *state)
{
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(state->rows)) != NULL)
        gtk_box_remove(GTK_BOX(state->rows), child);
    state->visible = 0U; state->total = 0U;
    if (state->view == NULL) {
        state->offset = 0U;
        gtk_label_set_text(GTK_LABEL(state->summary), "No qualification record");
        gtk_label_set_text(GTK_LABEL(state->identity), "");
        gtk_label_set_text(GTK_LABEL(state->blockers), "Capture a run to review its selected work and recorded outcomes.");
        gtk_label_set_text(GTK_LABEL(state->page), "No rows");
        gtk_widget_set_sensitive(state->previous, FALSE); gtk_widget_set_sensitive(state->next, FALSE);
        return;
    }
    UmiQualificationViewSummary summary;
    (void)UmiQualificationViewDescribe(state->view, &summary);
    char *text = g_strdup_printf("%s\n%zu targets selected · %zu of %zu test results recorded\n"
        "%zu passed · %zu failed · %zu skipped · %zu disabled · %zu missing · %zu not run · %zu cancelled · %zu timed out",
        UmiQualificationViewStateText(state->view), summary.selection.targets,
        summary.results.recorded, summary.results.tests, summary.results.passed, summary.results.failed,
        summary.results.skipped, summary.results.disabled, summary.results.missing,
        summary.results.notRun, summary.results.cancelled, summary.results.timedOut);
    gtk_label_set_text(GTK_LABEL(state->summary), text); g_free(text);
    text = g_strdup_printf("Attempt: %s\nPlan: %s\nObserved sources: %s",
        summary.identity.execution, summary.identity.plan, summary.source);
    gtk_label_set_text(GTK_LABEL(state->identity), text); g_free(text);
    GString *blockers = g_string_new(NULL);
    for (uint32_t bit = 1U; bit <= UMI_QUALIFICATION_EXECUTION_INTERRUPTED; bit <<= 1U) {
        if ((summary.results.blockers & bit) != 0U) {
            if (blockers->len != 0U) g_string_append_c(blockers, '\n');
            g_string_append(blockers, UmiReleaseQualificationBlockerText((UmiReleaseQualificationBlocker)bit));
        }
    }
    if (summary.results.executionStatus != UMI_STATUS_OK)
        g_string_append_printf(blockers, "\nExecution status: %s", umi_status_text(summary.results.executionStatus));
    if (summary.selection.fallbackReasons != 0U)
        g_string_append_printf(blockers, "\nCoverage widened because dependency knowledge was incomplete or full coverage was requested (reasons: 0x%x).",
            (unsigned)summary.selection.fallbackReasons);
    if (summary.results.readyForOwnerReview)
        g_string_append(blockers, "Scoped evidence is ready for review. This does not certify a release or authorize publication.");
    if (summary.savedReport)
        g_string_append(blockers, "\nSaved report: current source files and tools have not been checked. The file is not a signed execution receipt.");
    g_string_append(blockers, "\nBefore/after source observations are not an atomic source snapshot.");
    gtk_label_set_text(GTK_LABEL(state->blockers), blockers->str); g_string_free(blockers, TRUE);
    UmiQualificationViewRow rows[REVIEW_PAGE_SIZE];
    guint selected = gtk_drop_down_get_selected(GTK_DROP_DOWN(state->filter));
    UmiStatus status = UmiQualificationViewPage(state->view, (UmiQualificationViewFilter)selected,
        state->offset, rows, REVIEW_PAGE_SIZE, &state->visible, &state->total);
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(state->page), "The selected page is unavailable.");
        gtk_widget_set_sensitive(state->previous, FALSE); gtk_widget_set_sensitive(state->next, FALSE);
        return;
    }
    for (size_t i = 0U; i < state->visible; ++i) {
        char *name = DisplayName(rows[i].name);
        GtkWidget *row = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_box_append(GTK_BOX(row), Label(name)); g_free(name);
        text = g_strdup_printf("%s · selection reasons: 0x%x",
            UmiQualificationViewRowStateText(&rows[i]), (unsigned)rows[i].reasons);
        gtk_box_append(GTK_BOX(row), Label(text)); g_free(text);
        if (rows[i].recorded) {
            char exit[40] = "unknown";
            if (rows[i].result.exitCodeKnown)
                g_snprintf(exit, sizeof(exit), "%d", rows[i].result.exitCode);
            text = g_strdup_printf("Status: %s · exit: %s · %" PRIu64 " ms\nLog SHA-256: %s",
                umi_status_text(rows[i].result.status), exit, rows[i].result.durationMs, rows[i].result.logSha256);
            gtk_box_append(GTK_BOX(row), Label(text)); g_free(text);
        }
        gtk_box_append(GTK_BOX(state->rows), row);
    }
    text = state->total == 0U ? g_strdup("No rows match this filter; review the summary and blockers above.") :
        g_strdup_printf("Rows %zu–%zu of %zu", state->offset + 1U, state->offset + state->visible, state->total);
    gtk_label_set_text(GTK_LABEL(state->page), text); g_free(text);
    gtk_widget_set_sensitive(state->previous, state->offset != 0U);
    gtk_widget_set_sensitive(state->next, state->offset + state->visible < state->total);
}
static void FilterChanged(GObject *object, GParamSpec *property, gpointer data)
{
    (void)object; (void)property;
    Review *state = State(GTK_WIDGET(data));
    if (state != NULL) { state->offset = 0U; Render(state); }
}
static void Previous(GtkButton *button, gpointer data)
{
    (void)button; Review *state = State(GTK_WIDGET(data));
    if (state != NULL) { state->offset = state->offset > REVIEW_PAGE_SIZE ? state->offset - REVIEW_PAGE_SIZE : 0U; Render(state); }
}
static void Next(GtkButton *button, gpointer data)
{
    (void)button; Review *state = State(GTK_WIDGET(data));
    if (state != NULL && state->offset + state->visible < state->total) {
        state->offset += state->visible; Render(state);
    }
}
GtkWidget *UmiGtk4QualificationReviewCreate(void)
{
    GtkWidget *root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    Review *state = g_new0(Review, 1);
    g_object_set_data_full(G_OBJECT(root), REVIEW_KEY, state, Destroy);
    state->summary = Label("No qualification record");
    state->identity = Label(""); state->blockers = Label("");
    gtk_box_append(GTK_BOX(root), state->summary);
    GtkWidget *identity = gtk_expander_new("Run identity");
    gtk_expander_set_child(GTK_EXPANDER(identity), state->identity);
    gtk_box_append(GTK_BOX(root), identity); gtk_box_append(GTK_BOX(root), state->blockers);
    const char *filters[] = {"All records", "Non-passing outcomes", "Missing required results", "Tests", "Selected targets", NULL};
    state->filter = gtk_drop_down_new_from_strings(filters);
    gtk_widget_set_name(state->filter, "qualification-filter");
    gtk_widget_set_tooltip_text(state->filter, "Filter recorded rows; summary blockers remain visible.");
    gtk_box_append(GTK_BOX(root), state->filter);
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scroll, TRUE); gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 240);
    state->rows = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), state->rows);
    gtk_box_append(GTK_BOX(root), scroll);
    GtkWidget *navigation = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    state->previous = gtk_button_new_with_label("Previous"); state->next = gtk_button_new_with_label("Next");
    gtk_widget_set_name(state->previous, "qualification-previous"); gtk_widget_set_name(state->next, "qualification-next");
    state->page = Label("");
    gtk_box_append(GTK_BOX(navigation), state->previous); gtk_box_append(GTK_BOX(navigation), state->next);
    gtk_box_append(GTK_BOX(navigation), state->page); gtk_box_append(GTK_BOX(root), navigation);
    /* Object-bound connections disconnect when root dies even if a host retains
     * a child widget. Callbacks never borrow a worker or a freed model owner. */
    g_signal_connect_object(state->filter, "notify::selected", G_CALLBACK(FilterChanged), root, 0);
    g_signal_connect_object(state->previous, "clicked", G_CALLBACK(Previous), root, 0);
    g_signal_connect_object(state->next, "clicked", G_CALLBACK(Next), root, 0);
    Render(state); return root;
}
UmiStatus UmiGtk4QualificationReviewSetView(GtkWidget *widget, UmiQualificationView **view)
{
    Review *state = State(widget);
    if (state == NULL || view == NULL || (*view != NULL && *view == state->view))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the panel alive if a host observer drops its own reference while
     * GTK emits property notifications during refresh. */
    g_object_ref(widget);
    UmiQualificationView *previous = state->view;
    state->view = *view; *view = NULL; state->offset = 0U;
    Render(state); UmiQualificationViewDestroy(previous);
    g_object_unref(widget);
    return UMI_STATUS_OK;
}
UmiStatus UmiGtk4QualificationReviewReadPage(GtkWidget *widget,
    size_t *outOffset, size_t *outVisible, size_t *outTotal)
{
    Review *state = State(widget);
    if (state == NULL || outOffset == NULL || outVisible == NULL || outTotal == NULL ||
        outOffset == outVisible || outOffset == outTotal || outVisible == outTotal)
        return UMI_STATUS_INVALID_ARGUMENT;
    *outOffset = state->offset; *outVisible = state->visible; *outTotal = state->total;
    return UMI_STATUS_OK;
}
