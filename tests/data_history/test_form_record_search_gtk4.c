/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_form_record_search_gtk4.c
 *
 * PURPOSE:
 *   * Search the saved answer model without replacing a dirty or outdated native view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_record.h"
#include "umicom/ui/gtk4/form_record_editor.h"
#include "umicom/ui/gtk4/structured_form_filter.h"
#include "../support/gtk4_expanders.h"
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
  if (!root)
    return NULL;
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
    g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return !UmiFormRecordEditorGtk4Busy(root);
}
static guint result_count(GtkWidget *root) {
  GListModel *model = gtk_drop_down_get_model(GTK_DROP_DOWN(find(root, "form-record-results")));
  return model ? g_list_model_get_n_items(model) : 0;
}
static void change_filter(GObject *object, GParamSpec *property, gpointer data) {
  (void)object;
  (void)property;
  GtkWidget *root = data;
  gtk_editable_set_text(GTK_EDITABLE(find(find(root, "form-record-filter"), "form-filter-value")),
                        "99");
}
static UmiStatus store(FormFixture *f, UmiDataServer *writer, unsigned index, uint64_t expected,
                       bool unknown, bool damaged) {
  UmiRecordAmendment metadata = {.source = "Synthetic search fixture",
                                 .actor = "Example author",
                                 .recorded_at_nanoseconds = 1};
  (void)snprintf(metadata.entity_id, sizeof metadata.entity_id, "search.form.%02u", index);
  strcpy(metadata.reason, index % 2 ? "odd" : "even");
  UmiRecordRevision saved;
  if (damaged)
    return umi_record_document_append(writer, &metadata, "not a form", 10, expected, &saved);
  UmiStatus status = umi_structured_form_init(&f->schema, &f->before);
  UmiFormAnswer answer = unknown ? form_missing(UMI_FORM_UNKNOWN, "Fictional missing value")
                                 : form_mass((double)index);
  if (status == UMI_STATUS_OK)
    status =
        umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_record_save(writer, &metadata, &f->schema, &f->before, expected,
                                             &saved);
  return status;
}
int main(int argc, char **argv) {
  const char *cases[] = {"basic",
                         "empty",
                         "pages",
                         "metadata",
                         "state",
                         "known-version",
                         "unknown-version",
                         "damaged",
                         "invalid-input",
                         "invalid-filter",
                         "late-filter",
                         "late-metadata",
                         "filter-after-result",
                         "late-open-filter",
                         "open-stale",
                         "changed-head",
                         "dirty-search",
                         "mode-to-metadata",
                         "mode-to-typed",
                         "no-next",
                         "busy",
                         "retire-read",
                         "dispose-read",
                         "retired",
                         "null",
                         "long-query",
                         "button",
                         "completion-filter",
                         "acceptance-filter",
                         "retained-button",
                         "not-connected"};
  if (argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new(), *old = form_fixture_new();
  UmiDataServer *writer = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  GtkWidget *root = NULL, *retained = NULL;
  char *directory = NULL, *database = NULL;
  UmiStructuredFormFilter *filter = g_new0(UmiStructuredFormFilter, 1);
  UmiStructuredFormSessionView *state = g_new0(UmiStructuredFormSessionView, 1);
  gulong callback = 0;
  GtkWidget *callback_source = NULL;
  CHECK(f && old);
  old->schema.version = 2;
  const UmiStructuredFormSchema *schemas[] = {&f->schema, &old->schema};
  OK(umi_structured_form_catalogue_create(schemas, 2, &catalogue));
  UmiFormRecordEditorGtk4Options options = {
      .schema = &f->schema, .entity_prefix = "search.form.", .source = "Fictional record search"};
  OK(UmiFormRecordEditorGtk4New(&options, &root));
  g_object_ref_sink(root);
  UmiTestExpandSections(root);
  CHECK(find(root, "form-record-filter"));
  filter->count = 1;
  strcpy(filter->terms[0].field_id, "parcel.mass");
  filter->terms[0].operation = UMI_FORM_FILTER_EQUAL;
  filter->terms[0].operand = form_mass(5);
  OK(UmiFormRecordEditorGtk4SetFilter(root, filter));
  if (strcmp(name, "null") == 0) {
    CHECK(UmiFormRecordEditorGtk4SetFilter(NULL, filter) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordEditorGtk4SetFilter(root, NULL) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4FindTyped(NULL, "", 0) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (strcmp(name, "retired") == 0) {
    UmiFormRecordEditorGtk4Retire(root);
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, "", 0) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiFormRecordEditorGtk4SetFilter(root, filter) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "retained-button") == 0) {
    retained = g_object_ref(find(root, "form-record-find-typed"));
    g_object_unref(root);
    root = NULL;
    g_signal_emit_by_name(retained, "clicked");
    goto cleanup;
  }
  if (strcmp(name, "not-connected") == 0) {
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, "", 0) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (strcmp(name, "known-version") == 0)
    OK(UmiFormRecordEditorGtk4SetHistoryCatalogue(root, catalogue));
  CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "form-search-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory));
  database = g_build_filename(directory, "records.sqlite3", NULL);
  OK(umi_data_server_create_sqlite(database, &writer));
  for (unsigned i = 0; i < 20; ++i) {
    bool alternate =
        i == 19 && (strcmp(name, "known-version") == 0 || strcmp(name, "unknown-version") == 0);
    OK(store(alternate ? old : f, writer, i, 0, i == 19 && strcmp(name, "state") == 0,
             i == 19 && strcmp(name, "damaged") == 0));
  }
  OK(UmiFormRecordEditorGtk4Connect(root, database));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  if (strcmp(name, "pages") == 0 || strcmp(name, "changed-head") == 0 || strstr(name, "version"))
    filter->count = 0;
  if (strcmp(name, "empty") == 0)
    filter->terms[0].operand = form_mass(50);
  if (strcmp(name, "metadata") == 0) {
    filter->terms[0].operation = UMI_FORM_FILTER_LESS;
    filter->terms[0].operand = form_mass(4);
  }
  if (strcmp(name, "state") == 0) {
    filter->terms[0].operation = UMI_FORM_FILTER_STATE;
    filter->terms[0].operand = (UmiFormAnswer){.state = UMI_FORM_UNKNOWN};
  }
  OK(UmiFormRecordEditorGtk4SetFilter(root, filter));
  if (strcmp(name, "invalid-filter") == 0) {
    strcpy(filter->terms[0].operand.unit_id, "mass.lb");
    CHECK(UmiFormRecordEditorGtk4SetFilter(root, filter) != UMI_STATUS_OK);
    strcpy(filter->terms[0].operand.unit_id, "mass.kg");
  }
  GtkWidget *value = find(find(root, "form-record-filter"), "form-filter-value");
  if (strcmp(name, "invalid-input") == 0) {
    gtk_editable_set_text(GTK_EDITABLE(value), "-");
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, "", 0) != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4Busy(root));
    goto cleanup;
  }
  if (strcmp(name, "long-query") == 0) {
    char query[200];
    memset(query, 'a', sizeof query - 1);
    query[sizeof query - 1] = '\0';
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, query, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
    goto cleanup;
  }
  if (strcmp(name, "mode-to-typed") == 0) {
    OK(UmiFormRecordEditorGtk4Find(root, "", 0));
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, "", 1) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "dirty-search") == 0) {
    OK(UmiFormRecordEditorGtk4Open(root, "05"));
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    gtk_editable_set_text(GTK_EDITABLE(find(root, "parcel.mass.value")), "12");
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
  }
  if (strcmp(name, "completion-filter") == 0) {
    callback_source = find(root, "form-record-results");
    callback = g_signal_connect(callback_source, "notify::model", G_CALLBACK(change_filter), root);
  }
  if (strcmp(name, "acceptance-filter") == 0) {
    callback_source = find(root, "form-record-search");
    callback = g_signal_connect(callback_source, "notify::text", G_CALLBACK(change_filter), root);
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, "even", 0) == UMI_STATUS_CANCELLED);
    CHECK(!UmiFormRecordEditorGtk4Busy(root));
    goto cleanup;
  }
  const char *metadata = strcmp(name, "metadata") == 0 ? "even" : "";
  if (strcmp(name, "button") == 0) {
    g_signal_emit_by_name(find(root, "form-record-find-typed"), "clicked");
    CHECK(UmiFormRecordEditorGtk4Busy(root));
  } else
    OK(UmiFormRecordEditorGtk4FindTyped(root, metadata, 0));
  if (strcmp(name, "busy") == 0) {
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, "", 0) == UMI_STATUS_BUSY);
    CHECK(UmiFormRecordEditorGtk4SetFilter(root, filter) == UMI_STATUS_BUSY);
  }
  if (strcmp(name, "late-filter") == 0)
    gtk_editable_set_text(GTK_EDITABLE(value), "6");
  if (strcmp(name, "late-metadata") == 0)
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search")), "even");
  if (strcmp(name, "retire-read") == 0)
    UmiFormRecordEditorGtk4Retire(root);
  if (strcmp(name, "dispose-read") == 0)
    g_object_run_dispose(G_OBJECT(root));
  CHECK(idle(root));
  if (strcmp(name, "late-filter") == 0 || strcmp(name, "late-metadata") == 0 ||
      strcmp(name, "retire-read") == 0 || strcmp(name, "dispose-read") == 0 ||
      strcmp(name, "completion-filter") == 0 || strcmp(name, "unknown-version") == 0 ||
      strcmp(name, "damaged") == 0) {
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  guint expected = strcmp(name, "empty") == 0      ? 0U
                   : strcmp(name, "metadata") == 0 ? 2U
                   : (strcmp(name, "pages") == 0 || strcmp(name, "known-version") == 0 ||
                      strcmp(name, "changed-head") == 0)
                       ? 16U
                       : 1U;
  CHECK(result_count(root) == expected);
  if (strcmp(name, "empty") == 0 || strcmp(name, "metadata") == 0)
    goto cleanup;
  if (strcmp(name, "mode-to-metadata") == 0) {
    CHECK(UmiFormRecordEditorGtk4Find(root, "", 1) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "no-next") == 0) {
    CHECK(UmiFormRecordEditorGtk4FindTyped(root, "", 1) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "pages") == 0 || strcmp(name, "known-version") == 0 ||
      strcmp(name, "changed-head") == 0) {
    if (strcmp(name, "changed-head") == 0)
      OK(store(f, writer, 19, 1, false, false));
    OK(UmiFormRecordEditorGtk4FindTyped(root, "", 1));
    CHECK(idle(root));
    if (strcmp(name, "changed-head") == 0)
      CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_INVALID_STATE);
    else {
      OK(UmiFormRecordEditorGtk4LastStatus(root));
      CHECK(result_count(root) == (strcmp(name, "known-version") == 0 ? 3U : 4U));
    }
    goto cleanup;
  }
  if (strcmp(name, "filter-after-result") == 0) {
    OK(UmiFormRecordEditorGtk4SetFilter(root, filter));
    CHECK(UmiFormRecordEditorGtk4OpenListed(root, 0) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "dirty-search") == 0) {
    CHECK(UmiFormRecordEditorGtk4OpenListed(root, 0) == UMI_STATUS_INVALID_STATE);
    CHECK(strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "12") == 0);
    goto cleanup;
  }
  if (strcmp(name, "open-stale") == 0)
    OK(store(f, writer, 5, 1, false, false));
  OK(UmiFormRecordEditorGtk4OpenListed(root, 0));
  if (strcmp(name, "late-open-filter") == 0)
    gtk_editable_set_text(GTK_EDITABLE(value), "6");
  CHECK(idle(root));
  if (strcmp(name, "open-stale") == 0 || strcmp(name, "late-open-filter") == 0)
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
  else {
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(UmiFormRecordEditorGtk4State(root, state));
    CHECK(strcmp(state->record.entity_id,
                 strcmp(name, "state") == 0 ? "search.form.19" : "search.form.05") == 0);
  }
cleanup:
  if (root) {
    if (!idle(root)) {
      fprintf(stderr, "Worker still running; preserve fixture lifetime until process exit.\n");
      return 1;
    }
    if (callback)
      g_signal_handler_disconnect(callback_source, callback);
    UmiFormRecordEditorGtk4Retire(root);
    g_object_unref(root);
  }
  if (retained)
    g_object_unref(retained);
  umi_data_server_destroy(writer);
  umi_structured_form_catalogue_destroy(catalogue);
  if (database) {
    (void)g_remove(database);
    const char *suffixes[] = {"-wal", "-shm", "-journal"};
    for (size_t i = 0; i < 3; ++i) {
      char *side = g_strconcat(database, suffixes[i], NULL);
      (void)g_remove(side);
      g_free(side);
    }
  }
  if (directory)
    (void)g_rmdir(directory);
  g_free(database);
  g_free(directory);
  g_free(filter);
  g_free(state);
  free(f);
  free(old);
  return result;
}
