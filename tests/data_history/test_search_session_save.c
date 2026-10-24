/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Immutable save requests keep later edits and reject stale completion tokens. */
#include "search_session_fixture.h"
#undef CHECK
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
                         "amend",
                         "clean",
                         "invalid-attribution",
                         "zero-time",
                         "stale-edit",
                         "stale-generation",
                         "prepare",
                         "busy",
                         "later-edit",
                         "failure",
                         "wrong-token",
                         "wrong-metadata",
                         "duplicate-completion",
                         "stale-head",
                         "collision",
                         "discard-pending",
                         "restore-pending",
                         "capture-pending",
                         "request-tamper",
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
  UmiRecordSaveRequest *before = malloc(sizeof *before);
  UmiRecordDraftSnapshot *snapshot_before = malloc(sizeof *snapshot_before);
  CHECK(before && snapshot_before);
  OK(search_fixture_new(!strcmp(name, "new") || !strcmp(name, "collision") ? 1 : 2, argc == 3, &f));
  if (strcmp(name, "clean"))
    OK(search_fixture_edit(f, "Save this search"));
  OK(umi_structured_form_search_session_capture(f->session, f->snapshot));
  if (!strcmp(name, "invalid-attribution"))
    f->attribution.actor[0] = 0;
  if (!strcmp(name, "zero-time"))
    f->attribution.recorded_at_nanoseconds = 0;
  memset(before, 0x43, sizeof *before);
  *f->request = *before;
  if (!strcmp(name, "null")) {
    CHECK(umi_structured_form_search_session_prepare_save(NULL, 1, 1, &f->attribution,
                                                          f->request) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_prepare_save(f->session, f->view->generation,
                                                          f->view->edit_revision, NULL,
                                                          f->request) != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_prepare_save(f->session, f->view->generation,
                                                          f->view->edit_revision, &f->attribution,
                                                          NULL) != UMI_STATUS_OK);
    CHECK(!memcmp(f->request, before, sizeof *before));
    goto cleanup;
  }
  UmiStatus status = umi_structured_form_search_session_prepare_save(
      f->session, !strcmp(name, "stale-generation") ? 0 : f->view->generation,
      !strcmp(name, "stale-edit") ? 1 : f->view->edit_revision, &f->attribution, f->request);
  if (!strcmp(name, "clean") || !strcmp(name, "invalid-attribution") ||
      !strcmp(name, "zero-time") || !strcmp(name, "stale-edit") ||
      !strcmp(name, "stale-generation")) {
    CHECK(status != UMI_STATUS_OK && !memcmp(f->request, before, sizeof *before));
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(!f->view->record.saving);
    goto cleanup;
  }
  OK(status);
  uint64_t request_id = f->request->request_id;
  CHECK(request_id != 0);
  OK(umi_structured_form_search_session_snapshot(f->session, f->view));
  CHECK(f->view->record.saving);
  if (!strcmp(name, "prepare") || !strcmp(name, "busy") || !strcmp(name, "failure") ||
      !strcmp(name, "discard-pending") || !strcmp(name, "restore-pending") ||
      !strcmp(name, "capture-pending")) {
    if (!strcmp(name, "busy"))
      CHECK(umi_structured_form_search_session_prepare_save(f->session, f->view->generation,
                                                            f->view->edit_revision, &f->attribution,
                                                            before) == UMI_STATUS_BUSY);
    if (!strcmp(name, "discard-pending"))
      CHECK(umi_structured_form_search_session_discard(f->session, f->view->generation,
                                                       f->view->edit_revision) == UMI_STATUS_BUSY);
    if (!strcmp(name, "restore-pending"))
      CHECK(umi_structured_form_search_session_restore(f->session, f->snapshot) == UMI_STATUS_BUSY);
    if (!strcmp(name, "capture-pending")) {
      *snapshot_before = *f->snapshot;
      CHECK(umi_structured_form_search_session_capture(f->session, f->snapshot) ==
                UMI_STATUS_BUSY &&
            !memcmp(f->snapshot, snapshot_before, sizeof *snapshot_before));
    }
    CHECK(umi_structured_form_search_session_finish_save(
              f->session, request_id, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_IO_ERROR);
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(!f->view->record.saving && f->view->record.dirty &&
          !strcmp(f->view->preset.title, "Save this search"));
    goto cleanup;
  }
  if (!strcmp(name, "later-edit"))
    OK(search_fixture_edit(f, "Typed later"));
  if (!strcmp(name, "stale-head") || !strcmp(name, "collision")) {
    UmiRecordAmendment metadata = {.source = "Fictional concurrent writer",
                                   .actor = "Another example author",
                                   .reason = "Other revision",
                                   .recorded_at_nanoseconds = 2};
    strcpy(metadata.entity_id, f->view->record.entity_id);
    strcpy(f->preset->title, "Other stored query");
    OK(umi_structured_form_search_preset_save(f->server, &metadata, &f->form->schema, f->preset,
                                              !strcmp(name, "collision") ? 0 : 1, &f->saved));
  }
  if (!strcmp(name, "request-tamper"))
    f->request->content[0] ^= 1;
  status = umi_record_save_request_commit(f->server, f->request, &f->saved);
  if (!strcmp(name, "stale-head") || !strcmp(name, "collision") ||
      !strcmp(name, "request-tamper")) {
    CHECK(status != UMI_STATUS_OK);
    CHECK(umi_structured_form_search_session_finish_save(f->session, request_id, status, NULL) ==
          status);
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(f->view->record.dirty && !f->view->record.saving);
    goto cleanup;
  }
  OK(status);
  if (!strcmp(name, "wrong-token")) {
    CHECK(umi_structured_form_search_session_finish_save(f->session, request_id + 1, UMI_STATUS_OK,
                                                         &f->saved) != UMI_STATUS_OK);
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(f->view->record.saving);
  }
  if (!strcmp(name, "wrong-metadata")) {
    UmiRecordRevision wrong = f->saved;
    strcpy(wrong.amendment.actor, "Not the captured author");
    CHECK(umi_structured_form_search_session_finish_save(f->session, request_id, UMI_STATUS_OK,
                                                         &wrong) != UMI_STATUS_OK);
    OK(umi_structured_form_search_session_snapshot(f->session, f->view));
    CHECK(f->view->record.saving);
  }
  OK(umi_structured_form_search_session_finish_save(f->session, request_id, UMI_STATUS_OK,
                                                    &f->saved));
  OK(umi_structured_form_search_session_snapshot(f->session, f->view));
  CHECK(!f->view->record.saving &&
        f->view->record.expected_revision == (!strcmp(name, "new") ? 1 : 2));
  CHECK(f->view->record.dirty == (!strcmp(name, "later-edit")));
  CHECK(!strcmp(f->view->preset.title,
                !strcmp(name, "later-edit") ? "Typed later" : "Save this search"));
  OK(umi_record_document_latest(f->server, f->view->record.entity_id, f->document));
  OK(umi_structured_form_search_preset_decode(&f->form->schema, f->document->content,
                                              f->document->length, f->preset));
  CHECK(!strcmp(f->preset->title, "Save this search"));
  if (!strcmp(name, "duplicate-completion"))
    CHECK(umi_structured_form_search_session_finish_save(f->session, request_id, UMI_STATUS_OK,
                                                         &f->saved) != UMI_STATUS_OK);
cleanup:
  free(before);
  free(snapshot_before);
  search_fixture_free(f);
  return result;
}
