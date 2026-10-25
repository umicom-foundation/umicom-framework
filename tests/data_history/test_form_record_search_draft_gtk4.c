/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_form_record_search_draft_gtk4.c
 *
 * PURPOSE:
 *   Managed query editing must preserve independent record and scratch state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Managed query editing must preserve independent record and scratch state. */
#include "managed_search_fixture.h"
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
  const char *cases[] = {"empty",
                         "new",
                         "open",
                         "save-new",
                         "save-amend",
                         "later-edit",
                         "collision",
                         "stale",
                         "dirty-open",
                         "invalid-native",
                         "invalid-title",
                         "invalid-key",
                         "zero-time",
                         "dirty-record",
                         "busy",
                         "late-edit",
                         "retire-open",
                         "dispose-open",
                         "retire-save",
                         "dispose-save",
                         "discard",
                         "button-new",
                         "button-edit",
                         "button-save",
                         "retained-button",
                         "close-message",
                         "scratch-isolation",
                         "null",
                         "not-connected",
                         "missing",
                         "scope",
                         "schema"};
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
  GtkWidget *retained = NULL, *extra = NULL;
  OK(managed_new(argv[2], &f));
  UmiStructuredFormSchema alternate = f->form->schema;
  if (!strcmp(name, "empty")) {
    OK(UmiFormRecordEditorGtk4SearchDraftState(f->root, f->view));
    CHECK(!f->view->record.ready && UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }
  if (!strcmp(name, "not-connected")) {
    UmiFormRecordEditorGtk4Options options = {.schema = &f->form->schema,
                                              .entity_prefix = "managed.form.",
                                              .source = "Synthetic disconnected query"};
    OK(UmiFormRecordEditorGtk4New(&options, &extra));
    g_object_ref_sink(extra);
    CHECK(UmiFormRecordEditorGtk4BeginSearchDraft(extra, "new", "Query") ==
          UMI_STATUS_INVALID_STATE);
    CHECK(UmiFormRecordEditorGtk4EditSearchDraft(extra, "small") == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (!strcmp(name, "null")) {
    CHECK(UmiFormRecordEditorGtk4BeginSearchDraft(NULL, "new", "Query") ==
          UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordEditorGtk4SearchDraftState(f->root, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(UmiFormRecordEditorGtk4SaveSearchDraft(f->root, NULL) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, NULL) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "invalid-key") || !strcmp(name, "invalid-title")) {
    CHECK(UmiFormRecordEditorGtk4BeginSearchDraft(
              f->root, !strcmp(name, "invalid-key") ? "../escape" : "new",
              !strcmp(name, "invalid-title") ? "" : "Query") != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }
  if (!strcmp(name, "missing")) {
    OK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, "missing"));
    CHECK(managed_wait(f) == UMI_STATUS_NOT_FOUND);
    CHECK(UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }
  /* The former fixture changed a schema already borrowed by GTK. A separate
   * schema value now creates alternate encodings without violating that lifetime
   * contract. Retain the prior fixture for review of the corrected test setup. */
#if 0
  if (!strcmp(name, "scope") || !strcmp(name, "schema")) {
    if (!strcmp(name, "scope"))
      strcpy(f->preset->entity_prefix, "other.form.");
    else
      f->form->schema.version = 2;
    OK(umi_structured_form_search_preset_save(f->writer, &f->metadata, &f->form->schema, f->preset,
                                              1, &f->saved));
    f->form->schema.version = 1;
    OK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, "small"));
    CHECK(managed_wait(f) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }

#endif
  if (!strcmp(name, "scope") || !strcmp(name, "schema")) {
    if (!strcmp(name, "scope"))
      strcpy(f->preset->entity_prefix, "other.form.");
    else
      alternate.version = 2;
    /* A different schema must start a separate saved-query identity. The
     * storage service deliberately rejects cross-schema amendment of a head. */
#if 0
    OK(umi_structured_form_search_preset_save(f->writer, &f->metadata, &alternate, f->preset, 1,
                                              &f->saved));
    OK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, "small"));
#endif
    UmiRecordAmendment alternate_metadata = f->metadata;
    strcpy(alternate_metadata.entity_id, "form.search.alternate");
    OK(umi_structured_form_search_preset_save(f->writer, &alternate_metadata, &alternate,
                                              f->preset, 0, &f->saved));
    OK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, "alternate"));
    CHECK(managed_wait(f) != UMI_STATUS_OK);
    CHECK(UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }
  bool is_new = !strcmp(name, "new") || !strcmp(name, "save-new") || !strcmp(name, "button-new") ||
                !strcmp(name, "collision");
  if (is_new) {
    if (!strcmp(name, "button-new")) {
      g_signal_emit_by_name(managed_find(f->root, "form-record-search-draft-new"), "clicked");
      OK(UmiFormRecordEditorGtk4LastStatus(f->root));
    } else
      OK(UmiFormRecordEditorGtk4BeginSearchDraft(
          f->root, !strcmp(name, "collision") ? "small" : "new", "Draft query"));
  } else if (!strcmp(name, "button-edit")) {
    gtk_editable_set_text(GTK_EDITABLE(managed_find(f->root, "form-record-search-key")), "small");
    g_signal_emit_by_name(managed_find(f->root, "form-record-search-draft-edit"), "clicked");
    OK(managed_wait(f));
  } else
    OK(managed_open(f));
  if (!strcmp(name, "open") || !strcmp(name, "button-edit")) {
    OK(UmiFormRecordEditorGtk4SearchDraftState(f->root, f->view));
    CHECK(!strcmp(f->view->preset.title, "Stored query") &&
          UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }
  if (!strcmp(name, "new") || !strcmp(name, "button-new")) {
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
    OK(UmiFormRecordEditorGtk4SearchDraftState(f->root, f->view));
    CHECK(f->view->record.is_new);
    goto cleanup;
  }
  if (!strcmp(name, "busy") || !strcmp(name, "late-edit") || !strcmp(name, "retire-open") ||
      !strcmp(name, "dispose-open")) {
    OK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, "small"));
    if (!strcmp(name, "busy"))
      CHECK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, "small") == UMI_STATUS_BUSY);
    if (!strcmp(name, "late-edit"))
      gtk_editable_set_text(GTK_EDITABLE(managed_title(f)), "Later read edit");
    if (!strcmp(name, "retire-open"))
      UmiFormRecordEditorGtk4Retire(f->root);
    if (!strcmp(name, "dispose-open"))
      g_object_run_dispose(G_OBJECT(f->root));
    UmiStatus status = managed_wait(f);
    if (!strcmp(name, "busy"))
      CHECK(status == UMI_STATUS_OK);
    else
      CHECK(status != UMI_STATUS_OK);
    if (!strcmp(name, "late-edit"))
      CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_title(f))), "Later read edit"));
    goto cleanup;
  }
  gtk_editable_set_text(GTK_EDITABLE(managed_title(f)),
                        !strcmp(name, "invalid-native") ? "" : "Edited query");
  if (!strcmp(name, "invalid-native")) {
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
    CHECK(UmiFormRecordEditorGtk4SaveSearchDraft(f->root, &f->attribution) != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4Busy(f->root));
    goto cleanup;
  }
  if (!strcmp(name, "dirty-open")) {
    CHECK(UmiFormRecordEditorGtk4EditSearchDraft(f->root, "small") == UMI_STATUS_INVALID_STATE);
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_title(f))), "Edited query"));
    goto cleanup;
  }
  if (!strcmp(name, "close-message")) {
    CHECK(!UmiFormRecordEditorGtk4RequestClose(f->root));
    CHECK(strstr(gtk_label_get_text(GTK_LABEL(managed_find(f->root, "form-record-feedback"))),
                 "query"));
    goto cleanup;
  }
  if (!strcmp(name, "discard")) {
    OK(UmiFormRecordEditorGtk4DiscardSearchDraft(f->root));
    CHECK(UmiFormRecordEditorGtk4CanClose(f->root));
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_title(f))), "Stored query"));
    goto cleanup;
  }
  if (!strcmp(name, "scratch-isolation")) {
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_find(f->root, "form-record-search"))),
                  "scratch"));
    CHECK(!strcmp(
        gtk_editable_get_text(GTK_EDITABLE(managed_find(f->root, "form-record-search-title"))),
        "Draft query"));
    goto cleanup;
  }
  if (!strcmp(name, "retained-button")) {
    retained = g_object_ref(managed_find(f->root, "form-record-search-draft-save"));
    UmiFormRecordEditorGtk4Retire(f->root);
    g_signal_emit_by_name(retained, "clicked");
    CHECK(!UmiFormRecordEditorGtk4Busy(f->root));
    goto cleanup;
  }
  if (!strcmp(name, "dirty-record")) {
    OK(UmiFormRecordEditorGtk4Begin(f->root, "record"));
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
  }
  if (!strcmp(name, "stale")) {
    strcpy(f->preset->title, "Another writer");
    OK(umi_structured_form_search_preset_save(f->writer, &f->metadata, &f->form->schema, f->preset,
                                              1, &f->saved));
  }
  if (!strcmp(name, "zero-time")) {
    f->attribution.recorded_at_nanoseconds = 0;
    CHECK(UmiFormRecordEditorGtk4SaveSearchDraft(f->root, &f->attribution) != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4Busy(f->root));
    goto cleanup;
  }
  if (!strcmp(name, "button-save"))
    g_signal_emit_by_name(managed_find(f->root, "form-record-search-draft-save"), "clicked");
  else
    OK(UmiFormRecordEditorGtk4SaveSearchDraft(f->root, &f->attribution));
  if (!strcmp(name, "later-edit"))
    gtk_editable_set_text(GTK_EDITABLE(managed_title(f)), "Later save edit");
  if (!strcmp(name, "retire-save"))
    UmiFormRecordEditorGtk4Retire(f->root);
  if (!strcmp(name, "dispose-save"))
    g_object_run_dispose(G_OBJECT(f->root));
  UmiStatus saved = managed_wait(f);
  if (!strcmp(name, "stale") || !strcmp(name, "collision")) {
    CHECK(saved != UMI_STATUS_OK);
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
    goto cleanup;
  }
  CHECK(saved == UMI_STATUS_OK);
  OK(UmiFormRecordEditorGtk4SearchDraftState(f->root, f->view));
  CHECK(!f->view->record.saving && f->view->record.expected_revision == (is_new ? 1 : 2));
  if (!strcmp(name, "later-edit")) {
    CHECK(!UmiFormRecordEditorGtk4CanClose(f->root));
    CHECK(!strcmp(gtk_editable_get_text(GTK_EDITABLE(managed_title(f))), "Later save edit"));
  } else if (!strcmp(name, "dirty-record")) {
    UmiStructuredFormSessionView *state = malloc(sizeof *state);
    CHECK(state);
    UmiStatus status = UmiFormRecordEditorGtk4State(f->root, state);
    bool dirty = status == UMI_STATUS_OK && state->record.is_new && state->record.dirty;
    free(state);
    CHECK(status == UMI_STATUS_OK && dirty && !UmiFormRecordEditorGtk4CanClose(f->root));
  } else if (strcmp(name, "retire-save") && strcmp(name, "dispose-save"))
    CHECK(UmiFormRecordEditorGtk4CanClose(f->root));
cleanup:
  if (retained)
    g_object_unref(retained);
  if (extra) {
    UmiFormRecordEditorGtk4Retire(extra);
    g_object_unref(extra);
  }
  if (!managed_free(f))
    return 1;
  return result;
}
