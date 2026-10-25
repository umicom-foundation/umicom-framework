/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_backtrace/test_gtk4.c
 *
 * PURPOSE:
 *   Exercise copied review state and native window or path-policy changes during callbacks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/debug_backtrace.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
typedef struct NativeObserver {
  GtkWindow *window;
  GtkWidget *paths;
  unsigned fired;
  bool close;
} NativeObserver;
static void BufferChanged(GtkTextBuffer *buffer, gpointer data) {
  (void)buffer;
  NativeObserver *observer = data;
  if (observer->fired++)
    return;
  if (observer->close)
    gtk_window_destroy(observer->window);
  else
    gtk_drop_down_set_selected(GTK_DROP_DOWN(observer->paths), UMI_DEBUG_BACKTRACE_OMIT_PATHS);
}
static void Added(GtkApplication *app, GtkWindow *window, gpointer data) {
  (void)app;
  (void)data;
  if (!strcmp(gtk_widget_get_name(GTK_WIDGET(window)), "debug.backtrace.review"))
    gtk_window_destroy(window);
}
static char *Text(GtkWidget *view) {
  GtkTextIter first, last;
  GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
  gtk_text_buffer_get_bounds(buffer, &first, &last);
  return gtk_text_buffer_get_text(buffer, &first, &last, FALSE);
}
static GtkWidget *Find(GtkWindow *window, const char *name) {
  return umi_gtk4_automation_find_tagged_widget(GTK_WIDGET(window), name);
}
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1],
             *names[] = {"normal",        "cloned",      "paths",         "hidden-copy",
                         "retained-copy", "owner-close", "close-refresh", "policy-change",
                         "added-close",   "invalid"};
  CHECK(TraceKnown(name, names, sizeof names / sizeof names[0]));
  if (!gtk_init_check())
    return 77;
  TraceFixture f;
  TraceFixtureCreate(&f);
  UmiDebugBacktrace *trace = NULL;
  OK(UmiDebugBacktraceCapture(f.workspace, &trace));
  GtkApplication *app = gtk_application_new("org.umicom.BacktraceTest", G_APPLICATION_NON_UNIQUE);
  CHECK(g_application_register(G_APPLICATION(app), NULL, NULL));
  GtkWindow *owner = GTK_WINDOW(gtk_application_window_new(app));
  g_object_ref(owner);
  GtkWindow *window = NULL;
  gulong added = 0U;
  if (!strcmp(name, "added-close"))
    added = g_signal_connect(app, "window-added", G_CALLBACK(Added), NULL);
  UmiStatus status = UmiGtk4DebugBacktraceCreateWindow(owner, trace, &window);
  if (added)
    g_signal_handler_disconnect(app, added);
  if (!strcmp(name, "added-close")) {
    CHECK(status == UMI_STATUS_CANCELLED && window == NULL);
    goto done;
  }
  OK(status);
  CHECK(window != NULL);
  g_object_ref(window);
  GtkWidget *paths = Find(window, "debug.backtrace.paths"),
            *view = Find(window, "debug.backtrace.text"),
            *copy = Find(window, "debug.backtrace.copy"),
            *label = Find(window, "debug.backtrace.status");
  CHECK(GTK_IS_DROP_DOWN(paths) && GTK_IS_TEXT_VIEW(view) && GTK_IS_BUTTON(copy) &&
        GTK_IS_LABEL(label));
  CHECK(!gtk_text_view_get_editable(GTK_TEXT_VIEW(view)));
  char *text = Text(view);
  CHECK(strstr(text, "main.c") == NULL && strstr(text, "#0 main") != NULL);
  g_free(text);
  if (!strcmp(name, "cloned")) {
    UmiDebugBacktraceDestroy(trace);
    trace = NULL;
    TraceFixtureDestroy(&f);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(paths), UMI_DEBUG_BACKTRACE_FULL_PATHS);
    text = Text(view);
    CHECK(strstr(text, "/private/project/main.c") != NULL);
    g_free(text);
  } else if (!strcmp(name, "paths")) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(paths), UMI_DEBUG_BACKTRACE_FILENAMES);
    text = Text(view);
    CHECK(strstr(text, "main.c:42:3") != NULL && strstr(text, "/private") == NULL);
    g_free(text);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(paths), UMI_DEBUG_BACKTRACE_FULL_PATHS);
    text = Text(view);
    CHECK(strstr(text, "/private/project/main.c") != NULL);
    g_free(text);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(paths), UMI_DEBUG_BACKTRACE_OMIT_PATHS);
    text = Text(view);
    CHECK(strstr(text, "main.c") == NULL);
    g_free(text);
  } else if (!strcmp(name, "hidden-copy") || !strcmp(name, "retained-copy")) {
    g_object_ref(copy);
    g_object_ref(label);
    char *before = g_strdup(gtk_label_get_text(GTK_LABEL(label)));
    if (!strcmp(name, "retained-copy"))
      gtk_window_destroy(window);
    g_signal_emit_by_name(copy, "clicked");
    CHECK(!strcmp(gtk_label_get_text(GTK_LABEL(label)), before));
    g_free(before);
    g_object_unref(label);
    g_object_unref(copy);
  } else if (!strcmp(name, "owner-close")) {
    gtk_window_destroy(owner);
    CHECK(!UmiGtk4WindowIsOpen(window));
  } else if (!strcmp(name, "close-refresh") || !strcmp(name, "policy-change")) {
    GtkTextBuffer *buffer = g_object_ref(gtk_text_view_get_buffer(GTK_TEXT_VIEW(view)));
    NativeObserver observer = {window, paths, 0U, !strcmp(name, "close-refresh")};
    gulong handler = g_signal_connect(buffer, "changed", G_CALLBACK(BufferChanged), &observer);
    gtk_drop_down_set_selected(GTK_DROP_DOWN(paths), UMI_DEBUG_BACKTRACE_FULL_PATHS);
    g_signal_handler_disconnect(buffer, handler);
    CHECK(observer.fired > 0U);
    if (observer.close)
      CHECK(!UmiGtk4WindowIsOpen(window));
    else {
      text = Text(view);
      CHECK(text[0] == '\0');
      g_free(text);
    }
    g_object_unref(buffer);
  } else if (!strcmp(name, "invalid")) {
    GtkWindow *rejected = window;
    CHECK(UmiGtk4DebugBacktraceCreateWindow(owner, NULL, &rejected) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          rejected == NULL);
    CHECK(UmiGtk4DebugBacktraceCreateWindow(owner, trace, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    gtk_window_destroy(owner);
    CHECK(UmiGtk4DebugBacktraceCreateWindow(owner, trace, &rejected) == UMI_STATUS_CANCELLED &&
          rejected == NULL);
  }
  gtk_window_destroy(window);
  g_object_unref(window);
done:
  gtk_window_destroy(owner);
  g_object_unref(owner);
  g_object_unref(app);
  UmiDebugBacktraceDestroy(trace);
  TraceFixtureDestroy(&f);
  return 0;
}
