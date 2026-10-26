/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_form_record_search_presets_gtk4.c
 *
 * PURPOSE:
 *   Saved-query writes retain captured input; late read results cannot replace edits.
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
#include "umicom/data/structured_form_search_preset.h"
#include "umicom/ui/gtk4/form_record_editor.h"
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
  if (!strcmp(gtk_widget_get_name(root), name))
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
static GtkWidget *filter_value(GtkWidget *root) {
  return find(find(root, "form-record-filter"), "form-filter-value");
}
static void replace_filter(GObject *object, GParamSpec *spec, gpointer context) {
  (void)object;
  (void)spec;
  gtk_editable_set_text(GTK_EDITABLE(filter_value(context)), "7");
}
static void replace_title(GObject *object, GParamSpec *spec, gpointer context) {
  (void)object;
  (void)spec;
  gtk_editable_set_text(GTK_EDITABLE(find(context, "form-record-search-title")), "Observer title");
}
static void replace_key(GObject *object, GParamSpec *spec, gpointer context) {
  (void)object;
  (void)spec;
  gtk_editable_set_text(GTK_EDITABLE(find(context, "form-record-search-key")), "other");
}
static void filter_observer_title(GtkWidget *widget, guint64 revision, gpointer context) {
  (void)widget;
  (void)revision;
  replace_title(NULL, NULL, context);
}
static void retire_observer(GObject *object, GParamSpec *spec, gpointer context) {
  (void)object;
  (void)spec;
  UmiFormRecordEditorGtk4Retire(context);
}
int main(int argc, char **argv) {
  const char *cases[] = {"create",
                         "write-captured",
                         "retire-save",
                         "dispose-save",
                         "save-feedback-retire",
                         "collision",
                         "stale",
                         "zero-time",
                         "invalid-title",
                         "invalid-input",
                         "invalid-key",
                         "key-length",
                         "expected-overflow",
                         "open",
                         "missing",
                         "namespace",
                         "schema",
                         "utf8-title",
                         "utf8-operand",
                         "late-filter",
                         "late-metadata",
                         "late-title",
                         "late-key",
                         "retire-open",
                         "dispose-open",
                         "dirty-record",
                         "completion-filter",
                         "completion-retire",
                         "completion-title",
                         "completion-key",
                         "filter-observer-title",
                         "button-create",
                         "button-update",
                         "button-update-key",
                         "retained-button",
                         "null",
                         "retired"};
  if (argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known)
    return 2;
  if (!gtk_init_check())
    return 77;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  UmiDataServer *writer = NULL;
  GtkWidget *root = NULL, *retained = NULL, *observed = NULL;
  gulong callback = 0;
  char *directory = NULL, *database = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormSearchPresetRecord *loaded = malloc(sizeof *loaded);
  UmiStructuredFormSessionView *state = malloc(sizeof *state);
  CHECK(f && preset && loaded && state);
  strcpy(preset->title, "Stored search");
  strcpy(preset->entity_prefix, strcmp(name, "namespace") == 0 ? "other.form." : "preset.form.");
  strcpy(preset->metadata_text, "stored");
  preset->filter.count = 1;
  strcpy(preset->filter.terms[0].field_id, "parcel.mass");
  preset->filter.terms[0].operation = UMI_FORM_FILTER_EQUAL;
  preset->filter.terms[0].operand = form_mass(9);
  if (!strcmp(name, "utf8-title"))
    strcpy(preset->title, "\xff");
  if (!strcmp(name, "utf8-operand")) {
    strcpy(preset->filter.terms[0].field_id, "parcel.note");
    preset->filter.terms[0].operand = form_text("\xff");
  }
  CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "native-presets-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory));
  database = g_build_filename(directory, "presets.sqlite3", NULL);
  OK(umi_data_server_create_sqlite(database, &writer));
  UmiRecordAmendment metadata = {.entity_id = "form.search.small",
                                 .source = "Synthetic search",
                                 .actor = "Example author",
                                 .reason = "Saved definition",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  if (!strcmp(name, "schema"))
    f->schema.version = 2;
  OK(umi_structured_form_search_preset_save(writer, &metadata, &f->schema, preset, 0, &saved));
  f->schema.version = 1;
  UmiFormRecordEditorGtk4Options options = {
      .schema = &f->schema, .entity_prefix = "preset.form.", .source = "Fictional preset editor"};
  OK(UmiFormRecordEditorGtk4New(&options, &root));
  g_object_ref_sink(root);
  UmiTestExpandSections(root);
  OK(UmiFormRecordEditorGtk4Connect(root, database));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  strcpy(preset->filter.terms[0].field_id, "parcel.mass");
  preset->filter.terms[0].operand = form_mass(5);
  OK(UmiFormRecordEditorGtk4SetFilter(root, &preset->filter));
  gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search")), "live");
  gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-title")), "Draft search");
  gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-key")), "small");
  UmiRecordRevision untouched;
  memset(&untouched, 0x6e, sizeof untouched);
  saved = untouched;
  CHECK(UmiFormRecordEditorGtk4SavedSearch(root, &saved) == UMI_STATUS_NOT_FOUND &&
        !memcmp(&saved, &untouched, sizeof saved));
  UmiRecordDraftAttribution attribution = {.source = "Synthetic preset editor",
                                           .actor = "Example author",
                                           .reason = "Keep reusable conditions",
                                           .recorded_at_nanoseconds = 2};
  if (!strcmp(name, "null")) {
    CHECK(UmiFormRecordEditorGtk4SaveSearch(NULL, "new", "Search", 0, &attribution) ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordEditorGtk4OpenSearch(root, NULL) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4SavedSearch(root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    goto cleanup;
  }
  if (!strcmp(name, "retired")) {
    UmiFormRecordEditorGtk4Retire(root);
    CHECK(UmiFormRecordEditorGtk4OpenSearch(root, "small") == UMI_STATUS_INVALID_STATE);
    CHECK(UmiFormRecordEditorGtk4SaveSearch(root, "new", "Search", 0, &attribution) ==
          UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (!strcmp(name, "retained-button")) {
    retained = g_object_ref(find(root, "form-record-search-create"));
    g_object_unref(root);
    root = NULL;
    g_signal_emit_by_name(retained, "clicked");
    goto cleanup;
  }
  bool saving = !strcmp(name, "create") || !strcmp(name, "write-captured") ||
                !strcmp(name, "retire-save") || !strcmp(name, "dispose-save") ||
                !strcmp(name, "save-feedback-retire") || !strcmp(name, "collision") ||
                !strcmp(name, "stale") || !strcmp(name, "zero-time") ||
                !strcmp(name, "invalid-title") || !strcmp(name, "invalid-input") ||
                !strcmp(name, "invalid-key") || !strcmp(name, "key-length") ||
                !strcmp(name, "expected-overflow") || !strcmp(name, "button-create");
  if (saving) {
    const char *key = !strcmp(name, "collision") || !strcmp(name, "stale") ? "small" : "new";
    char long_key[42];
    memset(long_key, 'x', sizeof long_key - 1);
    long_key[sizeof long_key - 1] = 0;
    if (!strcmp(name, "invalid-key"))
      key = "not.a.key";
    if (!strcmp(name, "key-length"))
      key = long_key;
    if (!strcmp(name, "zero-time"))
      attribution.recorded_at_nanoseconds = 0;
    if (!strcmp(name, "invalid-input"))
      gtk_editable_set_text(GTK_EDITABLE(filter_value(root)), "-");
    if (!strcmp(name, "stale")) {
      metadata.recorded_at_nanoseconds = 2;
      strcpy(preset->entity_prefix, "preset.form.");
      strcpy(preset->title, "Second search");
      OK(umi_structured_form_search_preset_save(writer, &metadata, &f->schema, preset, 1, &saved));
    }
    if (!strcmp(name, "save-feedback-retire")) {
      observed = g_object_ref(find(root, "form-record-feedback"));
      CHECK(observed);
      callback = g_signal_connect(observed, "notify::label", G_CALLBACK(retire_observer), root);
    }
    UmiStatus status;
    if (!strcmp(name, "button-create")) {
      gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-key")), "new");
      gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-title")),
                            "Captured search");
      gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-actor")), "Example author");
      gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-reason")), "Visible save");
      g_signal_emit_by_name(find(root, "form-record-search-create"), "clicked");
      status = UmiFormRecordEditorGtk4LastStatus(root);
    } else
      status = UmiFormRecordEditorGtk4SaveSearch(
          root, key, !strcmp(name, "invalid-title") ? "" : "Captured search",
          !strcmp(name, "stale")               ? 1
          : !strcmp(name, "expected-overflow") ? UINT64_MAX
                                               : 0,
          &attribution);
    if (!strcmp(name, "zero-time") || !strcmp(name, "invalid-title") ||
        !strcmp(name, "invalid-input") || !strcmp(name, "invalid-key") ||
        !strcmp(name, "key-length") || !strcmp(name, "expected-overflow")) {
      CHECK(status != UMI_STATUS_OK && !UmiFormRecordEditorGtk4Busy(root));
      goto cleanup;
    }
    OK(status);
    if (!strcmp(name, "write-captured")) {
      gtk_editable_set_text(GTK_EDITABLE(filter_value(root)), "7");
      gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search")), "later");
    }
    if (!strcmp(name, "retire-save"))
      UmiFormRecordEditorGtk4Retire(root);
    if (!strcmp(name, "dispose-save"))
      g_object_run_dispose(G_OBJECT(root));
    CHECK(idle(root));
    if (!strcmp(name, "collision") || !strcmp(name, "stale")) {
      CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
      goto cleanup;
    }
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(UmiFormRecordEditorGtk4SavedSearch(root, &saved));
    CHECK(saved.revision == 1 && !strcmp(saved.amendment.entity_id, "form.search.new"));
    OK(umi_structured_form_search_preset_load(writer, "form.search.new", 0, &f->schema, loaded));
    CHECK(!strcmp(loaded->preset.title, "Captured search") &&
          !strcmp(loaded->preset.metadata_text, "live") &&
          loaded->preset.filter.terms[0].operand.value.decimal_value == 5);
    if (!strcmp(name, "write-captured"))
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(filter_value(root))), "7"));
    goto cleanup;
  }
  if (!strcmp(name, "dirty-record")) {
    OK(UmiFormRecordEditorGtk4Begin(root, "draft"));
    gtk_editable_set_text(GTK_EDITABLE(find(root, "parcel.mass.value")), "12");
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
  }
  if (!strcmp(name, "completion-filter") || !strcmp(name, "completion-retire")) {
    observed = g_object_ref(find(root, "form-record-search"));
    callback = g_signal_connect(observed, "notify::text",
                                !strcmp(name, "completion-filter") ? G_CALLBACK(replace_filter)
                                                                   : G_CALLBACK(retire_observer),
                                root);
  }
  if (!strcmp(name, "completion-title") || !strcmp(name, "completion-key")) {
    observed = g_object_ref(find(root, "form-record-search"));
    callback = g_signal_connect(observed, "notify::text",
                                !strcmp(name, "completion-title") ? G_CALLBACK(replace_title)
                                                                  : G_CALLBACK(replace_key),
                                root);
  }
  if (!strcmp(name, "filter-observer-title")) {
    observed = g_object_ref(find(root, "form-record-filter"));
    callback =
        g_signal_connect(observed, "filter-changed", G_CALLBACK(filter_observer_title), root);
  }
  OK(UmiFormRecordEditorGtk4OpenSearch(root, !strcmp(name, "missing") ? "missing" : "small"));
  if (!strcmp(name, "late-filter"))
    gtk_editable_set_text(GTK_EDITABLE(filter_value(root)), "7");
  if (!strcmp(name, "late-metadata"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search")), "later");
  if (!strcmp(name, "late-title"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-title")), "Later title");
  if (!strcmp(name, "late-key"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-key")), "other");
  if (!strcmp(name, "retire-open"))
    UmiFormRecordEditorGtk4Retire(root);
  if (!strcmp(name, "dispose-open"))
    g_object_run_dispose(G_OBJECT(root));
  CHECK(idle(root));
  bool refused = !strcmp(name, "missing") || !strcmp(name, "namespace") ||
                 !strcmp(name, "schema") || !strcmp(name, "utf8-title") ||
                 !strcmp(name, "utf8-operand") || !strncmp(name, "late-", 5) ||
                 !strcmp(name, "retire-open") || !strcmp(name, "dispose-open") ||
                 !strcmp(name, "completion-filter") || !strcmp(name, "completion-retire") ||
                 !strcmp(name, "completion-title") || !strcmp(name, "completion-key") ||
                 !strcmp(name, "filter-observer-title");
  if (refused) {
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4SavedSearch(root, &saved) == UMI_STATUS_NOT_FOUND);
    if (!strcmp(name, "completion-filter") || !strcmp(name, "late-filter"))
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(filter_value(root))), "7"));
    if (!strcmp(name, "completion-title") || !strcmp(name, "filter-observer-title"))
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "form-record-search-title"))),
                    "Observer title"));
    if (!strcmp(name, "completion-key"))
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "form-record-search-key"))),
                    "other"));
    if (!strcmp(name, "late-title"))
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "form-record-search-title"))),
                    "Later title"));
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  OK(UmiFormRecordEditorGtk4SavedSearch(root, &saved));
  CHECK(saved.revision == 1);
  CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "form-record-search-title"))),
                "Stored search") &&
        !strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "form-record-search"))), "stored"));
  if (!strcmp(name, "dirty-record")) {
    CHECK(!UmiFormRecordEditorGtk4CanClose(root));
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "12"));
  }
  if (!strcmp(name, "button-update") || !strcmp(name, "button-update-key")) {
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-actor")), "Example author");
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-reason")), "Visible update");
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-title")), "Updated search");
    if (!strcmp(name, "button-update-key"))
      gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-key")), "other");
    g_signal_emit_by_name(find(root, "form-record-search-update"), "clicked");
    if (!strcmp(name, "button-update-key")) {
      CHECK(!UmiFormRecordEditorGtk4Busy(root) &&
            UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
      goto cleanup;
    }
    CHECK(idle(root));
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(umi_structured_form_search_preset_load(writer, "form.search.small", 0, &f->schema, loaded));
    CHECK(loaded->record.revision == 2 && !strcmp(loaded->preset.title, "Updated search"));
  }
cleanup:
  if (root) {
    if (!idle(root)) {
      fprintf(stderr, "Worker still active; preserve borrowed fixture until exit.\n");
      return 1;
    }
    if (callback)
      g_signal_handler_disconnect(observed, callback);
    UmiFormRecordEditorGtk4Retire(root);
    g_object_unref(root);
  }
  if (observed)
    g_object_unref(observed);
  if (retained)
    g_object_unref(retained);
  umi_data_server_destroy(writer);
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
  free(preset);
  free(loaded);
  free(state);
  free(f);
  return result;
}
