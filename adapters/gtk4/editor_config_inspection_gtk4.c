/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/editor_config_inspection_gtk4.c
 *
 * PURPOSE:
 *   Present asynchronous read-only project configuration review with captured paths and owned
 * lifetime.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/document/editor_config_inspection.h"
#include "umicom/editor/editor_config_settings.h"
#include "umicom/ui/gtk4/editor_config_inspection.h"
#include "window_removal_private.h"
#include <string.h>
typedef struct InspectJob {
  char *root, *relative;
  UmiCancellationToken *cancel;
  UmiEditorConfigInspection *inspection;
  UmiEditorConfigInspectionDiagnostic diagnostic;
  UmiStatus status;
} InspectJob;
typedef struct InspectWindow {
  GtkWindow *window;
  GtkWidget *body, *root, *relative, *message, *report;
  GtkTextBuffer *buffer;
  InspectJob *job;
  UmiStatus status;
  int busy, closed;
} InspectWindow;
static void FreeJob(gpointer data) {
  InspectJob *job = data;
  UmiEditorConfigInspectionDestroy(job->inspection);
  umi_cancellation_token_destroy(job->cancel);
  g_free(job->root);
  g_free(job->relative);
  g_free(job);
}
static void FreeWindow(gpointer data) {
  InspectWindow *s = data;
  g_object_unref(s->body);
  g_object_unref(s->buffer);
  g_free(s);
}
static void Removed(GtkWidget *window, gpointer data) {
  (void)window;
  InspectWindow *s = data;
  s->closed = 1;
  if (s->job)
    umi_cancellation_token_request(s->job->cancel);
}
static void Work(GTask *task, gpointer source, gpointer taskData, GCancellable *cancel) {
  (void)source;
  (void)cancel;
  InspectJob *job = taskData;
  job->status = UmiEditorConfigInspectionLoad(job->root, job->relative, job->cancel,
                                              &job->inspection, &job->diagnostic);
  g_task_return_boolean(task, TRUE);
}
static char *Render(const InspectJob *job) {
  GString *text = g_string_new("Project editor configuration review\n\n");
  g_string_append_printf(text, "Project boundary: %s\nTarget relative file: %s\n\n", job->root,
                         job->relative);
  if (job->status != UMI_STATUS_OK) {
    g_string_append_printf(text, "Inspection refused: %s\n", umi_status_text(job->status));
    if (job->diagnostic.relative_path[0])
      g_string_append_printf(text, "Configuration: %s\nLine: %zu\n", job->diagnostic.relative_path,
                             job->diagnostic.line);
  } else {
    size_t files = UmiEditorConfigInspectionFileCount(job->inspection);
    g_string_append_printf(text, "Observed configuration files: %zu\n", files);
    for (size_t i = 0; i < files; ++i) {
      UmiEditorConfigInspectionFile file;
      if (UmiEditorConfigInspectionFileAt(job->inspection, i, &file) == UMI_STATUS_OK)
        g_string_append_printf(text, "%s (%zu bytes%s)\nSHA-256 %s\n", file.relative_path,
                               file.bytes, file.root ? ", root=true" : "", file.sha256);
    }
    const UmiEditorConfigResult *r = UmiEditorConfigInspectionResult(job->inspection);
    g_string_append(text, "\nResolved properties (last matching value wins):\n");
    for (size_t i = 0; i < UmiEditorConfigResultCount(r); ++i) {
      UmiEditorConfigProperty p;
      if (UmiEditorConfigResultAt(r, i, &p) == UMI_STATUS_OK)
        g_string_append_printf(text, "%s = %s\n  %s:%zu [%s]\n", p.key, p.value, p.source_name,
                               p.line, p.pattern);
    }
    if (!UmiEditorConfigResultCount(r))
      g_string_append(text, "No matching properties within this project boundary.\n");
    UmiEditorConfigSettings settings;
    if (UmiEditorConfigSettingsRead(r, &settings) == UMI_STATUS_OK)
      g_string_append_printf(text,
                             "\nUnrecognized properties: %zu. Invalid recognized values: %zu.\n",
                             settings.unknown_properties, settings.invalid_values);
  }
  g_string_append(text,
                  "\nThis is a read-only observation. No preferences or source files changed.\n"
                  "Files may change after they are read. Inspect again for a newer observation.\n"
                  "Ancestors above the chosen project boundary are not searched.\n");
  char *valid = g_utf8_make_valid(text->str, (gssize)text->len);
  g_string_free(text, TRUE);
  return valid;
}
static void Complete(GObject *object, GAsyncResult *result, gpointer user) {
  (void)user;
  InspectWindow *s = g_object_get_data(object, "umicom-editor-config-inspection");
  InspectJob *job = g_task_get_task_data(G_TASK(result));
  (void)g_task_propagate_boolean(G_TASK(result), NULL);
  /* The task retains its window and its immutable request. Keep busy through
   * all GTK notifications; observers cannot dispatch another read halfway. */
  if (job->cancel && umi_cancellation_token_is_requested(job->cancel))
    job->status = UMI_STATUS_CANCELLED;
  if (!s->closed) {
    s->status = job->status;
    if (strcmp(gtk_editable_get_text(GTK_EDITABLE(s->root)), job->root) ||
        strcmp(gtk_editable_get_text(GTK_EDITABLE(s->relative)), job->relative)) {
      s->status = UMI_STATUS_INVALID_STATE;
      gtk_label_set_text(GTK_LABEL(s->message),
                         "Inputs changed during inspection. Inspect again to review those paths.");
    } else {
      char *text = Render(job);
      gtk_text_buffer_set_text(s->buffer, text, -1);
      g_free(text);
      if (!s->closed)
        gtk_label_set_text(
            GTK_LABEL(s->message),
            job->status == UMI_STATUS_OK
                ? "Inspection complete. Values below belong to the captured paths; nothing was "
                  "applied."
                : "Inspection did not complete. Review the diagnostic below and retry.");
    }
  }
  s->job = NULL;
  s->busy = 0;
}
static void Start(GtkButton *button, gpointer data) {
  GtkWindow *window = data;
  g_object_ref(window);
  InspectWindow *s = g_object_get_data(G_OBJECT(window), "umicom-editor-config-inspection");
  if (!s || s->closed || s->busy || gtk_widget_get_root(GTK_WIDGET(button)) != GTK_ROOT(window) ||
      !gtk_window_get_application(window)) {
    g_object_unref(window);
    return;
  }
  InspectJob *job = g_new0(InspectJob, 1);
  job->root = g_strdup(gtk_editable_get_text(GTK_EDITABLE(s->root)));
  job->relative = g_strdup(gtk_editable_get_text(GTK_EDITABLE(s->relative)));
  job->status = umi_cancellation_token_create(&job->cancel);
  if (job->status != UMI_STATUS_OK) {
    s->status = job->status;
    FreeJob(job);
    g_object_unref(window);
    return;
  }
  s->job = job;
  s->busy = 1;
  s->status = UMI_STATUS_BUSY;
  gtk_text_buffer_set_text(s->buffer, "", 0);
  if (!s->closed)
    gtk_label_set_text(GTK_LABEL(s->message), "Inspecting captured project paths…");
  if (s->closed) {
    s->job = NULL;
    s->busy = 0;
    FreeJob(job);
    g_object_unref(window);
    return;
  }
  GTask *task = g_task_new(window, NULL, Complete, NULL);
  g_task_set_task_data(task, job, FreeJob);
  g_task_run_in_thread(task, Work);
  g_object_unref(task);
  g_object_unref(window);
}
static void Cancel(GtkButton *button, gpointer data) {
  GtkWindow *window = data;
  InspectWindow *s = g_object_get_data(G_OBJECT(window), "umicom-editor-config-inspection");
  if (s && !s->closed && s->job && gtk_widget_get_root(GTK_WIDGET(button)) == GTK_ROOT(window))
    umi_cancellation_token_request(s->job->cancel);
}
static void Close(GtkButton *button, gpointer data) {
  (void)button;
  gtk_window_destroy(GTK_WINDOW(data));
}
static GtkWidget *Field(GtkWidget *body, const char *label, const char *name, const char *value) {
  GtkWidget *caption = gtk_label_new(label);
  gtk_label_set_xalign(GTK_LABEL(caption), 0.0F);
  gtk_box_append(GTK_BOX(body), caption);
  GtkWidget *entry = gtk_entry_new();
  gtk_widget_set_name(entry, name);
  gtk_entry_set_max_length(GTK_ENTRY(entry), (int)UMI_EDITOR_CONFIG_INSPECTION_PATH - 1);
  gtk_editable_set_text(GTK_EDITABLE(entry), value ? value : "");
  gtk_box_append(GTK_BOX(body), entry);
  return entry;
}
GtkWindow *UmiGtk4EditorConfigInspectionCreateWindow(GtkApplication *application, const char *root,
                                                     const char *relative) {
  if (!GTK_IS_APPLICATION(application) || (root && !g_utf8_validate(root, -1, NULL)) ||
      (relative && !g_utf8_validate(relative, -1, NULL)))
    return NULL;
  if ((root && strlen(root) >= UMI_EDITOR_CONFIG_INSPECTION_PATH) ||
      (relative && strlen(relative) >= UMI_EDITOR_CONFIG_INSPECTION_PATH))
    return NULL;
  InspectWindow *s = g_new0(InspectWindow, 1);
  s->busy = 1;
  s->status = UMI_STATUS_UNAVAILABLE;
  s->window = GTK_WINDOW(gtk_window_new());
  g_object_ref(s->window);
  s->body = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  g_object_ref_sink(s->body);
  s->buffer = gtk_text_buffer_new(NULL);
  g_object_set_data_full(G_OBJECT(s->window), "umicom-editor-config-inspection", s, FreeWindow);
  UmiGtk4ObserveWindowRemoval(s->window, G_OBJECT(s->window), Removed, s);
  gtk_widget_set_name(GTK_WIDGET(s->window), "umicom-editor-config-inspection");
  gtk_window_set_title(s->window, "Project Editor Configuration");
  gtk_window_set_default_size(s->window, 820, 640);
  gtk_widget_set_margin_start(s->body, 12);
  gtk_widget_set_margin_end(s->body, 12);
  gtk_widget_set_margin_top(s->body, 12);
  gtk_widget_set_margin_bottom(s->body, 12);
  s->root = Field(s->body, "Absolute project boundary", "editor-config.root", root);
  s->relative =
      Field(s->body, "File relative to that boundary", "editor-config.relative", relative);
  GtkWidget *actions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
  GtkWidget *inspect = gtk_button_new_with_label("Inspect"),
            *cancel = gtk_button_new_with_label("Cancel inspection"),
            *close = gtk_button_new_with_label("Close");
  gtk_widget_set_name(inspect, "editor-config.inspect");
  gtk_widget_set_name(cancel, "editor-config.cancel");
  g_signal_connect_object(inspect, "clicked", G_CALLBACK(Start), s->window, 0);
  g_signal_connect_object(cancel, "clicked", G_CALLBACK(Cancel), s->window, 0);
  g_signal_connect_object(close, "clicked", G_CALLBACK(Close), s->window, 0);
  gtk_box_append(GTK_BOX(actions), inspect);
  gtk_box_append(GTK_BOX(actions), cancel);
  gtk_box_append(GTK_BOX(actions), close);
  gtk_box_append(GTK_BOX(s->body), actions);
  s->message = gtk_label_new(
      "Choose a project boundary and relative file, then Inspect. No settings are applied.");
  gtk_widget_set_name(s->message, "editor-config.message");
  gtk_label_set_wrap(GTK_LABEL(s->message), TRUE);
  gtk_box_append(GTK_BOX(s->body), s->message);
  s->report = gtk_text_view_new_with_buffer(s->buffer);
  gtk_widget_set_name(s->report, "editor-config.report");
  gtk_text_view_set_editable(GTK_TEXT_VIEW(s->report), FALSE);
  gtk_text_view_set_monospace(GTK_TEXT_VIEW(s->report), TRUE);
  GtkWidget *scroll = gtk_scrolled_window_new();
  gtk_widget_set_vexpand(scroll, TRUE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll), s->report);
  gtk_box_append(GTK_BOX(s->body), scroll);
  if (!s->closed)
    gtk_window_set_child(s->window, s->body);
  if (!s->closed)
    gtk_window_set_application(s->window, application);
  GtkWindow *window = s->window;
  if (s->closed || gtk_window_get_application(window) != application) {
    gtk_window_destroy(window);
    g_object_unref(window);
    return NULL;
  }
  s->busy = 0;
  g_object_unref(window);
  return window;
}
int UmiGtk4EditorConfigInspectionBusy(GtkWindow *window) {
  if (!GTK_IS_WINDOW(window))
    return 0;
  InspectWindow *s = g_object_get_data(G_OBJECT(window), "umicom-editor-config-inspection");
  return s && s->busy;
}
UmiStatus UmiGtk4EditorConfigInspectionStatus(GtkWindow *window) {
  if (!GTK_IS_WINDOW(window))
    return UMI_STATUS_INVALID_ARGUMENT;
  InspectWindow *s = g_object_get_data(G_OBJECT(window), "umicom-editor-config-inspection");
  if (!s)
    return UMI_STATUS_NOT_FOUND;
  if (s->closed)
    return UMI_STATUS_CANCELLED;
  return s->busy ? UMI_STATUS_BUSY : s->status;
}
