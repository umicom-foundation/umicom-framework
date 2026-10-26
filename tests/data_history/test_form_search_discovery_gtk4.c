/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_form_search_discovery_gtk4.c
 *
 * PURPOSE:
 *   Native discovery preserves query edits and opens only reviewed definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Native discovery preserves query edits and opens only reviewed definitions. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_search_preset.h"
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
  gint64 end = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  while (UmiFormRecordEditorGtk4Busy(root) && g_get_monotonic_time() < end) {
    g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return !UmiFormRecordEditorGtk4Busy(root);
}
typedef struct Observer {
  GtkWidget *root;
  const char *name;
  bool fired;
} Observer;
static void observe(GObject *object, GParamSpec *spec, gpointer context) {
  (void)object;
  (void)spec;
  Observer *o = context;
  if (o->fired)
    return;
  o->fired = true;
  if (!strcmp(o->name, "completion-retire"))
    UmiFormRecordEditorGtk4Retire(o->root);
  else if (!strcmp(o->name, "completion-model"))
    gtk_drop_down_set_model(GTK_DROP_DOWN(find(o->root, "form-record-search-results")), NULL);
  else if (!strcmp(o->name, "completion-label")) {
    const char *labels[] = {"Changed label", NULL};
    gtk_string_list_splice(GTK_STRING_LIST(gtk_drop_down_get_model(
                               GTK_DROP_DOWN(find(o->root, "form-record-search-results")))),
                           0, 1, labels);
  } else if (!strcmp(o->name, "open-observer-selection"))
    gtk_drop_down_set_selected(GTK_DROP_DOWN(find(o->root, "form-record-search-results")), 1);
  else
    gtk_editable_set_text(GTK_EDITABLE(find(o->root, "form-record-search-find-text")),
                          "Later filter");
}
int main(int argc, char **argv) {
  const char *cases[] = {"basic",
                         "empty",
                         "pages",
                         "title",
                         "key",
                         "open",
                         "dirty-record",
                         "stale-open",
                         "changed-page",
                         "unknown-version",
                         "known-version",
                         "malformed",
                         "utf8-title",
                         "utf8-later-title",
                         "late-query",
                         "retire-read",
                         "dispose-read",
                         "late-open-filter",
                         "late-open-query",
                         "late-open-selection",
                         "late-open-model",
                         "model-mutation",
                         "model-label",
                         "completion-label",
                         "model-replacement",
                         "completion-query",
                         "completion-model",
                         "completion-retire",
                         "open-observer-selection",
                         "no-page",
                         "no-next",
                         "changed-filter",
                         "invalid-arguments",
                         "long-query",
                         "null",
                         "busy",
                         "retained-button",
                         "buttons",
                         "not-connected"};
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
  FormFixture *f = form_fixture_new(), *g = form_fixture_new();
  UmiDataServer *writer = NULL;
  UmiStructuredFormCatalogue *catalogue = NULL;
  GtkWidget *root = NULL, *observed = NULL, *retained = NULL;
  gulong callback = 0;
  Observer observer = {.name = name};
  char *directory = NULL, *database = NULL;
  UmiStructuredFormSearchPreset *preset = calloc(1, sizeof *preset);
  UmiStructuredFormSearchPage *page = malloc(sizeof *page), *before = malloc(sizeof *before);
  CHECK(f && g && preset && page && before);
  g->schema.version = 2;
  const UmiStructuredFormSchema *schemas[] = {&f->schema, &g->schema};
  OK(umi_structured_form_catalogue_create(schemas, 2, &catalogue));
  CHECK(g_mkdir_with_parents(argv[2], 0700) == 0);
  directory = g_build_filename(argv[2], "native-discovery-XXXXXX", NULL);
  CHECK(g_mkdtemp(directory));
  database = g_build_filename(directory, "searches.sqlite3", NULL);
  OK(umi_data_server_create_sqlite(database, &writer));
  strcpy(preset->entity_prefix, "parcel.record.");
  preset->filter.count = 1;
  strcpy(preset->filter.terms[0].field_id, "parcel.mass");
  preset->filter.terms[0].operation = UMI_FORM_FILTER_LESS;
  preset->filter.terms[0].operand = form_mass(10);
  UmiRecordAmendment metadata = {.source = "Fictional discovery",
                                 .actor = "Example author",
                                 .reason = "Store search",
                                 .recorded_at_nanoseconds = 1};
  UmiRecordRevision saved;
  size_t count = !strcmp(name, "empty") ? 0 : 20;
  for (size_t i = 1; i <= count; ++i) {
    (void)snprintf(metadata.entity_id, sizeof metadata.entity_id, "form.search.%03zu", i);
    strcpy(preset->title, i % 2 ? "Bulk parcels" : "Small parcels");
    /* Native UTF-8 validation applies to rows about to be displayed. Keep the
     * earlier off-page setup for review; test visible and later-page refusal
     * separately without broadening the byte-preserving storage contract. */
#if 0
    if (!strcmp(name, "utf8-title") && i == 20)
      strcpy(preset->title, "\xff");
#endif
    if ((!strcmp(name, "utf8-title") && i == 1) ||
        (!strcmp(name, "utf8-later-title") && i == 20))
      strcpy(preset->title, "ÿ");
    if (!strcmp(name, "malformed") && i == 20)
      OK(umi_record_document_append(writer, &metadata, "bad", 3, 0, &saved));
    else
      OK(umi_structured_form_search_preset_save(
          writer, &metadata,
          (i == 20 && (!strcmp(name, "known-version") || !strcmp(name, "unknown-version")))
              ? &g->schema
              : &f->schema,
          preset, 0, &saved));
  }
  UmiFormRecordEditorGtk4Options options = {.schema = &f->schema,
                                            .entity_prefix = "parcel.record.",
                                            .source = "Fictional discovery host"};
  OK(UmiFormRecordEditorGtk4New(&options, &root));
  g_object_ref_sink(root);
  observer.root = root;
  if (!strcmp(name, "known-version"))
    OK(UmiFormRecordEditorGtk4SetHistoryCatalogue(root, catalogue));
  memset(before, 0x66, sizeof *before);
  *page = *before;
  if (!strcmp(name, "not-connected")) {
    CHECK(UmiFormRecordEditorGtk4FindSavedSearches(root, "", 0) != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4Busy(root));
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4Connect(root, database));
  CHECK(idle(root));
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  if (!strcmp(name, "null")) {
    CHECK(UmiFormRecordEditorGtk4FindSavedSearches(NULL, "", 0) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4FindSavedSearches(root, NULL, 0) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4SavedSearchPage(root, NULL) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4OpenSavedSearchResult(NULL, 0) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "no-page")) {
    CHECK(UmiFormRecordEditorGtk4SavedSearchPage(root, page) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiFormRecordEditorGtk4OpenSavedSearchResult(root, 0) != UMI_STATUS_OK);
    CHECK(!memcmp(page, before, sizeof *page));
    goto cleanup;
  }
  if (!strcmp(name, "invalid-arguments") || !strcmp(name, "long-query")) {
    char long_text[161];
    memset(long_text, 'x', 160);
    long_text[160] = 0;
    CHECK(UmiFormRecordEditorGtk4FindSavedSearches(
              root, !strcmp(name, "long-query") ? long_text : "",
              !strcmp(name, "long-query") ? 0 : 2) != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4Busy(root));
    goto cleanup;
  }
  if (!strcmp(name, "retained-button")) {
    retained = g_object_ref(find(root, "form-record-search-find"));
    g_object_unref(root);
    root = NULL;
    g_signal_emit_by_name(retained, "clicked");
    goto cleanup;
  }
  if (!strncmp(name, "completion-", 11)) {
    observed = g_object_ref(find(root, "form-record-search-results"));
    callback = g_signal_connect(observed, "notify::model", G_CALLBACK(observe), &observer);
  }
  const char *text = !strcmp(name, "title") || !strcmp(name, "no-next") ? "Small"
                     : !strcmp(name, "key")                             ? "003"
                                                                        : "";
  if (!strcmp(name, "buttons")) {
    g_signal_emit_by_name(find(root, "form-record-search-find"), "clicked");
    OK(UmiFormRecordEditorGtk4LastStatus(root));
  } else
    OK(UmiFormRecordEditorGtk4FindSavedSearches(root, text, 0));
  if (!strcmp(name, "busy")) {
    CHECK(UmiFormRecordEditorGtk4FindSavedSearches(root, "", 0) == UMI_STATUS_BUSY);
    CHECK(UmiFormRecordEditorGtk4OpenSavedSearchResult(root, 0) == UMI_STATUS_BUSY);
    CHECK(UmiFormRecordEditorGtk4SavedSearchPage(root, page) == UMI_STATUS_BUSY);
    CHECK(!memcmp(page, before, sizeof *page));
  }
  if (!strcmp(name, "late-query"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-find-text")), "Later");
  if (!strcmp(name, "retire-read"))
    UmiFormRecordEditorGtk4Retire(root);
  if (!strcmp(name, "dispose-read"))
    g_object_run_dispose(G_OBJECT(root));
  CHECK(idle(root));
  if (!strcmp(name, "utf8-later-title")) {
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(UmiFormRecordEditorGtk4SavedSearchPage(root, page));
    CHECK(page->count == 16U && page->next_after_entity[0] != ' ');
    memcpy(before, page, sizeof *before);
    OK(UmiFormRecordEditorGtk4FindSavedSearches(root, "", 1));
    CHECK(idle(root));
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_PARSE_ERROR);
    /* Refused presentation must retain the previously reviewed page and its
     * model binding; no partial row list may replace it. */
    OK(UmiFormRecordEditorGtk4SavedSearchPage(root, page));
    CHECK(memcmp(page, before, sizeof *page) == 0);
    goto cleanup;
  }
  bool failed = !strcmp(name, "unknown-version") || !strcmp(name, "malformed") ||
                !strcmp(name, "utf8-title") || !strcmp(name, "late-query") ||
                !strcmp(name, "retire-read") || !strcmp(name, "dispose-read") ||
                !strncmp(name, "completion-", 11);
  if (failed) {
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4SavedSearchPage(root, page) != UMI_STATUS_OK &&
          !memcmp(page, before, sizeof *page));
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4LastStatus(root));
  OK(UmiFormRecordEditorGtk4SavedSearchPage(root, page));
  size_t expected = !strcmp(name, "empty")           ? 0
                    : !strcmp(name, "known-version") ? 19
                    : !strcmp(name, "key")           ? 1
                    : text[0]                        ? 10
                                                     : 20;
  CHECK(page->matching_count == expected && page->count == (expected < 16 ? expected : 16));
  if (!strcmp(name, "empty")) {
    CHECK(UmiFormRecordEditorGtk4OpenSavedSearchResult(root, 0) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "pages") || !strcmp(name, "changed-page")) {
    if (!strcmp(name, "changed-page")) {
      strcpy(metadata.entity_id, "form.search.001");
      strcpy(preset->title, "Changed");
      metadata.recorded_at_nanoseconds = 2;
      OK(umi_structured_form_search_preset_save(writer, &metadata, &f->schema, preset, 1, &saved));
    }
    OK(UmiFormRecordEditorGtk4FindSavedSearches(root, "", 1));
    CHECK(idle(root));
    if (!strcmp(name, "changed-page"))
      CHECK(UmiFormRecordEditorGtk4LastStatus(root) == UMI_STATUS_INVALID_STATE);
    else {
      OK(UmiFormRecordEditorGtk4LastStatus(root));
      OK(UmiFormRecordEditorGtk4SavedSearchPage(root, page));
      CHECK(page->count == 4 && !page->next_after_entity[0]);
    }
    goto cleanup;
  }
  if (!strcmp(name, "no-next")) {
    CHECK(UmiFormRecordEditorGtk4FindSavedSearches(root, text, 1) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (!strcmp(name, "changed-filter")) {
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-find-text")), "Small");
    CHECK(UmiFormRecordEditorGtk4FindSavedSearches(root, "Small", 1) == UMI_STATUS_INVALID_STATE);
    CHECK(UmiFormRecordEditorGtk4OpenSavedSearchResult(root, 0) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  GtkWidget *results = find(root, "form-record-search-results");
  if (!strcmp(name, "model-mutation") || !strcmp(name, "model-label") ||
      !strcmp(name, "model-replacement")) {
    if (!strcmp(name, "model-label")) {
      const char *labels[] = {"Changed label", NULL};
      gtk_string_list_splice(GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(results))), 0, 1,
                             labels);
    } else if (!strcmp(name, "model-mutation"))
      gtk_string_list_append(GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(results))),
                             "Unreviewed row");
    else
      gtk_drop_down_set_model(GTK_DROP_DOWN(results), NULL);
    CHECK(UmiFormRecordEditorGtk4OpenSavedSearchResult(root, 0) == UMI_STATUS_INVALID_STATE);
    *page = *before;
    CHECK(UmiFormRecordEditorGtk4SavedSearchPage(root, page) == UMI_STATUS_INVALID_STATE &&
          !memcmp(page, before, sizeof *page));
    goto cleanup;
  }
  bool opening = !strcmp(name, "open") || !strcmp(name, "dirty-record") ||
                 !strcmp(name, "stale-open") || !strncmp(name, "late-open-", 10) ||
                 !strcmp(name, "open-observer-selection") || !strcmp(name, "buttons");
  if (!opening)
    goto cleanup;
  if (!strcmp(name, "dirty-record")) {
    OK(UmiFormRecordEditorGtk4Begin(root, "draft"));
    gtk_editable_set_text(GTK_EDITABLE(find(root, "parcel.mass.value")), "12");
  }
  if (!strcmp(name, "stale-open")) {
    strcpy(metadata.entity_id, "form.search.001");
    strcpy(preset->title, "Changed");
    metadata.recorded_at_nanoseconds = 2;
    OK(umi_structured_form_search_preset_save(writer, &metadata, &f->schema, preset, 1, &saved));
  }
  if (!strcmp(name, "open-observer-selection")) {
    observed = g_object_ref(find(root, "form-record-search-key"));
    callback = g_signal_connect(observed, "notify::text", G_CALLBACK(observe), &observer);
  }
  UmiStatus opened;
  if (!strcmp(name, "buttons")) {
    gtk_drop_down_set_selected(GTK_DROP_DOWN(results), 0);
    g_signal_emit_by_name(find(root, "form-record-search-open-result"), "clicked");
    opened = UmiFormRecordEditorGtk4LastStatus(root);
  } else
    opened = UmiFormRecordEditorGtk4OpenSavedSearchResult(root, 0);
  if (!strcmp(name, "open-observer-selection")) {
    CHECK(opened == UMI_STATUS_CANCELLED && !UmiFormRecordEditorGtk4Busy(root));
    goto cleanup;
  }
  OK(opened);
  if (!strcmp(name, "late-open-filter"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search-find-text")), "Later");
  if (!strcmp(name, "late-open-query"))
    gtk_editable_set_text(GTK_EDITABLE(find(root, "form-record-search")), "Later query");
  if (!strcmp(name, "late-open-selection"))
    gtk_drop_down_set_selected(GTK_DROP_DOWN(results), 1);
  if (!strcmp(name, "late-open-model"))
    gtk_string_list_append(GTK_STRING_LIST(gtk_drop_down_get_model(GTK_DROP_DOWN(results))),
                           "Late row");
  CHECK(idle(root));
  if (!strcmp(name, "stale-open") || !strncmp(name, "late-open-", 10)) {
    CHECK(UmiFormRecordEditorGtk4LastStatus(root) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4SavedSearch(root, &saved) == UMI_STATUS_NOT_FOUND);
  } else {
    OK(UmiFormRecordEditorGtk4LastStatus(root));
    OK(UmiFormRecordEditorGtk4SavedSearch(root, &saved));
    CHECK(!strcmp(saved.amendment.entity_id, "form.search.001"));
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "form-record-search-title"))),
                  "Bulk parcels"));
  }
  if (!strcmp(name, "dirty-record"))
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(find(root, "parcel.mass.value"))), "12") &&
          !UmiFormRecordEditorGtk4CanClose(root));
cleanup:
  if (root) {
    if (!idle(root)) {
      fprintf(stderr, "Worker remains active; preserve borrowed fixtures until exit.\n");
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
  g_free(directory);
  g_free(database);
  free(f);
  free(g);
  free(preset);
  free(page);
  free(before);
  return result;
}
