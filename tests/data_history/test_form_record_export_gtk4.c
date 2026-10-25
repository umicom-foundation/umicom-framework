/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_form_record_export_gtk4.c
 *
 * PURPOSE:
 *   Saved-history export captures its destination and baseline before dispatch.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Saved-history export captures its destination and baseline before dispatch. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/record_bundle_files.h"
#include "umicom/data/structured_form_record.h"
#include "umicom/ui/gtk4/form_record_editor.h"
#include <glib/gstdio.h>
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static GtkWidget *find(GtkWidget *root, const char *name) {
  if (strcmp(gtk_widget_get_name(root), name) == 0)
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = find(child, name);
    if (match)
      return match;
  }
  return NULL;
}
static bool idle(GtkWidget *root) {
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  while (UmiFormRecordEditorGtk4Busy(root) && g_get_monotonic_time() < until) {
    (void)g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return !UmiFormRecordEditorGtk4Busy(root);
}
int main(int argc, char **argv) {
  const char *cases[] = {"roundtrip",  "buttons",      "dirty",         "unsaved",
                         "empty",      "disconnected", "relative",      "existing",
                         "stale",      "retire-write", "dispose-write", "later-edit",
                         "later-path", "busy",         "capacity",      "missing-parent"};
  if (argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  FormFixture *fixture = form_fixture_new();
  UmiDataServer *store = NULL;
  GtkWidget *root = NULL;
  UmiRecordBundle *bundle = NULL;
  UmiRecordDocument *document = malloc(sizeof(*document));
  UmiStructuredFormSessionView *state = malloc(sizeof(*state));
  char *directory = NULL, *database = NULL, *path = NULL, *alternative = NULL;
  CHECK(fixture && document && state);
  CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "form-export-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory));
  database = g_build_filename(directory, "source.sqlite3", NULL);
  path = strcmp(name, "missing-parent") == 0
             ? g_build_filename(directory, "missing", "history.umibundle", NULL)
             : g_build_filename(directory, "history.umibundle", NULL);
  alternative = g_build_filename(directory, "another.umibundle", NULL);
  UmiFormRecordEditorGtk4Options options = {.schema = &fixture->schema,
                                            .entity_prefix = "export.form.",
                                            .source = "Synthetic editor fixture"};
  OK(UmiFormRecordEditorGtk4New(&options, &root));
  g_object_ref_sink(root);
  GtkWidget *path_entry = find(root, "form-record-export-path");
  GtkWidget *button = find(root, "form-record-export-history");
  CHECK(path_entry && button);
  if (strcmp(name, "disconnected") == 0) {
    CHECK(UmiFormRecordEditorGtk4ExportHistory(root, path) == UMI_STATUS_INVALID_STATE);
    CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    goto cleanup;
  }
  OK(umi_data_server_create_sqlite(database, &store));
  UmiRecordAmendment author = {.entity_id = "export.form.one",
                               .source = "Synthetic source",
                               .actor = "Example author",
                               .reason = "Export regression",
                               .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  size_t revisions = strcmp(name, "capacity") == 0 ? UMI_RECORD_BUNDLE_MAX_REVISIONS + 1U : 2U;
  for (size_t i = 0; i < revisions; ++i) {
    UmiFormAnswer answer = form_mass((double)(i + 1));
    OK(umi_structured_form_set(&fixture->schema, &fixture->before, fixture->before.revision,
                               "parcel.mass", &answer));
    author.recorded_at_nanoseconds = (uint64_t)i + 1;
    OK(umi_structured_form_record_save(store, &author, &fixture->schema, &fixture->before,
                                       (uint64_t)i, &saved));
  }
  OK(UmiFormRecordEditorGtk4Connect(root, database));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  if (strcmp(name, "empty") == 0 || strcmp(name, "unsaved") == 0) {
    if (strcmp(name, "unsaved") == 0)
      OK(UmiFormRecordEditorGtk4Begin(root, "new"));
    CHECK(UmiFormRecordEditorGtk4ExportHistory(root, path) == UMI_STATUS_INVALID_STATE);
    CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4Open(root, "one"));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  GtkWidget *input = find(root, "parcel.mass.value");
  CHECK(input);
  if (strcmp(name, "dirty") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(input), "12");
    CHECK(UmiFormRecordEditorGtk4ExportHistory(root, path) == UMI_STATUS_INVALID_STATE);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(input)), "12") == 0);
    CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    goto cleanup;
  }
  if (strcmp(name, "relative") == 0) {
    CHECK(UmiFormRecordEditorGtk4ExportHistory(root, "relative.umibundle") ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
    goto cleanup;
  }
  if (strcmp(name, "existing") == 0)
    CHECK(g_file_set_contents(path, "preserve", 8, NULL));
  if (strcmp(name, "stale") == 0) {
    author.recorded_at_nanoseconds = 3;
    OK(umi_structured_form_record_save(store, &author, &fixture->schema, &fixture->before, 2,
                                       &saved));
  }
  gtk_editable_set_text(GTK_EDITABLE(path_entry), path);
  if (strcmp(name, "buttons") == 0)
    g_signal_emit_by_name(button, "clicked");
  else
    OK(UmiFormRecordEditorGtk4ExportHistory(root, path));
  CHECK(UmiFormRecordEditorGtk4Busy(root));
  CHECK(!UmiFormRecordEditorGtk4CanClose(root));
  if (strcmp(name, "busy") == 0)
    CHECK(UmiFormRecordEditorGtk4ExportHistory(root, alternative) == UMI_STATUS_BUSY);
  if (strcmp(name, "later-edit") == 0)
    gtk_editable_set_text(GTK_EDITABLE(input), "12");
  if (strcmp(name, "later-path") == 0)
    gtk_editable_set_text(GTK_EDITABLE(path_entry), alternative);
  if (strcmp(name, "retire-write") == 0)
    UmiFormRecordEditorGtk4Retire(root);
  if (strcmp(name, "dispose-write") == 0)
    g_object_run_dispose(G_OBJECT(root));
  CHECK(idle(root));
  UmiStatus status = UmiFormRecordEditorGtk4LastStatus(root);
  if (strcmp(name, "existing") == 0) {
    CHECK(status == UMI_STATUS_ALREADY_EXISTS);
    char *bytes = NULL;
    gsize length = 0;
    CHECK(g_file_get_contents(path, &bytes, &length, NULL));
    bool unchanged = length == 8 && memcmp(bytes, "preserve", 8) == 0;
    g_free(bytes);
    CHECK(unchanged);
  } else if (strcmp(name, "stale") == 0 || strcmp(name, "capacity") == 0 ||
             strcmp(name, "missing-parent") == 0) {
    CHECK(status != UMI_STATUS_OK);
    CHECK(!g_file_test(path, G_FILE_TEST_EXISTS));
  } else {
    OK(status);
    OK(umi_record_bundle_read(directory, "history.umibundle", &bundle));
    CHECK(umi_record_bundle_count(bundle) == 2);
    OK(umi_record_bundle_at(bundle, 1, document));
    CHECK(document->record.revision == 2);
    if (strcmp(name, "later-edit") == 0) {
      CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(input)), "12") == 0);
      CHECK(!UmiFormRecordEditorGtk4CanClose(root));
    }
    if (strcmp(name, "later-path") == 0 || strcmp(name, "busy") == 0)
      CHECK(!g_file_test(alternative, G_FILE_TEST_EXISTS));
  }
  OK(UmiFormRecordEditorGtk4State(root, state));
  CHECK(state->record.expected_revision == 2 || strcmp(name, "capacity") == 0);
cleanup:
  /* An accepted task still borrows the fixture schema. Never release that
   * storage while a timed-out worker may retain the editor. Process exit owns it. */
  if (root && !idle(root))
    return 1;
  if (root) {
    UmiFormRecordEditorGtk4Retire(root);
    g_object_unref(root);
  }
  umi_record_bundle_destroy(bundle);
  umi_data_server_destroy(store);
  free(document);
  free(state);
  free(fixture);
  g_free(alternative);
  g_free(path);
  g_free(database);
  g_free(directory);
  return result;
}
