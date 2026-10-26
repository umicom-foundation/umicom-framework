/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ctest_coverage.c
 *
 * PURPOSE:
 *   Present immutable host coverage with captured inputs and guarded asynchronous replacement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/ctest_coverage.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/testing/ctest_coverage_manifest.h"
#include "umicom/testing/ctest_coverage_view.h"
#include "umicom/platform/output_file.h"
#include "umicom/platform/path.h"
#include <string.h>

typedef struct CoveragePanel {
    GtkWidget *root, *path, *identity, *status, *source;
    GtkTextBuffer *buffer;
    UmiCtestCoverageReview *review;
    UmiCtestCoverageView *view;
    UmiCtestCoverageFilter filter;
    GtkWidget *query, *host, *configuration, *outcome, *order;
    char *rendered;
    size_t offset;
    gboolean busy, cancelled;
    gboolean loading; /* Cancellation ends before accepted GTK publication begins. */
} CoveragePanel;

typedef struct CoverageJob {
    GtkWidget *root;
    char path[UMI_PATH_CAPACITY];
    char identity[UMI_CTEST_COVERAGE_IDENTITY_CAPACITY];
    UmiCtestCoverageReview *review;
    UmiStatus status;
} CoverageJob;

static void FreePanel(gpointer data)
{
    CoveragePanel *s = data;
    UmiCtestCoverageViewDestroy(s->view);
    UmiCtestCoverageReviewDestroy(s->review);
    g_object_unref(s->query); g_object_unref(s->host); g_object_unref(s->configuration);
    g_object_unref(s->outcome); g_object_unref(s->order);
    g_free(s->rendered);
    g_object_unref(s->path);
    g_object_unref(s->identity);
    g_object_unref(s->status);
    g_object_unref(s->source);
    g_object_unref(s->buffer);
    g_free(s);
}
static gboolean Active(CoveragePanel *s, GtkWidget *button)
{
    return s && !s->busy && gtk_widget_get_mapped(s->root) &&
        gtk_widget_get_mapped(button) && gtk_widget_is_ancestor(button, s->root);
}
/* The former full-review formatter is retained. The shared view now formats selected rows while keeping whole-review context. */
#if 0
static UmiStatus PrepareText(const UmiCtestCoverageReview *review, size_t offset, char **out)
{
    *out = NULL;
    size_t bytes = 0;
    UmiStatus status = UmiCtestCoverageReviewFormat(review, offset, 64U, NULL, 0, &bytes);
    if (status != UMI_STATUS_OK) return status;
    char *text = g_try_malloc(bytes + 1U);
    if (!text) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiCtestCoverageReviewFormat(review, offset, 64U, text, bytes + 1U, &bytes);
    if (status == UMI_STATUS_OK) *out = text;
    else g_free(text);
    return status;
}
#endif
static UmiStatus PrepareText(const UmiCtestCoverageView *review, size_t offset, char **out)
{
    *out = NULL;
    size_t bytes = 0;
    UmiStatus status = UmiCtestCoverageViewFormat(review, offset, 64U, NULL, 0, &bytes);
    if (status != UMI_STATUS_OK) return status;
    char *text = g_try_malloc(bytes + 1U);
    if (!text) return UMI_STATUS_OUT_OF_MEMORY;
    status = UmiCtestCoverageViewFormat(review, offset, 64U, text, bytes + 1U, &bytes);
    if (status == UMI_STATUS_OK) *out = text;
    else g_free(text);
    return status;
}
/* The model and its accepted text are published before GTK notifications.
 * Root and child references keep cleanup valid even when an observer closes
 * the embedding window from a buffer or label notification. */
static void PublishText(CoveragePanel *s, char *text, size_t offset)
{
    g_free(s->rendered);
    s->rendered = text;
    s->offset = offset;
    gtk_text_buffer_set_text(s->buffer, text, -1);
}
static void ReadWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel)
{
    (void)source; (void)cancel;
    CoverageJob *job = data;
    job->status = UmiCtestCoverageReviewReadFile(job->path, job->identity, &job->review);
    g_task_return_boolean(task, TRUE);
}
/* Cancellation applies to the pending read, before accepted-state publication.
 * Retain the previous busy-based boundary for review; the replacement completes
 * owned widget metadata even if a notification hides or retires the host. */
#if 0
static void ReadComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    GTask *task = G_TASK(result);
    CoverageJob *job = g_task_get_task_data(task);
    CoveragePanel *s = g_object_get_data(G_OBJECT(job->root), "umicom-ctest-coverage");
    (void)g_task_propagate_boolean(task, NULL);
    char *text = NULL;
    gboolean accepted = !s->cancelled && gtk_widget_get_mapped(job->root);
    if (accepted && job->status == UMI_STATUS_OK)
        job->status = PrepareText(job->review, 0, &text);
    if (accepted && job->status == UMI_STATUS_OK) {
        UmiCtestCoverageReview *previous = s->review;
        s->review = job->review;
        job->review = NULL;
        UmiCtestCoverageReviewDestroy(previous);
        PublishText(s, text, 0);
        text = NULL;
        if (!s->cancelled && gtk_widget_get_mapped(job->root))
            gtk_label_set_text(GTK_LABEL(s->source), job->path);
        if (!s->cancelled && gtk_widget_get_mapped(job->root))
            gtk_label_set_text(GTK_LABEL(s->status),
                "Saved coverage loaded. Required rows alone determine this review; "
                "no current source or host capability was verified.");
    } else if (gtk_widget_get_mapped(job->root)) {
        if (s->cancelled)
            gtk_label_set_text(GTK_LABEL(s->status), "Load cancelled. The accepted review is unchanged.");
        else {
            char *message = g_strdup_printf("Coverage could not be loaded: %s. "
                "Check the CSV plan, source identity and relative JUnit report paths. "
                "The accepted review is unchanged.", umi_status_text(job->status));
            gtk_label_set_text(GTK_LABEL(s->status), message);
            g_free(message);
        }
    }
    g_free(text);
    s->busy = FALSE;
    UmiCtestCoverageReviewDestroy(job->review);
    /* GTK references are released on this completion context, never by a
     * worker's task-data destructor. */
    g_task_set_task_data(task, NULL, NULL);
    g_object_unref(job->root);
    g_free(job);
}
#endif
/* The previous accepted-review publication remains for review. Destroy borrowed views before replacing their owned reviews; accept the new index and text together. */
#if 0
static void ReadComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    GTask *task = G_TASK(result);
    CoverageJob *job = g_task_get_task_data(task);
    CoveragePanel *s = g_object_get_data(G_OBJECT(job->root), "umicom-ctest-coverage");
    (void)g_task_propagate_boolean(task, NULL);
    char *text = NULL;
    gboolean accepted = !s->cancelled && gtk_widget_get_mapped(job->root);
    s->loading = FALSE;
    if (accepted && job->status == UMI_STATUS_OK)
        job->status = PrepareText(job->review, 0, &text);
    if (accepted && job->status == UMI_STATUS_OK) {
        UmiCtestCoverageReview *previous = s->review;
        s->review = job->review;
        job->review = NULL;
        UmiCtestCoverageReviewDestroy(previous);
        PublishText(s, text, 0);
        text = NULL;
        /* Owned children survive notification-driven host closure. Complete
         * accepted text and filename together, even if the host is now hidden. */
        gtk_label_set_text(GTK_LABEL(s->source), job->path);
        gtk_label_set_text(GTK_LABEL(s->status),
                "Saved coverage loaded. Required rows alone determine this review; "
                "no current source or host capability was verified.");
    } else if (gtk_widget_get_mapped(job->root)) {
        if (s->cancelled)
            gtk_label_set_text(GTK_LABEL(s->status), "Load cancelled. The accepted review is unchanged.");
        else {
            char *message = g_strdup_printf("Coverage could not be loaded: %s. "
                "Check the CSV plan, source identity and relative JUnit report paths. "
                "The accepted review is unchanged.", umi_status_text(job->status));
            gtk_label_set_text(GTK_LABEL(s->status), message);
            g_free(message);
        }
    }
    g_free(text);
    s->busy = FALSE;
    UmiCtestCoverageReviewDestroy(job->review);
    /* GTK references are released on this completion context, never by a
     * worker's task-data destructor. */
    g_task_set_task_data(task, NULL, NULL);
    g_object_unref(job->root);
    g_free(job);
}
#endif
static void ReadComplete(GObject *source, GAsyncResult *result, gpointer data)
{
    (void)source; (void)data;
    GTask *task = G_TASK(result);
    CoverageJob *job = g_task_get_task_data(task);
    CoveragePanel *s = g_object_get_data(G_OBJECT(job->root), "umicom-ctest-coverage");
    (void)g_task_propagate_boolean(task, NULL);
    char *text = NULL;
    UmiCtestCoverageView *view = NULL;
    gboolean accepted = !s->cancelled && gtk_widget_get_mapped(job->root);
    s->loading = FALSE;
    if (accepted && job->status == UMI_STATUS_OK)
        job->status = UmiCtestCoverageViewCreate(job->review, &s->filter, &view);
    if (accepted && job->status == UMI_STATUS_OK)
        job->status = PrepareText(view, 0, &text);
    if (accepted && job->status == UMI_STATUS_OK) {
        UmiCtestCoverageViewDestroy(s->view); s->view = view; view = NULL;
        UmiCtestCoverageReview *previous = s->review;
        s->review = job->review;
        job->review = NULL;
        UmiCtestCoverageReviewDestroy(previous);
        PublishText(s, text, 0);
        text = NULL;
        /* Owned children survive notification-driven host closure. Complete
         * accepted text and filename together, even if the host is now hidden. */
        gtk_label_set_text(GTK_LABEL(s->source), job->path);
        gtk_label_set_text(GTK_LABEL(s->status),
                "Saved coverage loaded. Required rows alone determine this review; "
                "no current source or host capability was verified.");
    } else if (gtk_widget_get_mapped(job->root)) {
        if (s->cancelled)
            gtk_label_set_text(GTK_LABEL(s->status), "Load cancelled. The accepted review is unchanged.");
        else {
            char *message = g_strdup_printf("Coverage could not be loaded: %s. "
                "Check the CSV plan, source identity and relative JUnit report paths. "
                "The accepted review is unchanged.", umi_status_text(job->status));
            gtk_label_set_text(GTK_LABEL(s->status), message);
            g_free(message);
        }
    }
    g_free(text);
    UmiCtestCoverageViewDestroy(view);
    s->busy = FALSE;
    UmiCtestCoverageReviewDestroy(job->review);
    /* GTK references are released on this completion context, never by a
     * worker's task-data destructor. */
    g_task_set_task_data(task, NULL, NULL);
    g_object_unref(job->root);
    g_free(job);
}
/* Cancellation applies to the pending read, before accepted-state publication.
 * Retain the previous busy-based boundary for review; the replacement completes
 * owned widget metadata even if a notification hides or retires the host. */
#if 0
static void Open(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    s->cancelled = FALSE;
    const char *path = gtk_editable_get_text(GTK_EDITABLE(s->path));
    const char *identity = gtk_editable_get_text(GTK_EDITABLE(s->identity));
    size_t path_bytes = strlen(path), identity_bytes = strlen(identity);
    UmiStatus status = UMI_STATUS_OK;
    char escaped[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    if (path_bytes >= UMI_PATH_CAPACITY || identity_bytes >= UMI_CTEST_COVERAGE_IDENTITY_CAPACITY)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(path);
    if (status == UMI_STATUS_OK) status = UmiCtestEscapeName(identity, escaped, sizeof escaped);
    CoverageJob *job = status == UMI_STATUS_OK ? g_try_new0(CoverageJob, 1) : NULL;
    if (status == UMI_STATUS_OK && !job) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(s->status),
            "Enter an absolute manifest filename and a nonempty printable source identity. "
            "The accepted review is unchanged.");
        s->busy = FALSE;
        g_object_unref(s->root);
        return;
    }
    memcpy(job->path, path, path_bytes + 1U);
    memcpy(job->identity, identity, identity_bytes + 1U);
    job->root = s->root; /* Transfer the retained root to owner-thread completion. */
    gtk_label_set_text(GTK_LABEL(s->status), "Reading saved host coverage…");
    if (s->cancelled || !gtk_widget_get_mapped(s->root)) {
        s->busy = FALSE;
        g_object_unref(job->root);
        g_free(job);
        return;
    }
    GTask *task = g_task_new(NULL, NULL, ReadComplete, NULL);
    g_task_set_task_data(task, job, NULL);
    g_task_run_in_thread(task, ReadWorker);
    g_object_unref(task);
}
#endif
static void Open(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    s->loading = TRUE;
    s->cancelled = FALSE;
    const char *path = gtk_editable_get_text(GTK_EDITABLE(s->path));
    const char *identity = gtk_editable_get_text(GTK_EDITABLE(s->identity));
    size_t path_bytes = strlen(path), identity_bytes = strlen(identity);
    UmiStatus status = UMI_STATUS_OK;
    char escaped[UMI_CTEST_EXACT_PATTERN_CAPACITY];
    if (path_bytes >= UMI_PATH_CAPACITY || identity_bytes >= UMI_CTEST_COVERAGE_IDENTITY_CAPACITY)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
    if (status == UMI_STATUS_OK) status = UmiOutputFileValidatePath(path);
    if (status == UMI_STATUS_OK) status = UmiCtestEscapeName(identity, escaped, sizeof escaped);
    CoverageJob *job = status == UMI_STATUS_OK ? g_try_new0(CoverageJob, 1) : NULL;
    if (status == UMI_STATUS_OK && !job) status = UMI_STATUS_OUT_OF_MEMORY;
    if (status != UMI_STATUS_OK) {
        gtk_label_set_text(GTK_LABEL(s->status),
            "Enter an absolute manifest filename and a nonempty printable source identity. "
            "The accepted review is unchanged.");
        s->loading = FALSE;
        s->busy = FALSE;
        g_object_unref(s->root);
        return;
    }
    memcpy(job->path, path, path_bytes + 1U);
    memcpy(job->identity, identity, identity_bytes + 1U);
    job->root = s->root; /* Transfer the retained root to owner-thread completion. */
    gtk_label_set_text(GTK_LABEL(s->status), "Reading saved host coverage…");
    if (s->cancelled || !gtk_widget_get_mapped(s->root)) {
        s->loading = FALSE;
        s->busy = FALSE;
        g_object_unref(job->root);
        g_free(job);
        return;
    }
    GTask *task = g_task_new(NULL, NULL, ReadComplete, NULL);
    g_task_set_task_data(task, job, NULL);
    g_task_run_in_thread(task, ReadWorker);
    g_object_unref(task);
}
/* Cancellation applies to the pending read, before accepted-state publication.
 * Retain the previous busy-based boundary for review; the replacement completes
 * owned widget metadata even if a notification hides or retires the host. */
#if 0
static void Cancel(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!s || !s->busy || !gtk_widget_get_mapped(s->root) ||
        !gtk_widget_is_ancestor(GTK_WIDGET(button), s->root)) return;
    g_object_ref(s->root);
    s->cancelled = TRUE;
    gtk_label_set_text(GTK_LABEL(s->status),
        "Cancellation requested. An in-progress file read finishes without replacing the review.");
    g_object_unref(s->root);
}
#endif
static void Cancel(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!s || !s->loading || !gtk_widget_get_mapped(s->root) ||
        !gtk_widget_is_ancestor(GTK_WIDGET(button), s->root)) return;
    g_object_ref(s->root);
    s->cancelled = TRUE;
    gtk_label_set_text(GTK_LABEL(s->status),
        "Cancellation requested. An in-progress file read finishes without replacing the review.");
    g_object_unref(s->root);
}
/* The full-review paging implementation is retained. Navigation now follows the accepted filtered index, never the hidden source row count. */
#if 0
static void Page(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button)) || !s->review) return;
    UmiCtestCoverageSummary summary;
    if (UmiCtestCoverageReviewRead(s->review, &summary) != UMI_STATUS_OK) return;
    int direction = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "coverage-direction"));
    size_t offset = s->offset;
    if (direction < 0) {
        if (!offset) return;
        offset = offset >= 64U ? offset - 64U : 0;
    } else {
        if (summary.requirements - offset <= 64U) return;
        offset += 64U;
    }
    g_object_ref(s->root);
    s->busy = TRUE;
    char *text = NULL;
    UmiStatus status = PrepareText(s->review, offset, &text);
    if (status == UMI_STATUS_OK) PublishText(s, text, offset);
    else if (gtk_widget_get_mapped(s->root))
        gtk_label_set_text(GTK_LABEL(s->status), "Unable to prepare that page. The current page is unchanged.");
    s->busy = FALSE;
    g_object_unref(s->root);
}
#endif
static void Page(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button)) || !s->review) return;
    UmiCtestCoverageViewSummary summary;
    if (UmiCtestCoverageViewRead(s->view, &summary) != UMI_STATUS_OK) return;
    int direction = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "coverage-direction"));
    size_t offset = s->offset;
    if (direction < 0) {
        if (!offset) return;
        offset = offset >= 64U ? offset - 64U : 0;
    } else {
        if (summary.matches - offset <= 64U) return;
        offset += 64U;
    }
    g_object_ref(s->root);
    s->busy = TRUE;
    char *text = NULL;
    UmiStatus status = PrepareText(s->view, offset, &text);
    if (status == UMI_STATUS_OK) PublishText(s, text, offset);
    else if (gtk_widget_get_mapped(s->root))
        gtk_label_set_text(GTK_LABEL(s->status), "Unable to prepare that page. The current page is unchanged.");
    s->busy = FALSE;
    g_object_unref(s->root);
}
static void Copy(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button)) || !s->rendered) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    gdk_clipboard_set_text(gtk_widget_get_clipboard(s->root), s->rendered);
    s->busy = FALSE;
    g_object_unref(s->root);
}
/* Cancellation applies to the pending read, before accepted-state publication.
 * Retain the previous busy-based boundary for review; the replacement completes
 * owned widget metadata even if a notification hides or retires the host. */
#if 0
static void Clear(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    UmiCtestCoverageReviewDestroy(s->review);
    s->review = NULL;
    g_clear_pointer(&s->rendered, g_free);
    s->offset = 0;
    gtk_text_buffer_set_text(s->buffer, "", -1);
    if (gtk_widget_get_mapped(s->root)) gtk_label_set_text(GTK_LABEL(s->source), "No coverage loaded");
    if (gtk_widget_get_mapped(s->root)) gtk_label_set_text(GTK_LABEL(s->status), "Saved review closed.");
    s->busy = FALSE;
    g_object_unref(s->root);
}
#endif
/* The previous close implementation is retained. Borrowed filtered views must be released before the underlying coverage review. */
#if 0
static void Clear(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    UmiCtestCoverageReviewDestroy(s->review);
    s->review = NULL;
    g_clear_pointer(&s->rendered, g_free);
    s->offset = 0;
    gtk_text_buffer_set_text(s->buffer, "", -1);
    gtk_label_set_text(GTK_LABEL(s->source), "No coverage loaded");
    gtk_label_set_text(GTK_LABEL(s->status), "Saved review closed.");
    s->busy = FALSE;
    g_object_unref(s->root);
}
#endif
static void Clear(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button))) return;
    g_object_ref(s->root);
    s->busy = TRUE;
    UmiCtestCoverageViewDestroy(s->view); s->view = NULL;
    UmiCtestCoverageReviewDestroy(s->review);
    s->review = NULL;
    g_clear_pointer(&s->rendered, g_free);
    s->offset = 0;
    gtk_text_buffer_set_text(s->buffer, "", -1);
    gtk_label_set_text(GTK_LABEL(s->source), "No coverage loaded");
    gtk_label_set_text(GTK_LABEL(s->status), "Saved review closed.");
    s->busy = FALSE;
    g_object_unref(s->root);
}
/* Cancellation applies to the pending read, before accepted-state publication.
 * Retain the previous busy-based boundary for review; the replacement completes
 * owned widget metadata even if a notification hides or retires the host. */
#if 0
static void Unmapped(GtkWidget *root, gpointer data)
{
    (void)root;
    CoveragePanel *s = data;
    if (s->busy) s->cancelled = TRUE;
}
#endif
static void Unmapped(GtkWidget *root, gpointer data)
{
    (void)root;
    CoveragePanel *s = data;
    if (s->loading) s->cancelled = TRUE;
}
gboolean UmiGtk4CtestCoveragePending(GtkWidget *panel)
{
    if (!GTK_IS_WIDGET(panel)) return FALSE;
    CoveragePanel *s = g_object_get_data(G_OBJECT(panel), "umicom-ctest-coverage");
    return s && s->busy;
}
static GtkWidget *Entry(CoveragePanel *s, const char *hint, const char *tag)
{
    GtkWidget *entry = gtk_entry_new();
    g_object_ref_sink(entry);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), hint);
    (void)umi_gtk4_automation_tag_widget(entry, tag);
    gtk_box_append(GTK_BOX(s->root), entry);
    return entry;
}
static GtkWidget *Label(CoveragePanel *s, const char *text, const char *tag)
{
    GtkWidget *label = gtk_label_new(text);
    g_object_ref_sink(label);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);
    (void)umi_gtk4_automation_tag_widget(label, tag);
    gtk_box_append(GTK_BOX(s->root), label);
    return label;
}
static void Button(CoveragePanel *s, GtkWidget *row, const char *title, const char *tag,
    GCallback callback, int direction)
{
    GtkWidget *button = gtk_button_new_with_label(title);
    (void)umi_gtk4_automation_tag_widget(button, tag);
    g_object_set_data(G_OBJECT(button), "coverage-direction", GINT_TO_POINTER(direction));
    g_signal_connect_object(button, "clicked", callback, s->root, 0);
    gtk_box_append(GTK_BOX(row), button);
}

/* Filtering is presentation only. Adopt a complete new index and text before
 * notifications, leaving the previous view intact if validation/allocation fails. */
static void ApplyFilter(GtkButton *button, gpointer data)
{
    CoveragePanel *s = g_object_get_data(G_OBJECT(data), "umicom-ctest-coverage");
    if (!Active(s, GTK_WIDGET(button)) || s->review == NULL) return;
    g_object_ref(s->root); s->busy = TRUE;
    UmiCtestCoverageFilter filter;
    UmiCtestCoverageFilterInit(&filter);
    guint outcome = gtk_drop_down_get_selected(GTK_DROP_DOWN(s->outcome));
    guint order = gtk_drop_down_get_selected(GTK_DROP_DOWN(s->order));
    const char *query = gtk_editable_get_text(GTK_EDITABLE(s->query));
    const char *host = gtk_editable_get_text(GTK_EDITABLE(s->host));
    const char *configuration = gtk_editable_get_text(GTK_EDITABLE(s->configuration));
    UmiStatus status = outcome > 10U || order > 3U || strlen(query) >= sizeof filter.text_contains ||
        strlen(host) >= sizeof filter.host_contains || strlen(configuration) >= sizeof filter.configuration_contains
        ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK;
    if (status == UMI_STATUS_OK) {
        filter.outcomes = outcome == 0U ? UMI_CTEST_COVERAGE_VIEW_ALL :
            outcome == 1U ? UMI_CTEST_COVERAGE_VIEW_OUTSTANDING : 1U << (outcome - 2U);
        filter.order = (UmiCtestCoverageOrder)order;
        strcpy(filter.text_contains, query); strcpy(filter.host_contains, host);
        strcpy(filter.configuration_contains, configuration);
    }
    UmiCtestCoverageView *view = NULL;
    char *text = NULL;
    if (status == UMI_STATUS_OK) status = UmiCtestCoverageViewCreate(s->review, &filter, &view);
    if (status == UMI_STATUS_OK) status = PrepareText(view, 0U, &text);
    if (status == UMI_STATUS_OK) {
        UmiCtestCoverageViewDestroy(s->view); s->view = view; view = NULL; s->filter = filter;
        PublishText(s, text, 0U); text = NULL;
        gtk_label_set_text(GTK_LABEL(s->status),
            "Filter applied to saved requirements. Whole-review outstanding counts remain unchanged.");
    } else {
        gtk_label_set_text(GTK_LABEL(s->status),
            "Filter refused. Use bounded printable text and a listed outcome/order. The accepted page is unchanged.");
    }
    g_free(text); UmiCtestCoverageViewDestroy(view);
    s->busy = FALSE; g_object_unref(s->root);
}
static GtkWidget *FilterChoice(CoveragePanel *s, const char *const *labels, const char *tag)
{
    GtkWidget *choice = gtk_drop_down_new_from_strings(labels);
    g_object_ref_sink(choice);
    (void)umi_gtk4_automation_tag_widget(choice, tag);
    gtk_box_append(GTK_BOX(s->root), choice);
    return choice;
}

GtkWidget *UmiGtk4CtestCoverageCreate(void)
{
    CoveragePanel *s = g_new0(CoveragePanel, 1);
    s->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    UmiCtestCoverageFilterInit(&s->filter);
    /* Preserve the existing all-requirements initial display. */
    s->filter.outcomes = UMI_CTEST_COVERAGE_VIEW_ALL;
    s->path = Entry(s, "Absolute host-coverage CSV manifest", "ctest.coverage.path");
    GtkWidget *picker = UmiPathPickerGtk4Create(GTK_ENTRY(s->path), UMI_PATH_PICKER_EXISTING_FILE,
        "Choose saved host-coverage plan", NULL);
    if (picker) gtk_box_append(GTK_BOX(s->root), picker);
    s->identity = Entry(s, "Expected source/build fingerprint", "ctest.coverage.identity");
    s->source = Label(s, "No coverage loaded", "ctest.coverage.source");
    s->query = Entry(s, "Filter test name or capability (literal text)", "ctest.coverage.filter.text");
    s->host = Entry(s, "Filter host", "ctest.coverage.filter.host");
    s->configuration = Entry(s, "Filter configuration", "ctest.coverage.filter.configuration");
    const char *outcomes[] = {"All requirements", "Outstanding requirements", "Passed", "Failed", "Timed out",
        "Skipped", "Disabled", "Not run", "Missing host", "Missing test", "Source mismatch", NULL};
    const char *orders[] = {"Requirement order", "Test name order", "Host/configuration order", "Outcome order", NULL};
    s->outcome = FilterChoice(s, outcomes, "ctest.coverage.filter.outcome");
    s->order = FilterChoice(s, orders, "ctest.coverage.filter.order");
    GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    Button(s, row, "Apply filter", "ctest.coverage.filter.apply", G_CALLBACK(ApplyFilter), 0);
    Button(s, row, "Open coverage", "ctest.coverage.open", G_CALLBACK(Open), 0);
    Button(s, row, "Cancel load", "ctest.coverage.cancel", G_CALLBACK(Cancel), 0);
    Button(s, row, "Previous", "ctest.coverage.previous", G_CALLBACK(Page), -1);
    Button(s, row, "Next", "ctest.coverage.next", G_CALLBACK(Page), 1);
    Button(s, row, "Copy page", "ctest.coverage.copy", G_CALLBACK(Copy), 0);
    Button(s, row, "Close review", "ctest.coverage.clear", G_CALLBACK(Clear), 0);
    GtkWidget *actions = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(actions), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(actions), row);
    gtk_box_append(GTK_BOX(s->root), actions);
    s->buffer = gtk_text_buffer_new(NULL);
    GtkWidget *view = gtk_text_view_new_with_buffer(s->buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
    (void)umi_gtk4_automation_tag_widget(view, "ctest.coverage.output");
    GtkWidget *scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 220);
    gtk_box_append(GTK_BOX(s->root), scroll);
    s->status = Label(s, "Read saved evidence. This does not run tests, verify hosts or change live results.",
        "ctest.coverage.status");
    (void)umi_gtk4_automation_tag_widget(s->root, "ctest.coverage.panel");
    g_object_set_data_full(G_OBJECT(s->root), "umicom-ctest-coverage", s, FreePanel);
    g_signal_connect(s->root, "unmap", G_CALLBACK(Unmapped), s);
    return s->root;
}
