/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Fictional query data exercises the public workbench through its real worker. */
#ifndef UMICOM_MANAGED_SEARCH_FIXTURE_H
#define UMICOM_MANAGED_SEARCH_FIXTURE_H
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_search_preset.h"
#include "umicom/ui/gtk4/form_record_editor.h"
#include "umicom/ui/gtk4/structured_form_search_draft.h"
#include <glib/gstdio.h>
typedef struct ManagedSearchFixture {
  FormFixture *form;
  UmiDataServer *writer;
  GtkWidget *root;
  char *directory, *database, *recovery;
  UmiStructuredFormSearchPreset *preset;
  UmiStructuredFormSearchPresetRecord *loaded;
  UmiStructuredFormSearchSessionView *view;
  UmiRecordDraftAttribution attribution;
  UmiRecordAmendment metadata;
  UmiRecordRevision saved;
} ManagedSearchFixture;
static inline GtkWidget *managed_find(GtkWidget *root, const char *name) {
  if (!root)
    return NULL;
  if (!strcmp(gtk_widget_get_name(root), name))
    return root;
  for (GtkWidget *child = gtk_widget_get_first_child(root); child;
       child = gtk_widget_get_next_sibling(child)) {
    GtkWidget *match = managed_find(child, name);
    if (match)
      return match;
  }
  return NULL;
}
static inline bool managed_idle(GtkWidget *root) {
  gint64 until = g_get_monotonic_time() + 15 * G_USEC_PER_SEC;
  while (UmiFormRecordEditorGtk4Busy(root) && g_get_monotonic_time() < until) {
    g_main_context_iteration(NULL, FALSE);
    g_usleep(1000);
  }
  return !UmiFormRecordEditorGtk4Busy(root);
}
static inline UmiStatus managed_wait(ManagedSearchFixture *f) {
  return managed_idle(f->root) ? UmiFormRecordEditorGtk4LastStatus(f->root) : UMI_STATUS_BUSY;
}
static inline GtkWidget *managed_title(ManagedSearchFixture *f) {
  return managed_find(managed_find(f->root, "form-record-search-draft"), "search-draft-title");
}
static inline UmiStatus managed_open(ManagedSearchFixture *f) {
  UmiStatus status = UmiFormRecordEditorGtk4EditSearchDraft(f->root, "small");
  return status == UMI_STATUS_OK ? managed_wait(f) : status;
}
static inline UmiStatus managed_new(const char *parent, ManagedSearchFixture **out) {
  ManagedSearchFixture *f = calloc(1, sizeof *f);
  if (!f)
    return UMI_STATUS_OUT_OF_MEMORY;
  *out = f;
  f->form = form_fixture_new();
  f->preset = calloc(1, sizeof *f->preset);
  f->loaded = malloc(sizeof *f->loaded);
  f->view = malloc(sizeof *f->view);
  if (!f->form || !f->preset || !f->loaded || !f->view)
    return UMI_STATUS_OUT_OF_MEMORY;
  if (g_mkdir_with_parents(parent, 0700))
    return UMI_STATUS_IO_ERROR;
  f->directory = g_build_filename(parent, "managed-query-XXXXXX", NULL);
  if (!g_mkdtemp(f->directory))
    return UMI_STATUS_IO_ERROR;
  f->database = g_build_filename(f->directory, "queries.sqlite3", NULL);
  f->recovery = g_build_filename(f->directory, "query.umidraft", NULL);
  UmiStatus status = umi_data_server_create_sqlite(f->database, &f->writer);
  strcpy(f->preset->title, "Stored query");
  strcpy(f->preset->entity_prefix, "managed.form.");
  strcpy(f->preset->metadata_text, "stored");
  f->preset->filter.count = 1;
  strcpy(f->preset->filter.terms[0].field_id, "parcel.mass");
  f->preset->filter.terms[0].operation = UMI_FORM_FILTER_LESS;
  f->preset->filter.terms[0].operand = form_mass(9);
  f->metadata = (UmiRecordAmendment){.entity_id = "form.search.small",
                                     .source = "Synthetic managed query",
                                     .actor = "Example author",
                                     .reason = "Store initial definition",
                                     .recorded_at_nanoseconds = 1};
  f->attribution = (UmiRecordDraftAttribution){.source = "Synthetic managed query",
                                               .actor = "Example author",
                                               .reason = "Edit the fictional definition",
                                               .recorded_at_nanoseconds = 2};
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_search_preset_save(f->writer, &f->metadata, &f->form->schema,
                                                    f->preset, 0, &f->saved);
  UmiFormRecordEditorGtk4Options options = {.schema = &f->form->schema,
                                            .entity_prefix = "managed.form.",
                                            .source = "Synthetic managed query"};
  if (status == UMI_STATUS_OK)
    status = UmiFormRecordEditorGtk4New(&options, &f->root);
  if (status == UMI_STATUS_OK) {
    g_object_ref_sink(f->root);
    status = UmiFormRecordEditorGtk4Connect(f->root, f->database);
  }
  if (status == UMI_STATUS_OK)
    status = managed_wait(f);
  if (status == UMI_STATUS_OK) {
    f->preset->filter.terms[0].operand = form_mass(5);
    status = UmiFormRecordEditorGtk4SetFilter(f->root, &f->preset->filter);
    gtk_editable_set_text(GTK_EDITABLE(managed_find(f->root, "form-record-search")), "scratch");
    gtk_editable_set_text(GTK_EDITABLE(managed_find(f->root, "form-record-search-key")), "new");
    gtk_editable_set_text(GTK_EDITABLE(managed_find(f->root, "form-record-search-title")),
                          "Draft query");
    gtk_editable_set_text(GTK_EDITABLE(managed_find(f->root, "form-record-actor")),
                          "Example author");
    gtk_editable_set_text(GTK_EDITABLE(managed_find(f->root, "form-record-reason")),
                          "Edit fictional search");
    gtk_editable_set_text(GTK_EDITABLE(managed_find(f->root, "form-record-search-recovery-path")),
                          f->recovery);
  }
  return status;
}
static inline bool managed_free(ManagedSearchFixture *f) {
  if (!f)
    return true;
  /* A timed-out worker may still borrow schema data. Keep it alive until exit
   * and fail the test rather than tearing storage out from underneath it. */
  if (f->root && !managed_idle(f->root))
    return false;
  if (f->root) {
    UmiFormRecordEditorGtk4Retire(f->root);
    g_object_unref(f->root);
  }
  umi_data_server_destroy(f->writer);
  if (f->recovery)
    (void)g_remove(f->recovery);
  if (f->database) {
    (void)g_remove(f->database);
    const char *suffixes[] = {"-wal", "-shm", "-journal"};
    for (size_t i = 0; i < 3; ++i) {
      char *side = g_strconcat(f->database, suffixes[i], NULL);
      (void)g_remove(side);
      g_free(side);
    }
  }
  if (f->directory)
    (void)g_rmdir(f->directory);
  g_free(f->recovery);
  g_free(f->database);
  g_free(f->directory);
  free(f->preset);
  free(f->loaded);
  free(f->view);
  free(f->form);
  free(f);
  return true;
}
#endif
