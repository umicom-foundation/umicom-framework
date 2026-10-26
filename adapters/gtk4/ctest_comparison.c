/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ctest_comparison.c
 *
 * PURPOSE:
 *   Capture saved run pairs and publish complete accepted comparisons on the GTK owner thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/ui/gtk4/ctest_comparison.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/testing/ctest_comparison_file.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path.h"
#include <string.h>

typedef struct ComparisonPanel {
    GtkWidget *root, *before, *after, *before_source, *after_source, *status;
    GtkTextBuffer *buffer;
    UmiCtestReportComparison *comparison;
    char *rendered;
    size_t offset;
    gboolean busy, loading, cancelled;
} ComparisonPanel;
typedef struct ComparisonJob {
    GtkWidget *root;
    char before[UMI_PATH_CAPACITY], after[UMI_PATH_CAPACITY];
    UmiCtestReportComparison *comparison;
    UmiStatus status;
} ComparisonJob;

static void FreePanel(gpointer data)
{
    ComparisonPanel *s = data;
    UmiCtestReportComparisonDestroy(s->comparison);
    g_free(s->rendered);
    g_object_unref(s->before);
    g_object_unref(s->after);
    g_object_unref(s->before_source);
    g_object_unref(s->after_source);
    g_object_unref(s->status);
    g_object_unref(s->buffer);
    g_free(s);
}
static gboolean Active(ComparisonPanel *s, GtkWidget *button)
{
    return s && !s->busy && gtk_widget_get_mapped(s->root) &&
        gtk_widget_get_mapped(button) && gtk_widget_is_ancestor(button, s->root);
}
static UmiStatus Prepare(const UmiCtestReportComparison *comparison, size_t offset, char **out)
{
    *out = NULL;
    size_t bytes = 0;
    UmiStatus status = UmiCtestReportComparisonFormat(comparison, offset, 64U, NULL, 0, &bytes);
    if (status != UMI_STATUS_OK) return status;
    char *text = g_try_malloc(bytes + 1U);
    if (!text) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiCtestReportComparisonFormat(comparison, offset, 64U, text, bytes + 1U, &bytes);
    if (status == UMI_STATUS_OK) *out = text;
    else g_free(text);
    return status;
}
static void Publish(ComparisonPanel *s, char *text, size_t offset)
{
    g_free(s->rendered);
    s->rendered = text;
    s->offset = offset;
    gtk_text_buffer_set_text(s->buffer, text, -1);
}
static void Worker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    ComparisonJob *job = data;
    job->status = UmiCtestReportCompareFiles(job->before, job->after, &job->comparison);
    g_task_return_boolean(task, TRUE);
}
static void Complete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    GTask *task = G_TASK(result);
    ComparisonJob *job = g_task_get_task_data(task);
    ComparisonPanel *s = g_object_get_data(G_OBJECT(job->root), "umicom-ctest-comparison");
    (void)g_task_propagate_boolean(task, NULL);
    gboolean admitted = !s->cancelled && gtk_widget_get_mapped(job->root);
    s->loading = FALSE;
    char *text = NULL;
    if (admitted && job->status == UMI_STATUS_OK)
        job->status = Prepare(job->comparison, 0, &text);
    if (admitted && job->status == UMI_STATUS_OK) {
        UmiCtestReportComparisonDestroy(s->comparison);
        s->comparison = job->comparison;
        job->comparison = NULL;
        Publish(s, text, 0);
        text = NULL;
        /* These owned labels finish the accepted publication even when a
         * synchronous text observer hides or retires the embedding window. */
        gtk_label_set_text(GTK_LABEL(s->before_source), job->before);
        gtk_label_set_text(GTK_LABEL(s->after_source), job->after);
        gtk_label_set_text(GTK_LABEL(s->status),
            "Saved pair accepted. Removed tests are not repairs; inspect unchanged failures in the current report.");
    } else if (gtk_widget_get_mapped(job->root)) {
        if (s->cancelled)
            gtk_label_set_text(GTK_LABEL(s->status), "Comparison cancelled. The accepted pair is unchanged.");
        else {
            char *message = g_strdup_printf("Cannot compare the selected reports: %s. "
                "Choose two complete JUnit files. The accepted pair is unchanged.", umi_status_text(job->status));
            gtk_label_set_text(GTK_LABEL(s->status), message);
            g_free(message);
        }
    }
    g_free(text);
    s->busy = FALSE;
    UmiCtestReportComparisonDestroy(job->comparison);
    /* Job data owns GTK references, so release it here on the owner context,
     * not through a destructor which could run as the worker task is retired. */
    g_task_set_task_data(task, NULL, NULL);
    g_object_unref(job->root);
    g_free(job);
}
static void Open(GtkButton *button, gpointer data)
{
    ComparisonPanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-comparison");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    s->loading = TRUE;
    s->cancelled = FALSE;
    const char *before = gtk_editable_get_text(GTK_EDITABLE(s->before));
    const char *after = gtk_editable_get_text(GTK_EDITABLE(s->after));
    size_t left = strlen(before), right = strlen(after);
    UmiStatus status = left < UMI_PATH_CAPACITY && right < UMI_PATH_CAPACITY
        ? UmiOutputFileValidatePath(before) : UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(after);
    ComparisonJob *job = status == UMI_STATUS_OK ? g_try_new0(ComparisonJob, 1) : NULL;
    if (status == UMI_STATUS_OK && !job) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(s->status),
            "Enter two absolute local JUnit filenames. The accepted pair is unchanged.");
        s->loading = FALSE;
        s->busy = FALSE;
        g_object_unref(s->root);
        return;
    }
    memcpy(job->before, before, left + 1U);
    memcpy(job->after, after, right + 1U);
    job->root = s->root;
    /* All input is copied before notification handlers can change either entry. */
    gtk_label_set_text(GTK_LABEL(s->status), "Reading and comparing the selected saved reports…");
    if (s->cancelled || !gtk_widget_get_mapped(s->root)) {
        s->loading = FALSE;
        s->busy = FALSE;
        g_object_unref(job->root);
        g_free(job);
        return;
    }
    GTask *task = g_task_new(NULL, NULL, Complete, NULL);
    g_task_set_task_data(task, job, NULL);
    g_task_run_in_thread(task, Worker);
    g_object_unref(task);
}
static void Cancel(GtkButton *button, gpointer data)
{
    ComparisonPanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-comparison");
    if (!s || !s->loading || !gtk_widget_get_mapped(s->root) ||
        !gtk_widget_get_mapped(GTK_WIDGET(button)) ||
        !gtk_widget_is_ancestor(GTK_WIDGET(button), s->root)) return;
    g_object_ref(s->root);
    s->cancelled = TRUE;
    gtk_label_set_text(GTK_LABEL(s->status),
        "Cancellation requested. Waiting for the local reads to release their resources.");
    g_object_unref(s->root);
}
static void Unmapped(GtkWidget *widget, gpointer data)
{
    (void)widget;
    ComparisonPanel *s = data;
    if (s->loading) s->cancelled = TRUE;
}
static void Page(GtkButton *button, gpointer data)
{
    ComparisonPanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-comparison");
    if (!Active(s, GTK_WIDGET(button)) || !s->comparison) return;
    UmiCtestReportComparisonSummary summary;
    if (UmiCtestReportComparisonReadSummary(s->comparison, &summary) != UMI_STATUS_OK) return;
    size_t offset = s->offset;
    int direction = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "comparison-direction"));
    if (direction < 0) {
        if (!offset) return;
        offset = offset >= 64U ? offset - 64U : 0;
    } else {
        if (summary.changed - offset <= 64U) return;
        offset += 64U;
    }
    g_object_ref(s->root);
    s->busy = TRUE;
    char *text = NULL;
    UmiStatus status = Prepare(s->comparison, offset, &text);
    if (status == UMI_STATUS_OK) Publish(s, text, offset);
    else gtk_label_set_text(GTK_LABEL(s->status), "Unable to prepare that page. The accepted page is unchanged.");
    s->busy = FALSE;
    g_object_unref(s->root);
}
static void Copy(GtkButton *button, gpointer data)
{
    ComparisonPanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-comparison");
    if (!Active(s, GTK_WIDGET(button)) || !s->rendered) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    gdk_clipboard_set_text(gtk_widget_get_clipboard(s->root), s->rendered);
    s->busy = FALSE;
    g_object_unref(s->root);
}
static void Clear(GtkButton *button, gpointer data)
{
    ComparisonPanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-comparison");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    UmiCtestReportComparisonDestroy(s->comparison);
    s->comparison = NULL;
    g_clear_pointer(&s->rendered, g_free);
    s->offset = 0;
    gtk_text_buffer_set_text(s->buffer, "", -1);
    gtk_label_set_text(GTK_LABEL(s->before_source), "No baseline accepted");
    gtk_label_set_text(GTK_LABEL(s->after_source), "No current report accepted");
    gtk_label_set_text(GTK_LABEL(s->status), "Saved comparison closed; both source files are unchanged.");
    s->busy = FALSE;
    g_object_unref(s->root);
}
gboolean UmiGtk4CtestComparisonPending(GtkWidget *panel)
{
    if (!GTK_IS_WIDGET(panel)) return FALSE;
    ComparisonPanel *s = g_object_get_data(G_OBJECT(panel), "umicom-ctest-comparison");
    return s && s->busy;
}
static GtkWidget *Label(ComparisonPanel *s, const char *text, const char *tag)
{
    GtkWidget *label = gtk_label_new(text);
    g_object_ref_sink(label);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    (void)umi_gtk4_automation_tag_widget(label, tag);
    gtk_box_append(GTK_BOX(s->root), label);
    return label;
}
static GtkWidget *Entry(ComparisonPanel *s, const char *hint, const char *tag)
{
    GtkWidget *entry = gtk_entry_new();
    g_object_ref_sink(entry);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), hint);
    (void)umi_gtk4_automation_tag_widget(entry, tag);
    gtk_box_append(GTK_BOX(s->root), entry);
    GtkWidget *picker = UmiPathPickerGtk4Create(GTK_ENTRY(entry),
        UMI_PATH_PICKER_EXISTING_FILE, hint, NULL);
    if (picker) gtk_box_append(GTK_BOX(s->root), picker);
    return entry;
}
static void Button(ComparisonPanel *s, GtkWidget *row, const char *label, const char *tag,
    GCallback callback, int direction)
{
    GtkWidget *button = gtk_button_new_with_label(label);
    (void)umi_gtk4_automation_tag_widget(button, tag);
    g_object_set_data(G_OBJECT(button), "comparison-direction", GINT_TO_POINTER(direction));
    g_signal_connect_object(button, "clicked", callback, s->root, 0);
    gtk_box_append(GTK_BOX(row), button);
}
GtkWidget *UmiGtk4CtestComparisonCreate(void)
{
    ComparisonPanel *s = g_new0(ComparisonPanel, 1);
    s->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    s->before = Entry(s, "Baseline JUnit file (absolute path)", "ctest.comparison.before");
    s->after = Entry(s, "Current JUnit file (absolute path)", "ctest.comparison.after");
    s->before_source = Label(s, "No baseline accepted", "ctest.comparison.before-source");
    s->after_source = Label(s, "No current report accepted", "ctest.comparison.after-source");
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    Button(s, row, "Compare reports", "ctest.comparison.open", G_CALLBACK(Open), 0);
    Button(s, row, "Cancel load", "ctest.comparison.cancel", G_CALLBACK(Cancel), 0);
    Button(s, row, "Previous", "ctest.comparison.previous", G_CALLBACK(Page), -1);
    Button(s, row, "Next", "ctest.comparison.next", G_CALLBACK(Page), 1);
    Button(s, row, "Copy page", "ctest.comparison.copy", G_CALLBACK(Copy), 0);
    Button(s, row, "Close comparison", "ctest.comparison.clear", G_CALLBACK(Clear), 0);
    GtkWidget *actions = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(actions), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(actions), row);
    gtk_box_append(GTK_BOX(s->root), actions);
    s->buffer = gtk_text_buffer_new(NULL);
    GtkWidget *view = gtk_text_view_new_with_buffer(s->buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    (void)umi_gtk4_automation_tag_widget(view, "ctest.comparison.output");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 220);
    gtk_box_append(GTK_BOX(s->root), scroll);
    s->status = Label(s, "Compare saved outcomes. This does not execute tests or prove a source revision passed.",
        "ctest.comparison.status");
    (void)umi_gtk4_automation_tag_widget(s->root, "ctest.comparison.panel");
    g_object_set_data_full(G_OBJECT(s->root), "umicom-ctest-comparison", s, FreePanel);
    g_signal_connect(s->root, "unmap", G_CALLBACK(Unmapped), s);
    return s->root;
}
