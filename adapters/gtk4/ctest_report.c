/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/ctest_report.c
 *
 * PURPOSE:
 *   Own asynchronous saved-report admission and immutable page presentation on the GTK thread.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/ctest_report.h"
#include "umicom/platform/path.h"
#include "umicom/testing/ctest_report_file.h"
#include "umicom/testing/ctest_report_view.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/path_picker.h"
#include <string.h>
typedef struct SavedReportPanel {
  GtkWidget *root, *path, *name, *outcome, *order, *status, *source;
  GtkTextBuffer *buffer;
  UmiCtestReport *report;
  UmiCtestReportView *view;
  char *rendered;
  size_t offset;
  gboolean busy;
  gboolean cancelled;
} SavedReportPanel;
typedef struct SavedReportJob {
  GtkWidget *root;
  char path[UMI_PATH_CAPACITY];
  UmiStatus status;
  UmiCtestReport *report;
} SavedReportJob;
static void RetireView(SavedReportPanel *s) {
  UmiCtestReportViewDestroy(s->view);
  s->view = NULL;
  g_clear_pointer(&s->rendered, g_free);
  s->offset = 0U;
  gtk_text_buffer_set_text(s->buffer, "", -1);
}
static void FreePanel(gpointer data) {
  SavedReportPanel *s = data;
  UmiCtestReportViewDestroy(s->view);
  UmiCtestReportDestroy(s->report);
  g_free(s->rendered);
  g_object_unref(s->path);
  g_object_unref(s->name);
  g_object_unref(s->outcome);
  g_object_unref(s->order);
  g_object_unref(s->status);
  g_object_unref(s->source);
  g_object_unref(s->buffer);
  g_free(s);
}
static gboolean Active(SavedReportPanel *s, GtkWidget *button) {
  return !s->busy && gtk_widget_get_mapped(s->root) && gtk_widget_get_mapped(button) &&
         gtk_widget_is_ancestor(button, s->root);
}
static void Render(SavedReportPanel *s) {
  size_t bytes = 0U;
  char *text = NULL;
  UmiStatus status = UmiCtestReportViewFormat(s->view, s->offset, 64U, NULL, 0U, &bytes);
  if (status == UMI_STATUS_OK) {
    text = g_malloc(bytes + 1U);
    status = UmiCtestReportViewFormat(s->view, s->offset, 64U, text, bytes + 1U, &bytes);
  }
  g_free(s->rendered);
  s->rendered = status == UMI_STATUS_OK ? text : NULL;
  if (status != UMI_STATUS_OK)
    g_free(text);
  gtk_text_buffer_set_text(s->buffer, s->rendered ? s->rendered : "", -1);
  if (gtk_widget_get_mapped(s->root))
    gtk_label_set_text(GTK_LABEL(s->status), status == UMI_STATUS_OK
                                                 ? "Saved observations only. Apply filters "
                                                   "explicitly; Copy page copies the current page."
                                                 : "Unable to format this selection.");
}
static UmiStatus ApplyDraft(SavedReportPanel *s) {
  UmiCtestReportFilter filter;
  UmiCtestReportFilterInit(&filter);
  guint outcome = gtk_drop_down_get_selected(GTK_DROP_DOWN(s->outcome));
  guint order = gtk_drop_down_get_selected(GTK_DROP_DOWN(s->order));
  const unsigned masks[] = {62U, 63U, 1U, 2U, 4U, 8U, 16U, 32U};
  const char *name = gtk_editable_get_text(GTK_EDITABLE(s->name));
  size_t length = strlen(name);
  if (outcome >= G_N_ELEMENTS(masks) || order > 3U || length >= sizeof filter.name_contains)
    return UMI_STATUS_INVALID_ARGUMENT;
  filter.outcomes = masks[outcome];
  filter.order = (UmiCtestReportOrder)order;
  memcpy(filter.name_contains, name, length + 1U);
  return UmiCtestReportViewCreate(s->report, &filter, &s->view);
}
static void Apply(GtkButton *button, gpointer data) {
  GtkWidget *root = data;
  SavedReportPanel *s = g_object_get_data(G_OBJECT(root), "umicom-saved-report");
  if (!Active(s, GTK_WIDGET(button)) || s->report == NULL)
    return;
  g_object_ref(root);
  s->busy = TRUE;
  RetireView(s);
  UmiStatus status = gtk_widget_get_mapped(root) ? ApplyDraft(s) : UMI_STATUS_CANCELLED;
  if (status == UMI_STATUS_OK)
    Render(s);
  else if (gtk_widget_get_mapped(root))
    gtk_label_set_text(GTK_LABEL(s->status),
                       "Check the name filter and choices, then Apply again.");
  s->busy = FALSE;
  g_object_unref(root);
}
static void Page(GtkButton *button, gpointer data) {
  GtkWidget *root = data;
  SavedReportPanel *s = g_object_get_data(G_OBJECT(root), "umicom-saved-report");
  if (!Active(s, GTK_WIDGET(button)) || s->view == NULL)
    return;
  g_object_ref(root);
  s->busy = TRUE;
  UmiCtestReportViewSummary summary;
  if (UmiCtestReportViewRead(s->view, &summary) == UMI_STATUS_OK) {
    int direction = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "page-direction"));
    if (direction > 0 && s->offset < summary.matches && summary.matches - s->offset > 64U)
      s->offset += 64U;
    else if (direction < 0)
      s->offset = s->offset >= 64U ? s->offset - 64U : 0U;
    Render(s);
  }
  s->busy = FALSE;
  g_object_unref(root);
}
static void Clear(GtkButton *button, gpointer data) {
  GtkWidget *root = data;
  SavedReportPanel *s = g_object_get_data(G_OBJECT(root), "umicom-saved-report");
  if (!Active(s, GTK_WIDGET(button)))
    return;
  g_object_ref(root);
  s->busy = TRUE;
  RetireView(s);
  UmiCtestReportDestroy(s->report);
  s->report = NULL;
  if (gtk_widget_get_mapped(root))
    gtk_label_set_text(GTK_LABEL(s->source), "No report loaded");
  if (gtk_widget_get_mapped(root))
    gtk_label_set_text(GTK_LABEL(s->status), "Saved report closed; its source file is unchanged.");
  s->busy = FALSE;
  g_object_unref(root);
}
static void Copy(GtkButton *button, gpointer data) {
  GtkWidget *root = data;
  SavedReportPanel *s = g_object_get_data(G_OBJECT(root), "umicom-saved-report");
  if (!Active(s, GTK_WIDGET(button)) || s->rendered == NULL)
    return;
  g_object_ref(root);
  s->busy = TRUE;
  GdkClipboard *clipboard = gtk_widget_get_clipboard(GTK_WIDGET(button));
  if (clipboard != NULL)
    gdk_clipboard_set_text(clipboard, s->rendered);
  s->busy = FALSE;
  g_object_unref(root);
}
static void JobFree(gpointer data) {
  SavedReportJob *job = data;
  UmiCtestReportDestroy(job->report);
  g_object_unref(job->root);
  g_free(job);
}
static void ReadWorker(GTask *task, gpointer source, gpointer data, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  SavedReportJob *job = data;
  job->status = UmiCtestReportReadFile(job->path, &job->report);
  g_task_return_boolean(task, TRUE);
}
static void ReadComplete(GObject *source, GAsyncResult *result, gpointer data) {
  (void)source;
  (void)data;
  SavedReportJob *job = g_task_get_task_data(G_TASK(result));
  SavedReportPanel *s = g_object_get_data(G_OBJECT(job->root), "umicom-saved-report");
  /* The worker owns no GTK objects except a retained root reference. Only
   * this owner-thread completion transfers a successfully parsed report. */
  if (!s->cancelled && gtk_widget_get_mapped(job->root) && job->status == UMI_STATUS_OK) {
    s->report = job->report;
    job->report = NULL;
    gtk_label_set_text(GTK_LABEL(s->source), job->path);
    UmiStatus status =
        !s->cancelled && gtk_widget_get_mapped(job->root) ? ApplyDraft(s) : UMI_STATUS_CANCELLED;
    if (status == UMI_STATUS_OK)
      Render(s);
    else if (gtk_widget_get_mapped(job->root))
      gtk_label_set_text(GTK_LABEL(s->status), "Report read; correct the filter and Apply again.");
  } else if (!s->cancelled && gtk_widget_get_mapped(job->root)) {
    gtk_label_set_text(
        GTK_LABEL(s->status),
        "Unable to read a complete CTest JUnit XML report. Check the file, format and byte limit.");
  }
  if (s->cancelled && gtk_widget_get_mapped(job->root))
    gtk_label_set_text(
        GTK_LABEL(s->status),
        "Load cancelled; the local read has finished and its results were not displayed.");
  s->busy = FALSE;
}
static void Unmapped(GtkWidget *widget, gpointer data) {
  (void)widget;
  SavedReportPanel *s = data;
  /* Hiding then showing the same widget must not revive an older load. */
  if (s->busy)
    s->cancelled = TRUE;
}
static void Open(GtkButton *button, gpointer data) {
  GtkWidget *root = data;
  SavedReportPanel *s = g_object_get_data(G_OBJECT(root), "umicom-saved-report");
  if (!Active(s, GTK_WIDGET(button)))
    return;
  g_object_ref(root);
  s->busy = TRUE;
  s->cancelled = FALSE;
  SavedReportJob *job = g_new0(SavedReportJob, 1);
  job->root = g_object_ref(root);
  const char *path = gtk_editable_get_text(GTK_EDITABLE(s->path));
  size_t length = strlen(path);
  gboolean valid = length < sizeof job->path && umi_path_is_absolute(path);
  if (valid)
    memcpy(job->path, path, length + 1U);
  RetireView(s);
  UmiCtestReportDestroy(s->report);
  s->report = NULL;
  if (gtk_widget_get_mapped(root))
    gtk_label_set_text(GTK_LABEL(s->source), "No report loaded");
  if (valid && !s->cancelled && gtk_widget_get_mapped(root)) {
    gtk_label_set_text(GTK_LABEL(s->status), "Reading the selected saved report...");
    if (!s->cancelled && gtk_widget_get_mapped(root)) {
      GTask *task = g_task_new(NULL, NULL, ReadComplete, NULL);
      g_task_set_task_data(task, job, JobFree);
      g_task_run_in_thread(task, ReadWorker);
      g_object_unref(task);
      job = NULL;
    }
  } else if (gtk_widget_get_mapped(root))
    gtk_label_set_text(GTK_LABEL(s->status), "Choose an absolute local JUnit report path.");
  if (job != NULL) {
    s->busy = FALSE;
    JobFree(job);
  }
  g_object_unref(root);
}

static void Cancel(GtkButton *button, gpointer data) {
  GtkWidget *root = data;
  SavedReportPanel *s = g_object_get_data(G_OBJECT(root), "umicom-saved-report");
  if (!s->busy || !gtk_widget_get_mapped(root) || !gtk_widget_get_mapped(GTK_WIDGET(button)))
    return;
  g_object_ref(root);
  s->cancelled = TRUE;
  gtk_label_set_text(GTK_LABEL(s->status),
                     "Load cancelled. Waiting for the local read to release its resources.");
  g_object_unref(root);
}
gboolean UmiGtk4CtestReportBusy(GtkWidget *panel) {
  if (!GTK_IS_WIDGET(panel))
    return FALSE;
  SavedReportPanel *s = g_object_get_data(G_OBJECT(panel), "umicom-saved-report");
  return s != NULL && s->busy;
}

static GtkWidget *Entry(SavedReportPanel *s, const char *hint, const char *tag, int maximum) {
  GtkWidget *entry = gtk_entry_new();
  g_object_ref_sink(entry);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry), hint);
  gtk_entry_set_max_length(GTK_ENTRY(entry), maximum);
  (void)umi_gtk4_automation_tag_widget(entry, tag);
  gtk_box_append(GTK_BOX(s->root), entry);
  return entry;
}
static void Button(SavedReportPanel *s, GtkWidget *row, const char *title, const char *tag,
                   GCallback action, int direction) {
  GtkWidget *button = gtk_button_new_with_label(title);
  (void)umi_gtk4_automation_tag_widget(button, tag);
  g_object_set_data(G_OBJECT(button), "page-direction", GINT_TO_POINTER(direction));
  g_signal_connect_object(button, "clicked", action, s->root, 0);
  gtk_box_append(GTK_BOX(row), button);
}
GtkWidget *UmiGtk4CtestReportCreate(void) {
  SavedReportPanel *s = g_new0(SavedReportPanel, 1);
  s->root = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
  s->path =
      Entry(s, "Absolute CTest JUnit XML filename", "ctest.report.path", UMI_PATH_CAPACITY - 1);
  GtkWidget *browse = UmiPathPickerGtk4Create(GTK_ENTRY(s->path), UMI_PATH_PICKER_EXISTING_FILE,
                                              "Choose saved CTest report", NULL);
  if (browse != NULL)
    gtk_box_append(GTK_BOX(s->root), browse);
  s->source = gtk_label_new("No report loaded");
  g_object_ref_sink(s->source);
  gtk_label_set_selectable(GTK_LABEL(s->source), TRUE);
  gtk_label_set_wrap(GTK_LABEL(s->source), TRUE);
  (void)umi_gtk4_automation_tag_widget(s->source, "ctest.report.source");
  gtk_box_append(GTK_BOX(s->root), s->source);
  s->name = Entry(s, "Test name contains (ASCII case-insensitive)", "ctest.report.name", 255);
  const char *outcomes[] = {"Non-passed", "All outcomes", "Passed",  "Failed", "Timed out",
                            "Skipped",    "Disabled",     "Not run", NULL};
  const char *orders[] = {"Report order", "Name", "Longest first", "Outcome", NULL};
  s->outcome = gtk_drop_down_new_from_strings(outcomes);
  g_object_ref_sink(s->outcome);
  s->order = gtk_drop_down_new_from_strings(orders);
  g_object_ref_sink(s->order);
  (void)umi_gtk4_automation_tag_widget(s->outcome, "ctest.report.outcome");
  (void)umi_gtk4_automation_tag_widget(s->order, "ctest.report.order");
  gtk_box_append(GTK_BOX(s->root), s->outcome);
  gtk_box_append(GTK_BOX(s->root), s->order);
  GtkWidget *row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
  Button(s, row, "Open report", "ctest.report.open", G_CALLBACK(Open), 0);
  Button(s, row, "Cancel load", "ctest.report.cancel", G_CALLBACK(Cancel), 0);
  Button(s, row, "Apply filters", "ctest.report.apply", G_CALLBACK(Apply), 0);
  Button(s, row, "Previous", "ctest.report.previous", G_CALLBACK(Page), -1);
  Button(s, row, "Next", "ctest.report.next", G_CALLBACK(Page), 1);
  Button(s, row, "Copy page", "ctest.report.copy", G_CALLBACK(Copy), 0);
  Button(s, row, "Close report", "ctest.report.clear", G_CALLBACK(Clear), 0);
  gtk_box_append(GTK_BOX(s->root), row);
  s->buffer = gtk_text_buffer_new(NULL);
  GtkWidget *view = gtk_text_view_new_with_buffer(s->buffer);
  gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(view), TRUE);
  (void)umi_gtk4_automation_tag_widget(view, "ctest.report.output");
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), view);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scroll), 200);
  gtk_box_append(GTK_BOX(s->root), scroll);
  s->status =
      gtk_label_new("Read saved JUnit XML. This does not run tests or verify the current source.");
  g_object_ref_sink(s->status);
  (void)umi_gtk4_automation_tag_widget(s->status, "ctest.report.status");
  gtk_label_set_wrap(GTK_LABEL(s->status), TRUE);
  gtk_box_append(GTK_BOX(s->root), s->status);
  (void)umi_gtk4_automation_tag_widget(s->root, "ctest.report.panel");
  g_object_set_data_full(G_OBJECT(s->root), "umicom-saved-report", s, FreePanel);
  g_signal_connect(s->root, "unmap", G_CALLBACK(Unmapped), s);
  return s->root;
}
