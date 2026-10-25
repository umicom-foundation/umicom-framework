/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/test_editor_config_inspection_gtk4.c
 *
 * PURPOSE:
 *   Verify native asynchronous review, cancellation, input changes and window retirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../build_log/fixture.h"
#include "umicom/platform/local_replace.h"
#include "umicom/ui/gtk4/editor_config_inspection.h"
static GtkWidget *Find(GtkWidget *widget, const char *name) {
  if (!strcmp(gtk_widget_get_name(widget), name))
    return widget;
  for (GtkWidget *child = gtk_widget_get_first_child(widget); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *found = Find(child, name);
    if (found)
      return found;
  }
  return NULL;
}
static char *Text(GtkWidget *widget) {
  GtkTextBuffer *b = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
  GtkTextIter a, z;
  gtk_text_buffer_get_bounds(b, &a, &z);
  return gtk_text_buffer_get_text(b, &a, &z, FALSE);
}
static void Drain(GtkWindow *window) {
  gint64 deadline = g_get_monotonic_time() + 15 * G_TIME_SPAN_SECOND;
  while (UmiGtk4EditorConfigInspectionBusy(window) && g_get_monotonic_time() < deadline) {
    while (g_main_context_pending(NULL))
      g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  CHECK(!UmiGtk4EditorConfigInspectionBusy(window));
}
static void RemoveOnReport(GtkTextBuffer *buffer, gpointer data) {
  if (gtk_text_buffer_get_char_count(buffer) > 0)
    gtk_window_destroy(GTK_WINDOW(data));
}
int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *name = argv[1],
             *names[] = {"inspect",       "empty",  "malformed", "cancel",       "retired",
                         "changed-input", "repeat", "readonly",  "close-report", "arguments"};
  int known = 0;
  for (size_t i = 0; i < sizeof names / sizeof names[0]; ++i)
    if (!strcmp(name, names[i]))
      known = 1;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  char root[UMI_PATH_CAPACITY], path[UMI_PATH_CAPACITY];
  FixtureDirectory(root);
  FixturePath(path, root, ".editorconfig");
  const char *config =
      !strcmp(name, "malformed") ? "[*]\nbad\n" : "root=true\n[*]\nindent_size=3\n";
  if (strcmp(name, "empty"))
    CHECK(UmiLocalFileReplace(path, config, strlen(config)) == UMI_STATUS_OK);
  GtkApplication *app =
      gtk_application_new("org.umicom.EditorConfigInspectionTest", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(app), NULL, NULL));
  if (!strcmp(name, "arguments")) {
    CHECK(UmiGtk4EditorConfigInspectionCreateWindow(NULL, root, "a.c") == NULL);
    CHECK(UmiGtk4EditorConfigInspectionCreateWindow(app, "\xc0\xaf", "a.c") == NULL);
    CHECK(UmiGtk4EditorConfigInspectionStatus(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    g_object_unref(app);
    return 0;
  }
  GtkWindow *window = UmiGtk4EditorConfigInspectionCreateWindow(app, root, "a.c");
  CHECK(window);
  g_object_ref(window);
  CHECK(UmiGtk4EditorConfigInspectionStatus(window) == UMI_STATUS_UNAVAILABLE);
  GtkWidget *inspect = Find(GTK_WIDGET(window), "editor-config.inspect"),
            *report = Find(GTK_WIDGET(window), "editor-config.report");
  CHECK(inspect && report && !gtk_text_view_get_editable(GTK_TEXT_VIEW(report)));
  if (!strcmp(name, "close-report"))
    g_signal_connect(gtk_text_view_get_buffer(GTK_TEXT_VIEW(report)), "changed",
                     G_CALLBACK(RemoveOnReport), window);
  g_signal_emit_by_name(inspect, "clicked");
  CHECK(UmiGtk4EditorConfigInspectionBusy(window));
  if (!strcmp(name, "cancel"))
    g_signal_emit_by_name(Find(GTK_WIDGET(window), "editor-config.cancel"), "clicked");
  if (!strcmp(name, "retired"))
    gtk_window_destroy(window);
  if (!strcmp(name, "changed-input"))
    gtk_editable_set_text(GTK_EDITABLE(Find(GTK_WIDGET(window), "editor-config.relative")),
                          "other.c");
  Drain(window);
  UmiStatus status = UmiGtk4EditorConfigInspectionStatus(window);
  if (!strcmp(name, "retired") || !strcmp(name, "close-report") || !strcmp(name, "cancel"))
    CHECK(status == UMI_STATUS_CANCELLED);
  else if (!strcmp(name, "changed-input"))
    CHECK(status == UMI_STATUS_INVALID_STATE);
  else if (!strcmp(name, "malformed"))
    CHECK(status == UMI_STATUS_PARSE_ERROR);
  else {
    CHECK(status == UMI_STATUS_OK);
    char *text = Text(report);
    CHECK(strstr(text, "Target relative file: a.c"));
    CHECK(strstr(text, !strcmp(name, "empty") ? "No matching properties" : "indent_size = 3"));
    g_free(text);
    if (!strcmp(name, "repeat")) {
      CHECK(UmiLocalFileReplace(path, "[*]\nindent_size=2\n", strlen("[*]\nindent_size=2\n")) ==
            UMI_STATUS_OK);
      g_signal_emit_by_name(inspect, "clicked");
      Drain(window);
      text = Text(report);
      CHECK(strstr(text, "indent_size = 2"));
      g_free(text);
    }
  }
  if (!strcmp(name, "readonly")) {
    size_t count;
    unsigned char *bytes = FixtureRead(path, &count);
    CHECK(count == strlen(config) && !memcmp(bytes, config, count));
    free(bytes);
  }
  gtk_window_destroy(window);
  g_object_unref(window);
  g_object_unref(app);
  return 0;
}
