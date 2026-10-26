/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/local_file_drop/test_local_file_drop.c
 *
 * PURPOSE:
 *   Exercise copied drop paths, rejected selections and native receiver ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/local_file_drop.h"
#include "umicom/ui/gtk4/window_lifecycle.h"
#include <stdio.h>
#include <string.h>
#include "../support/main_context.h"

typedef struct Probe {
  GtkWindow *window;
  GtkWidget *widget;
  GtkDropTarget *target;
  GValue *value;
  const char *scenario;
  char *expected;
  unsigned calls, released;
  gboolean valid;
} Probe;

static void Released(gpointer data) { ++((Probe *)data)->released; }

static UmiStatus Received(GtkWidget *widget, const char *const *paths, size_t count,
                          gpointer data) {
  Probe *p = data;
  ++p->calls;
  p->valid = p->valid && widget == p->widget && count == 2U && strcmp(paths[0], p->expected) == 0 &&
             p->released == 0U;
  if (strcmp(p->scenario, "duplicate") == 0)
    p->valid = p->valid && strcmp(paths[0], paths[1]) == 0;
  if (strcmp(p->scenario, "reentry") == 0) {
    gboolean nested = TRUE;
    g_signal_emit_by_name(p->target, "drop", p->value, 0.0, 0.0, &nested);
    p->valid = p->valid && !nested && p->calls == 1U;
  } else if (strcmp(p->scenario, "remove") == 0)
    gtk_widget_remove_controller(widget, GTK_EVENT_CONTROLLER(p->target));
  else if (strcmp(p->scenario, "close") == 0)
    gtk_window_destroy(p->window);
  else if (strcmp(p->scenario, "clear-value") == 0)
    g_value_unset(p->value);
  /* Receiver paths must survive both native host changes and releasing the
   * transfer value. The application may copy them into a queued operation. */
  p->valid = p->valid && strcmp(paths[0], p->expected) == 0 && p->released == 0U;
  return strcmp(p->scenario, "receiver-error") == 0 ? UMI_STATUS_PERMISSION_DENIED : UMI_STATUS_OK;
}

/* Retain the iteration-only pump for review. Dispatch slices now observe the
 * same mapping deadline; one blocked native callback still needs the outer
 * CTest timeout and stage evidence below. This does not claim to repair it. */
#if 0
static void Pump(void) {
  for (unsigned i = 0U; i < 128U && g_main_context_pending(NULL); ++i)
    (void)g_main_context_iteration(NULL, FALSE);
}
#endif
static void Pump(gint64 deadline) {
  (void)UmiTestDispatchSlice(NULL, deadline, 32U);
}

static gint64 started;
static const char *scenario;
static void Stage(const char *phase) {
  fprintf(stderr, "local-file-drop scenario=%s phase=%s elapsed-us=%" G_GINT64_FORMAT "\n",
          scenario, phase, g_get_monotonic_time() - started);
  fflush(stderr);
}

#define CHECK(c)                                                                                   \
  do {                                                                                             \
    if (!(c)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c);                                      \
      failed = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)

int main(int argc, char **argv) {
  if (argc != 2)
    return 2;
  const char *cases[] = {"local",          "duplicate",  "limit",     "remote",     "mixed-remote",
                         "long",           "wrong-type", "null-list", "unparented", "hidden",
                         "insensitive",    "reentry",    "remove",    "close",      "clear-value",
                         "receiver-error", "constructor"};
  gboolean known = FALSE;
  for (size_t i = 0U; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = TRUE;
  if (!known)
    return 2;
  scenario = argv[1];
  started = g_get_monotonic_time();
  Stage("gtk-init-begin");
  if (!gtk_init_check())
    return 77;
  Stage("gtk-init-complete");
  int failed = 0;
  Probe p = {.scenario = argv[1], .valid = TRUE};
  GValue value = G_VALUE_INIT;
  GFile *files[2] = {NULL, NULL};
  char *first = g_build_filename(g_get_tmp_dir(), "umicom-drop-one.c", NULL);
  char *second = g_build_filename(g_get_tmp_dir(), "umicom-drop-two.c", NULL);
  p.value = &value;
  Stage("window-create-begin");
  p.window = GTK_WINDOW(g_object_ref_sink(gtk_window_new()));
  p.widget = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_window_set_child(p.window, p.widget);
  g_object_ref(p.widget);
  Stage("controller-create-begin");
  CHECK(UmiLocalFileDropGtk4Create(0U, Received, &p, Released) == NULL);
  CHECK(UmiLocalFileDropGtk4Create(UMI_LOCAL_FILE_DROP_MAXIMUM + 1U, Received, &p, Released) ==
        NULL);
  CHECK(UmiLocalFileDropGtk4Create(2U, NULL, &p, Released) == NULL);
  CHECK(p.released == 0U);
  p.target =
      UmiLocalFileDropGtk4Create(strcmp(argv[1], "limit") == 0 ? 1U : 2U, Received, &p, Released);
  CHECK(p.target != NULL);
  if (strcmp(argv[1], "unparented") != 0) {
    g_object_ref(p.target);
    gtk_widget_add_controller(p.widget, GTK_EVENT_CONTROLLER(p.target));
  }
  Stage("window-present-begin");
  gtk_window_present(p.window);
  Stage("window-present-returned");
  gint64 deadline = g_get_monotonic_time() + 3 * G_TIME_SPAN_SECOND;
  while (!gtk_widget_get_mapped(p.widget) && g_get_monotonic_time() < deadline) {
    Pump(deadline);
    g_usleep(1000U);
  }
  Stage("mapping-wait-complete");
  CHECK(gtk_widget_get_mapped(p.widget));
  if (strcmp(argv[1], "constructor") == 0)
    goto cleanup;
  if (strcmp(argv[1], "hidden") == 0)
    gtk_widget_set_visible(p.widget, FALSE);
  if (strcmp(argv[1], "insensitive") == 0)
    gtk_widget_set_sensitive(p.widget, FALSE);
  if (strcmp(argv[1], "long") == 0) {
    char *leaf = g_strnfill(UMI_LOCAL_FILE_DROP_PATH_CAPACITY + 1U, 'x');
    g_free(first);
    first = g_build_filename(g_get_tmp_dir(), leaf, NULL);
    g_free(leaf);
  }
  files[0] = g_file_new_for_path(first);
  files[1] = g_file_new_for_path(strcmp(argv[1], "duplicate") == 0 ? first : second);
  p.expected = g_file_get_path(files[0]);
  if (strcmp(argv[1], "remote") == 0 || strcmp(argv[1], "mixed-remote") == 0) {
    const size_t remote = strcmp(argv[1], "remote") == 0 ? 0U : 1U;
    g_clear_object(&files[remote]);
    files[remote] = g_file_new_for_uri("https://example.invalid/source.c");
  }
  if (strcmp(argv[1], "wrong-type") == 0) {
    g_value_init(&value, G_TYPE_STRING);
    g_value_set_string(&value, first);
  } else {
    g_value_init(&value, GDK_TYPE_FILE_LIST);
    if (strcmp(argv[1], "null-list") != 0)
      g_value_take_boxed(&value, gdk_file_list_new_from_array(files, 2U));
  }
  Stage("drop-value-ready");
  gboolean accepted = FALSE;
  Stage("drop-dispatch-begin");
  g_signal_emit_by_name(p.target, "drop", &value, 0.0, 0.0, &accepted);
  Stage("drop-dispatch-returned");
  const gboolean called = strcmp(argv[1], "local") == 0 || strcmp(argv[1], "duplicate") == 0 ||
                          strcmp(argv[1], "reentry") == 0 || strcmp(argv[1], "remove") == 0 ||
                          strcmp(argv[1], "close") == 0 || strcmp(argv[1], "clear-value") == 0 ||
                          strcmp(argv[1], "receiver-error") == 0;
  CHECK(p.valid && p.calls == (called ? 1U : 0U));
  CHECK(accepted == (called && strcmp(argv[1], "receiver-error") != 0));
cleanup:
  Stage("value-release-begin");
  if (G_VALUE_TYPE(&value) != G_TYPE_INVALID)
    g_value_unset(&value);
  Stage("window-destroy-begin");
  if (p.window != NULL && UmiGtk4WindowIsOpen(p.window))
    gtk_window_destroy(p.window);
  Stage("controller-release-begin");
  if (p.target != NULL) {
    GtkWidget *owner = gtk_event_controller_get_widget(GTK_EVENT_CONTROLLER(p.target));
    if (owner != NULL)
      gtk_widget_remove_controller(owner, GTK_EVENT_CONTROLLER(p.target));
    g_object_unref(p.target);
    if (p.released != 1U)
      failed = 1;
  }
  Stage("object-release-begin");
  g_clear_object(&p.widget);
  g_clear_object(&p.window);
  g_clear_object(&files[0]);
  g_clear_object(&files[1]);
  g_free(p.expected);
  g_free(first);
  g_free(second);
  Stage(failed ? "failed" : "completed");
  return failed;
}
