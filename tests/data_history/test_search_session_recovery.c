/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Recovery validates both baseline and draft against the chosen query scope. */
#include "search_session_fixture.h"
#undef CHECK
#include "umicom/base/sha256.h"
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
  const char *cases[] = {"new",
                         "saved",
                         "clean",
                         "codec",
                         "wrong-current-scope",
                         "wrong-baseline-scope",
                         "wrong-schema",
                         "wrong-entity",
                         "damaged-baseline",
                         "damaged-current",
                         "new-baseline-metadata",
                         "dirty-target",
                         "clean-target",
                         "wrong-store",
                         "later-head",
                         "capture-empty",
                         "null"};
  if (argc != 2 && argc != 3)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (!strcmp(argv[1], cases[i]))
      known = true;
  if (!known || (argc == 3 && strcmp(argv[2], "sqlite")))
    return 2;
  int result = 0;
  const char *name = argv[1];
  SearchFixture *f = NULL;
  UmiStructuredFormSearchSession *restored = NULL;
  UmiDataServer *other = NULL;
  UmiStructuredFormSearchSessionView *view = calloc(1, sizeof *view),
                                     *before = malloc(sizeof *before);
  unsigned char *bytes = NULL;
  size_t length = 0;
  CHECK(view && before);
  int mode = !strcmp(name, "capture-empty")                                   ? 0
             : !strcmp(name, "new") || !strcmp(name, "new-baseline-metadata") ? 1
                                                                              : 2;
  OK(search_fixture_new(mode, argc == 3, &f));
  OK(umi_structured_form_search_session_create(&f->form->schema, "parcel.record.", &restored));
  if (!strcmp(name, "capture-empty")) {
    memset(f->snapshot, 0x56, sizeof *f->snapshot);
    UmiRecordDraftSnapshot *original = malloc(sizeof *original);
    CHECK(original);
    *original = *f->snapshot;
    UmiStatus status = umi_structured_form_search_session_capture(f->session, f->snapshot);
    bool unchanged = !memcmp(original, f->snapshot, sizeof *original);
    free(original);
    CHECK(status == UMI_STATUS_INVALID_STATE && unchanged);
    goto cleanup;
  }
  if (strcmp(name, "clean"))
    OK(search_fixture_edit(f, "Recovered query"));
  OK(umi_structured_form_search_session_capture(f->session, f->snapshot));
  if (!strcmp(name, "null")) {
    CHECK(umi_structured_form_search_session_capture(NULL, f->snapshot) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_capture(f->session, NULL) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_restore(NULL, f->snapshot) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_restore(restored, NULL) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "codec")) {
    OK(umi_record_draft_snapshot_encode(f->snapshot, &bytes, &length));
    memset(f->snapshot, 0, sizeof *f->snapshot);
    OK(umi_record_draft_snapshot_decode(bytes, length, f->snapshot));
  }
  if (!strcmp(name, "wrong-current-scope") || !strcmp(name, "wrong-baseline-scope")) {
    strcpy(f->preset->entity_prefix, "other.record.");
    if (!strcmp(name, "wrong-current-scope"))
      OK(umi_structured_form_search_preset_encode(&f->form->schema, f->preset, f->snapshot->content,
                                                  sizeof f->snapshot->content,
                                                  &f->snapshot->length));
    else {
      OK(umi_structured_form_search_preset_encode(
          &f->form->schema, f->preset, f->snapshot->baseline.content,
          sizeof f->snapshot->baseline.content, &f->snapshot->baseline.length));
      OK(UmiSha256Buffer(f->snapshot->baseline.content, f->snapshot->baseline.length,
                         f->snapshot->baseline.record.amendment.content_sha256));
    }
  }
  if (!strcmp(name, "wrong-schema"))
    f->form->schema.version = 2;
  if (!strcmp(name, "wrong-entity"))
    strcpy(f->snapshot->baseline.record.amendment.entity_id, "other.query");
  if (!strcmp(name, "damaged-baseline"))
    f->snapshot->baseline.content[0] ^= 1;
  if (!strcmp(name, "damaged-current"))
    f->snapshot->content[0] ^= 1;
  if (!strcmp(name, "new-baseline-metadata"))
    strcpy(f->snapshot->baseline.record.amendment.source, "Unexpected source");
  if (!strcmp(name, "dirty-target"))
    OK(umi_structured_form_search_session_begin(restored, "form.search.other", f->preset));
  if (!strcmp(name, "clean-target"))
    OK(umi_structured_form_search_session_open(restored, f->document));
  OK(umi_structured_form_search_session_snapshot(restored, view));
  *before = *view;
  if (!strcmp(name, "wrong-store")) {
    OK(umi_data_server_create_memory(&other));
    CHECK(umi_record_draft_snapshot_check_store(other, f->snapshot) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (!strcmp(name, "later-head")) {
    UmiRecordAmendment metadata = {.entity_id = "form.search.example",
                                   .source = "Fictional concurrent writer",
                                   .actor = "Another example author",
                                   .reason = "Advance definition",
                                   .recorded_at_nanoseconds = 3};
    strcpy(f->preset->title, "Stored later");
    OK(umi_structured_form_search_preset_save(f->server, &metadata, &f->form->schema, f->preset, 1,
                                              &f->saved));
    OK(umi_record_draft_snapshot_check_store(f->server, f->snapshot));
  }
  UmiStatus status = umi_structured_form_search_session_restore(restored, f->snapshot);
  bool invalid = !strncmp(name, "wrong-", 6) || !strncmp(name, "damaged-", 8) ||
                 !strcmp(name, "new-baseline-metadata") || !strcmp(name, "dirty-target");
  if (invalid) {
    CHECK(status != UMI_STATUS_OK);
    f->form->schema.version = 1;
    OK(umi_structured_form_search_session_snapshot(restored, view));
    CHECK(!memcmp(view, before, sizeof *view));
    goto cleanup;
  }
  OK(status);
  OK(umi_structured_form_search_session_snapshot(restored, view));
  /* Keep the original comparison for review: strcmp returns an ordering,
   * so convert it to a boolean before comparing the dirty flag. */
#if 0
    CHECK(view->record.dirty==strcmp(name,"clean")!=0);
#endif
  CHECK(view->record.dirty == (strcmp(name, "clean") != 0));
  CHECK(
      !strcmp(view->preset.title, !strcmp(name, "clean") ? "Baseline search" : "Recovered query"));
  CHECK(view->generation == (!strcmp(name, "clean-target") ? 2 : 1) && view->edit_revision == 1);
  CHECK(view->record.expected_revision == (!strcmp(name, "new") ? 0 : 1));
  if (!strcmp(name, "later-head")) {
    OK(umi_structured_form_search_session_prepare_save(
        restored, view->generation, view->edit_revision, &f->attribution, f->request));
    status = umi_record_save_request_commit(f->server, f->request, &f->saved);
    CHECK(status == UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_search_session_finish_save(restored, f->request->request_id, status,
                                                         NULL) == status);
  }
cleanup:
  umi_record_draft_snapshot_bytes_free(bytes);
  umi_data_server_destroy(other);
  umi_structured_form_search_session_destroy(restored);
  free(view);
  free(before);
  search_fixture_free(f);
  return result;
}
