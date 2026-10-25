/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_form_record_search_recovery_gtk4.c
 *
 * PURPOSE:
 *   Query recovery is explicit private-file I/O, independent of answer recovery.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Query recovery is explicit private-file I/O, independent of answer recovery. */
#include "managed_search_fixture.h"
#include "umicom/data/record_draft_files.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv) {
  const char *cases[] = {"keep",          "keeps-dirty",    "existing-file",   "recover",
                         "new-recovery",  "dirty-target",   "late-edit",       "retire-write",
                         "dispose-write", "retire-read",    "dispose-read",    "relative",
                         "null",          "empty",          "invalid-native",  "damaged-file",
                         "wrong-scope",   "wrong-schema",   "wrong-baseline",  "stale-save",
                         "button-keep",   "button-recover", "record-isolation"};
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
  ManagedSearchFixture *f = NULL;
  UmiRecordDraftSnapshot *snapshot = NULL;
  OK(managed_new(argv[2], &f));
  UmiStructuredFormSchema alternate = f->form->schema;
  if (!strcmp(name, "empty")) {
    CHECK(UmiFormRecordEditorGtk4KeepSearchRecovery(f->root, f->recovery) != UMI_STATUS_OK);
    CHECK(!g_file_test(f->recovery, G_FILE_TEST_EXISTS));
    goto cleanup;
  }
  if (!strcmp(name, "null")) {
    CHECK(UmiFormRecordEditorGtk4KeepSearchRecovery(f->root, NULL) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4RecoverSearch(f->root, NULL) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4RecoverSearch(NULL, f->recovery) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "relative")) {
    CHECK(UmiFormRecordEditorGtk4KeepSearchRecovery(f->root, "query.umidraft") != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4RecoverSearch(f->root, "query.umidraft") != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "new-recovery"))
    OK(UmiFormRecordEditorGtk4BeginSearchDraft(f->root, "new", "Unsaved new query"));
  else
    OK(managed_open(f));
  gtk_editable_set_text(GTK_EDITABLE(managed_title(f)),
                        !strcmp(name, "invalid-native") ? "" : "Recovered edit");
  if (!strcmp(name, "invalid-native")) {
    CHECK(UmiFormRecordEditorGtk4KeepSearchRecovery(f->root, f->recovery) != UMI_STATUS_OK);
    CHECK(!g_file_test(f->recovery, G_FILE_TEST_EXISTS));
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }
  /* The former fixture changed a schema already borrowed by GTK. A separate
   * schema value now creates alternate encodings without violating that lifetime
   * contract. Retain the prior fixture for review of the corrected test setup. */
#if 0
  if (!strcmp(name, "wrong-scope") || !strcmp(name, "wrong-schema") ||
      !strcmp(name, "wrong-baseline")) {
    snapshot = malloc(sizeof *snapshot);
    CHECK(snapshot);
    OK(UmiStructuredFormSearchDraftGtk4Capture(managed_find(f->root, "form-record-search-draft"),
                                               snapshot));
    if (!strcmp(name, "wrong-baseline"))
      strcpy(snapshot->baseline.record.amendment.actor, "Different stored author");
    else {
      OK(umi_structured_form_search_preset_decode(&f->form->schema, snapshot->content,
                                                  snapshot->length, f->preset));
      if (!strcmp(name, "wrong-scope"))
        strcpy(f->preset->entity_prefix, "other.form.");
      else
        f->form->schema.version = 2;
      OK(umi_structured_form_search_preset_encode(&f->form->schema, f->preset, snapshot->content,
                                                  sizeof snapshot->content, &snapshot->length));
      f->form->schema.version = 1;
    }
    OK(umi_record_draft_snapshot_write_new(f->recovery, snapshot));
  }

#endif
  if (!strcmp(name, "wrong-scope") || !strcmp(name, "wrong-schema") ||
      !strcmp(name, "wrong-baseline")) {
    snapshot = malloc(sizeof *snapshot);
    CHECK(snapshot);
    OK(UmiStructuredFormSearchDraftGtk4Capture(managed_find(f->root, "form-record-search-draft"),
                                               snapshot));
    if (!strcmp(name, "wrong-baseline"))
      strcpy(snapshot->baseline.record.amendment.actor, "Different stored author");
    else {
      OK(umi_structured_form_search_preset_decode(&f->form->schema, snapshot->content,
                                                  snapshot->length, f->preset));
      if (!strcmp(name, "wrong-scope"))
        strcpy(f->preset->entity_prefix, "other.form.");
      else
        alternate.version = 2;
      OK(umi_structured_form_search_preset_encode(&alternate, f->preset, snapshot->content,
                                                  sizeof snapshot->content, &snapshot->length));
    }
    OK(umi_record_draft_snapshot_write_new(f->recovery, snapshot));
  } else {
    if (!strcmp(name, "button-keep"))
      g_signal_emit_by_name(managed_find(f->root, "form-record-search-recovery-keep"), "clicked");
    else
      OK(UmiFormRecordEditorGtk4KeepSearchRecovery(f->root, f->recovery));
    if (!strcmp(name, "retire-write"))
      UmiFormRecordEditorGtk4Retire(f->root);
    if (!strcmp(name, "dispose-write"))
      g_object_run_dispose(G_OBJECT(f->root));
    OK(managed_wait(f));
    CHECK(g_file_test(f->recovery, G_FILE_TEST_IS_REGULAR));
    if (!strcmp(name, "retire-write") || !strcmp(name, "dispose-write"))
      goto cleanup;
  }
  if (!strcmp(name, "keep") || !strcmp(name, "keeps-dirty") || !strcmp(name, "button-keep")) {
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
    OK(UmiFormRecordEditorGtk4SearchDraftState(f->root, f->view));
    CHECK(f->view->record.dirty && f->view->record.expected_revision == 1);
    goto cleanup;
  }
  if (!strcmp(name, "existing-file")) {
    gchar *before = NULL, *after = NULL;
    gsize na = 0, nb = 0;
    CHECK(g_file_get_contents(f->recovery, &before, &na, NULL));
    OK(UmiFormRecordEditorGtk4KeepSearchRecovery(f->root, f->recovery));
    UmiStatus status = managed_wait(f);
    bool read = g_file_get_contents(f->recovery, &after, &nb, NULL);
    bool same = read && na == nb && !memcmp(before, after, na);
    g_free(before);
    g_free(after);
    CHECK(status != UMI_STATUS_OK && same);
    goto cleanup;
  }
  if (!strcmp(name, "dirty-target")) {
    CHECK(UmiFormRecordEditorGtk4RecoverSearch(f->root, f->recovery) == UMI_STATUS_INVALID_STATE);
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_title(f))), "Recovered edit"));
    goto cleanup;
  }
  OK(UmiFormRecordEditorGtk4DiscardSearchDraft(f->root));
  if (!strcmp(name, "damaged-file"))
    CHECK(g_file_set_contents(f->recovery, "broken", 6, NULL));
  if (!strcmp(name, "stale-save")) {
    strcpy(f->preset->title, "New stored head");
    OK(umi_structured_form_search_preset_save(f->writer, &f->metadata, &f->form->schema, f->preset,
                                              1, &f->saved));
  }
  if (!strcmp(name, "record-isolation"))
    OK(UmiFormRecordEditorGtk4Begin(f->root, "record"));
  if (!strcmp(name, "button-recover"))
    g_signal_emit_by_name(managed_find(f->root, "form-record-search-recovery-open"), "clicked");
  else
    OK(UmiFormRecordEditorGtk4RecoverSearch(f->root, f->recovery));
  if (!strcmp(name, "late-edit"))
    gtk_editable_set_text(GTK_EDITABLE(managed_title(f)), "Keep later edit");
  if (!strcmp(name, "retire-read"))
    UmiFormRecordEditorGtk4Retire(f->root);
  if (!strcmp(name, "dispose-read"))
    g_object_run_dispose(G_OBJECT(f->root));
  UmiStatus restored = managed_wait(f);
  if (!strcmp(name, "late-edit") || !strcmp(name, "retire-read") || !strcmp(name, "dispose-read") ||
      !strcmp(name, "damaged-file") || !strcmp(name, "wrong-scope") ||
      !strcmp(name, "wrong-schema") || !strcmp(name, "wrong-baseline")) {
    CHECK(restored != UMI_STATUS_OK);
    if (!strcmp(name, "late-edit"))
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_title(f))), "Keep later edit"));
    goto cleanup;
  }
  CHECK(restored == UMI_STATUS_OK);
  CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_title(f))), "Recovered edit"));
  CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
  OK(UmiFormRecordEditorGtk4SearchDraftState(f->root, f->view));
  CHECK(f->view->record.dirty &&
        f->view->record.expected_revision == (!strcmp(name, "new-recovery") ? 0 : 1));
  if (!strcmp(name, "stale-save")) {
    OK(UmiFormRecordEditorGtk4SaveSearchDraft(f->root, &f->attribution));
    CHECK(managed_wait(f) != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
  }
  if (!strcmp(name, "record-isolation")) {
    UmiStructuredFormSessionView *state = malloc(sizeof *state);
    CHECK(state);
    UmiStatus status = UmiFormRecordEditorGtk4State(f->root, state);
    bool dirty = status == UMI_STATUS_OK && state->record.is_new && state->record.dirty;
    free(state);
    CHECK(status == UMI_STATUS_OK && dirty);
  }
cleanup:
  free(snapshot);
  if (!managed_free(f))
    return 1;
  return result;
}
