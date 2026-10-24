/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Old units and labels belong to historical records, never the active schema. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
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
  for (GtkWidget *child = gtk_widget_get_first_child(root); child != NULL;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = find(child, name);
    if (match != NULL)
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
  const char *cases[] = {"historical",     "unknown",      "fallback", "dirty",   "repeat",
                         "missing-active", "clone-active", "late",     "retired", "retire-read"};
  if (argc != 3)
    return 2;
  bool selected = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      selected = true;
  if (!selected)
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  FormFixture *old = form_fixture_new(), *current = form_fixture_new();
  UmiStructuredFormCatalogue *catalogue = NULL;
  UmiDataServer *writer = NULL;
  GtkWidget *root = NULL;
  char *directory = NULL, *database = NULL;
  UmiStructuredFormSessionView *state = malloc(sizeof(*state));
  CHECK(old && current && state);
  current->schema.version = 2;
  current->rules[0].unit_id = "mass.lb";
  current->fields[0].label = "Current mass";
  OK(umi_structured_form_init(&current->schema, &current->before));
  UmiStructuredFormSchema clone = current->schema;
  const UmiStructuredFormSchema *schemas[] = {&current->schema, &old->schema};
  size_t count = 2;
  if (strcmp(name, "unknown") == 0)
    count = 1;
  if (strcmp(name, "missing-active") == 0) {
    schemas[0] = &old->schema;
    count = 1;
  }
  if (strcmp(name, "clone-active") == 0)
    schemas[0] = &clone;
  OK(umi_structured_form_catalogue_create(schemas, count, &catalogue));
  UmiFormRecordEditorGtk4Options options = {.schema = &current->schema,
                                            .entity_prefix = "version.form.",
                                            .source = "Synthetic version fixture"};
  OK(UmiFormRecordEditorGtk4New(&options, &root));
  g_object_ref_sink(root);
  if (strcmp(name, "retired") == 0) {
    UmiFormRecordEditorGtk4Retire(root);
    CHECK(UmiFormRecordEditorGtk4SetHistoryCatalogue(root, catalogue) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "missing-active") == 0 || strcmp(name, "clone-active") == 0) {
    CHECK(UmiFormRecordEditorGtk4SetHistoryCatalogue(root, catalogue) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (strcmp(name, "fallback") != 0 && strcmp(name, "late") != 0)
    OK(UmiFormRecordEditorGtk4SetHistoryCatalogue(root, catalogue));
  if (strcmp(name, "repeat") == 0) {
    CHECK(UmiFormRecordEditorGtk4SetHistoryCatalogue(root, catalogue) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "form-versions-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory) != NULL);
  database = g_build_filename(directory, "history.sqlite3", NULL);
  OK(umi_data_server_create_sqlite(database, &writer));
  UmiRecordAmendment metadata = {.entity_id = "version.form.one",
                                 .source = "Synthetic source",
                                 .actor = "Example author",
                                 .reason = "Versioned fixture",
                                 .recorded_at_nanoseconds = 1U};
  UmiRecordRevision saved;
  UmiFormAnswer answer = form_mass(7);
  OK(umi_structured_form_set(&old->schema, &old->before, old->before.revision, "parcel.mass",
                             &answer));
  OK(umi_structured_form_record_save(writer, &metadata, &old->schema, &old->before, 0, &saved));
  answer = form_mass(9);
  memcpy(answer.unit_id, "mass.lb", 8);
  OK(umi_structured_form_set(&current->schema, &current->before, current->before.revision,
                             "parcel.mass", &answer));
  OK(umi_structured_form_record_save(writer, &metadata, &current->schema, &current->before, 1,
                                     &saved));
  OK(UmiFormRecordEditorGtk4Connect(root, database));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  if (strcmp(name, "late") == 0) {
    CHECK(UmiFormRecordEditorGtk4SetHistoryCatalogue(root, catalogue) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4Open(root, "one"));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  OK(UmiFormRecordEditorGtk4History(root, 0));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  GtkWidget *input = find(root, "parcel.mass.value");
  CHECK(input != NULL);
  if (strcmp(name, "dirty") == 0)
    gtk_editable_set_text(GTK_EDITABLE(input), "12");
  OK(UmiFormRecordEditorGtk4InspectRevision(root, 1));
  if (strcmp(name, "retire-read") == 0)
    UmiFormRecordEditorGtk4Retire(root);
  CHECK(idle(root));
  if (strcmp(name, "retire-read") == 0) {
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_CANCELLED);
    goto cleanup;
  }
  if (strcmp(name, "unknown") == 0 || strcmp(name, "fallback") == 0) {
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
    CHECK(find(root, "form-inspect.parcel.mass.value") == NULL);
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  GtkWidget *label = find(root, "form-inspect.parcel.mass.value");
  CHECK(GTK_IS_LABEL(label));
  const char *value = gtk_label_get_text(GTK_LABEL(label));
  CHECK(strstr(value, "7") != NULL && strstr(value, "mass.kg") != NULL &&
        strstr(value, "mass.lb") == NULL);
  CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(input)),
               strcmp(name, "dirty") == 0 ? "12" : "9") == 0);
  OK(UmiFormRecordEditorGtk4State(root, state));
  CHECK(state->record.expected_revision == 2 && state->form.schema_version == 2);
  if (strcmp(name, "dirty") == 0)
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
cleanup:
  if (root != NULL) {
    if (!idle(root))
      result = 1;
    UmiFormRecordEditorGtk4Retire(root);
    g_object_unref(root);
  }
  umi_data_server_destroy(writer);
  umi_structured_form_catalogue_destroy(catalogue);
  if (database != NULL) {
    (void)g_remove(database);
    const char *suffixes[] = {"-wal", "-shm", "-journal"};
    for (size_t i = 0; i < 3; ++i) {
      char *side = g_strconcat(database, suffixes[i], NULL);
      (void)g_remove(side);
      g_free(side);
    }
  }
  if (directory != NULL)
    (void)g_rmdir(directory);
  g_free(database);
  g_free(directory);
  free(state);
  free(current);
  free(old);
  return result;
}
