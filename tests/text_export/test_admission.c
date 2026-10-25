/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/text_export/test_admission.c
 *
 * PURPOSE:
 *   Refuse stale reports and serialize owner callbacks, feedback and worker capture.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/automation.h"
#include "umicom/ui/gtk4/text_export.h"
#include <glib/gstdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%d: %s\n", __LINE__, #x);                                                   \
      exit(1);                                                                                     \
    }                                                                                              \
  } while (0)
typedef struct Guard {
  GtkWidget *root, *button, *entry;
  GtkTextBuffer *buffer;
  const char *mode, *other;
  unsigned calls, feedback;
  int *released;
} Guard;
static UmiStatus Admit(void *data) {
  Guard *g = data;
  ++g->calls;
  CHECK(UmiTextExportGtk4Busy(g->root));
  if (!strcmp(g->mode, "denied") || !strcmp(g->mode, "feedback-refusal"))
    return UMI_STATUS_PERMISSION_DENIED;
  if (!strcmp(g->mode, "stale"))
    return UMI_STATUS_BUSY;
  if (!strcmp(g->mode, "callback-reentry"))
    g_signal_emit_by_name(g->button, "clicked");
  if (!strcmp(g->mode, "callback-retire"))
    UmiTextExportGtk4Retire(g->root);
  if (!strcmp(g->mode, "callback-updates"))
    gtk_text_buffer_set_text(g->buffer, "Reviewed replacement", -1);
  return UMI_STATUS_OK;
}
static void Released(gpointer data) {
  Guard *g = data;
  ++*g->released;
  g_free(g);
}
static GtkWidget *Find(GtkWidget *root, const char *id) {
  return umi_gtk4_automation_find_tagged_widget(root, id);
}
static void Wait(GtkWidget *root) {
  gint64 end = g_get_monotonic_time() + 10 * G_TIME_SPAN_SECOND;
  while (UmiTextExportGtk4Busy(root) && g_get_monotonic_time() < end) {
    while (g_main_context_iteration(NULL, FALSE)) {
    }
    g_usleep(1000);
  }
  CHECK(!UmiTextExportGtk4Busy(root));
}
static void Feedback(GObject *object, GParamSpec *property, gpointer data) {
  (void)property;
  Guard *g = data;
  const char *text = gtk_label_get_text(GTK_LABEL(object));
  if (strncmp(text, "Saved captured", 14U) && strncmp(text, "Cannot capture", 14U))
    return;
  ++g->feedback;
  CHECK(UmiTextExportGtk4Busy(g->root));
  gtk_editable_set_text(GTK_EDITABLE(g->entry), g->other);
  g_signal_emit_by_name(g->button, "clicked");
  if (!strcmp(g->mode, "feedback-retire"))
    UmiTextExportGtk4Retire(g->root);
}
int main(int argc, char **argv) {
  CHECK(argc == 3);
  const char *const cases[] = {"normal",
                               "constructor",
                               "denied",
                               "stale",
                               "callback-reentry",
                               "callback-retire",
                               "callback-updates",
                               "feedback-refusal",
                               "feedback-completion",
                               "feedback-retire",
                               "source-gone",
                               "invalid-path",
                               "retired",
                               "accepted-snapshot"};
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  CHECK(known);
  if (!gtk_init_check())
    return 77;
  char *directory = g_build_filename(argv[2], "admission-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory) != NULL);
  char *path = g_build_filename(directory, "report.txt", NULL),
       *other = g_build_filename(directory, "other.txt", NULL);
  int released = 0;
  Guard *g = g_new0(Guard, 1);
  g->released = &released;
  g->mode = argv[1];
  g->other = other;
  g->buffer = gtk_text_buffer_new(NULL);
  gtk_text_buffer_set_text(g->buffer, "Reviewed report", -1);
  if (!strcmp(argv[1], "constructor")) {
    CHECK(UmiTextExportGtk4CreateGuardedBuffer(g->buffer, "report.txt", NULL, g, Released) == NULL);
    CHECK(UmiTextExportGtk4CreateGuardedBuffer(g->buffer, "", Admit, g, Released) == NULL &&
          released == 0);
    g_object_unref(g->buffer);
    Released(g);
    CHECK(released == 1);
    g_free(directory);
    g_free(path);
    g_free(other);
    return 0;
  }
  g->root = UmiTextExportGtk4CreateGuardedBuffer(g->buffer, "report.txt", Admit, g, Released);
  CHECK(g->root != NULL);
  g_object_ref_sink(g->root);
  gtk_expander_set_expanded(GTK_EXPANDER(g->root), TRUE);
  g->button = Find(g->root, "text.export.save");
  g->entry = Find(g->root, "text.export.path");
  CHECK(g->button && g->entry);
  gtk_editable_set_text(GTK_EDITABLE(g->entry),
                        !strcmp(argv[1], "invalid-path") ? "relative.txt" : path);
  if (!strncmp(argv[1], "feedback", 8U))
    g_signal_connect(Find(g->root, "text.export.status"), "notify::label", G_CALLBACK(Feedback), g);
  if (!strcmp(argv[1], "source-gone"))
    g_clear_object(&g->buffer);
  if (!strcmp(argv[1], "retired"))
    UmiTextExportGtk4Retire(g->root);
  g_signal_emit_by_name(g->button, "clicked");
  if (!strcmp(argv[1], "accepted-snapshot")) {
    gtk_text_buffer_set_text(g->buffer, "Later text", -1);
    g->mode = "denied";
  }
  Wait(g->root);
  bool refusal = !strcmp(argv[1], "denied") || !strcmp(argv[1], "stale") ||
                 !strcmp(argv[1], "callback-retire") || !strcmp(argv[1], "feedback-refusal") ||
                 !strcmp(argv[1], "source-gone") || !strcmp(argv[1], "invalid-path") ||
                 !strcmp(argv[1], "retired");
  CHECK(g_file_test(path, G_FILE_TEST_EXISTS) != refusal &&
        !g_file_test(other, G_FILE_TEST_EXISTS));
  CHECK(g->calls == ((!strcmp(argv[1], "invalid-path") || !strcmp(argv[1], "retired")) ? 0U : 1U));
  if (!refusal) {
    char *contents = NULL;
    CHECK(g_file_get_contents(path, &contents, NULL, NULL));
    CHECK(!strcmp(contents, !strcmp(argv[1], "callback-updates") ? "Reviewed replacement"
                                                                 : "Reviewed report"));
    g_free(contents);
  }
  if (!strncmp(argv[1], "feedback", 8U))
    CHECK(g->feedback == 1U);
  GtkWidget *root = g->root;
  g_clear_object(&g->buffer);
  UmiTextExportGtk4Retire(root);
  g_object_unref(root);
  CHECK(released == 1);
  g_free(directory);
  g_free(path);
  g_free(other);
  return 0;
}
