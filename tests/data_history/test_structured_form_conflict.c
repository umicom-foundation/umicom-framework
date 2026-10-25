/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_structured_form_conflict.c
 *
 * PURPOSE:
 *   Reconciliation preserves local work on every refusal and retains the final
 *   storage revision check. Memory and SQLite exercise the same contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Reconciliation preserves local work on every refusal and retains the final
 * storage revision check. Memory and SQLite exercise the same contract. */
#include "../application_experience/structured_form_fixture.h"
#undef CHECK
#include "umicom/data/structured_form_conflict.h"
#include "umicom/data/structured_form_record.h"
#define CHECK(x)                                                                                   \
  do {                                                                                             \
    if (!(x)) {                                                                                    \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x);                                      \
      result = 1;                                                                                  \
      goto cleanup;                                                                                \
    }                                                                                              \
  } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static UmiStatus local_mass(FormFixture *f, UmiStructuredFormSession *session,
                            UmiStructuredFormSessionView *view, UmiStructuredFormEdit *edit,
                            double value) {
  UmiStatus status = umi_structured_form_session_snapshot(session, view);
  if (status == UMI_STATUS_OK) {
    f->after = view->form;
    UmiFormAnswer answer = form_mass(value);
    status =
        umi_structured_form_set(&f->schema, &f->after, f->after.revision, "parcel.mass", &answer);
  }
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_edit_capture(&f->schema, &view->form, &f->after, edit);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_session_apply(session, view->generation, edit);
  return status;
}
int main(int argc, char **argv) {
  if (argc != 2 && argc != 3)
    return 2;
  const char *name = argv[1];
  const char *cases[] = {"local",
                         "incoming",
                         "base",
                         "custom",
                         "disjoint",
                         "unresolved",
                         "late-local",
                         "generation",
                         "review-revision",
                         "other-session",
                         "new-collision",
                         "wrong-entity",
                         "corrupt",
                         "same-revision",
                         "pending",
                         "request-lifetime",
                         "stale-after-review",
                         "required",
                         "preview",
                         "recovery"};
  bool found = false;
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i)
    if (strcmp(name, cases[i]) == 0)
      found = true;
  if (!found || (argc == 3 && strcmp(argv[2], "sqlite") != 0))
    return 2;
  int result = 0;
  FormFixture *f = form_fixture_new();
  UmiStructuredFormSession *session = NULL, *other = NULL;
  UmiStructuredFormConflict *review = NULL, *refused = NULL;
  UmiDataServer *server = NULL;
  UmiStructuredFormSessionView *view = calloc(1, sizeof(*view)), *prior = calloc(1, sizeof(*prior));
  UmiStructuredFormEdit *edit = calloc(1, sizeof(*edit));
  UmiStructuredForm *incoming_form = calloc(1, sizeof(*incoming_form)),
                    *preview = calloc(1, sizeof(*preview));
  UmiRecordDocument *incoming = calloc(1, sizeof(*incoming)),
                    *baseline = calloc(1, sizeof(*baseline));
  UmiRecordSaveRequest *request = calloc(1, sizeof(*request));
  UmiRecordDraftSnapshot *snapshot = calloc(1, sizeof(*snapshot));
  UmiRecordRevision saved;
  UmiFormMergeState merge_state;
  uint64_t cancelled_id = 0;
  CHECK(f && view && prior && edit && incoming_form && preview && incoming && baseline && request &&
        snapshot);
  if (argc == 3)
    OK(umi_data_server_create_sqlite(":memory:", &server));
  else
    OK(umi_data_server_create_memory(&server));
  OK(umi_structured_form_session_create(&f->schema, &session));
  OK(umi_structured_form_session_create(&f->schema, &other));
  UmiRecordAmendment metadata = {.entity_id = "forms.parcel",
                                 .source = "Fictional source",
                                 .actor = "Example reviewer",
                                 .reason = "Initial form",
                                 .recorded_at_nanoseconds = 1U};
  UmiRecordDraftAttribution attribution = {.source = "Fictional source",
                                           .actor = "Example reviewer",
                                           .reason = "Reconcile answers",
                                           .recorded_at_nanoseconds = 1U};
  UmiFormAnswer mass = form_mass(10);
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &mass));
  OK(umi_structured_form_record_save(server, &metadata, &f->schema, &f->before, 0, &saved));
  OK(umi_record_document_latest(server, "forms.parcel", baseline));
  if (strcmp(name, "new-collision") == 0) {
    OK(umi_structured_form_session_begin(session, "forms.parcel"));
    OK(umi_structured_form_session_snapshot(session, view));
    CHECK(umi_structured_form_session_review(session, view->generation, baseline, &refused) ==
              UMI_STATUS_INVALID_STATE &&
          refused == NULL);
    goto cleanup;
  }
  OK(umi_structured_form_session_open(session, baseline));
  OK(umi_structured_form_session_open(other, baseline));
  OK(local_mass(f, session, view, edit, 20));
  *incoming_form = f->before;
  UmiFormAnswer changed = strcmp(name, "disjoint") == 0 ? form_text("Remote note") : form_mass(30);
  OK(umi_structured_form_set(&f->schema, incoming_form, incoming_form->revision,
                             strcmp(name, "disjoint") == 0 ? "parcel.note" : "parcel.mass",
                             &changed));
  OK(umi_structured_form_record_save(server, &metadata, &f->schema, incoming_form, 1, &saved));
  OK(umi_record_document_latest(server, "forms.parcel", incoming));
  OK(umi_structured_form_session_snapshot(session, view));
  *prior = *view;
  if (strcmp(name, "wrong-entity") == 0) {
    memcpy(incoming->record.amendment.entity_id, "forms.other", 12);
    CHECK(umi_structured_form_session_review(session, view->generation, incoming, &refused) ==
              UMI_STATUS_INVALID_STATE &&
          refused == NULL);
    goto cleanup;
  }
  if (strcmp(name, "corrupt") == 0) {
    incoming->content[0] ^= 1;
    CHECK(umi_structured_form_session_review(session, view->generation, incoming, &refused) !=
              UMI_STATUS_OK &&
          refused == NULL);
    goto cleanup;
  }
  if (strcmp(name, "same-revision") == 0) {
    *incoming = *baseline;
    memcpy(incoming->record.amendment.actor, "Changed actor", 14);
    CHECK(umi_structured_form_session_review(session, view->generation, incoming, &refused) ==
              UMI_STATUS_INVALID_STATE &&
          refused == NULL);
    goto cleanup;
  }
  OK(umi_structured_form_session_capture(session, snapshot));
  OK(umi_record_draft_snapshot_check_store(server, snapshot));
  OK(umi_structured_form_session_review(session, view->generation, incoming, &review));
  OK(umi_structured_form_conflict_state(review, &merge_state));
  CHECK(merge_state.conflict_count == (strcmp(name, "disjoint") == 0 ? 0U : 1U));
  if (strcmp(name, "unresolved") == 0) {
    CHECK(umi_structured_form_session_accept_review(session, view->generation, review, 1, 0) ==
          UMI_STATUS_INVALID_STATE);
    OK(umi_structured_form_session_snapshot(session, view));
    CHECK(view->generation == prior->generation &&
          view->form.answers[0].value.decimal_value == 20 && view->record.expected_revision == 1);
    goto cleanup;
  }
  if (strcmp(name, "custom") == 0) {
    UmiFormAnswer custom = form_mass(40);
    OK(umi_structured_form_conflict_custom(review, 1, "parcel.mass", &custom));
  } else if (strcmp(name, "disjoint") != 0)
    OK(umi_structured_form_conflict_select(
        review, 1, "parcel.mass",
        strcmp(name, "incoming") == 0 ? UMI_FORM_MERGE_TAKE_INCOMING
        : strcmp(name, "base") == 0   ? UMI_FORM_MERGE_KEEP_BASE
                                      : UMI_FORM_MERGE_KEEP_LOCAL));
  OK(umi_structured_form_conflict_state(review, &merge_state));
  if (strcmp(name, "late-local") == 0) {
    OK(local_mass(f, session, view, edit, 50));
    CHECK(umi_structured_form_session_accept_review(session, view->generation, review,
                                                    merge_state.revision,
                                                    0) == UMI_STATUS_INVALID_STATE);
    OK(umi_structured_form_session_snapshot(session, view));
    CHECK(view->form.answers[0].value.decimal_value == 50 && view->record.expected_revision == 1);
    goto cleanup;
  }
  if (strcmp(name, "generation") == 0) {
    CHECK(umi_structured_form_session_accept_review(session, view->generation + 1, review,
                                                    merge_state.revision,
                                                    0) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "review-revision") == 0) {
    CHECK(umi_structured_form_session_accept_review(session, view->generation, review,
                                                    merge_state.revision + 1,
                                                    0) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "other-session") == 0) {
    OK(local_mass(f, other, view, edit, 20));
    CHECK(umi_structured_form_session_accept_review(other, view->generation, review,
                                                    merge_state.revision,
                                                    0) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "required") == 0) {
    CHECK(umi_structured_form_session_accept_review(session, view->generation, review,
                                                    merge_state.revision,
                                                    1) == UMI_STATUS_INVALID_STATE);
    goto cleanup;
  }
  if (strcmp(name, "preview") == 0) {
    OK(umi_structured_form_conflict_preview(review, merge_state.revision, 0, preview));
    CHECK(preview->answers[0].value.decimal_value == 20);
    OK(umi_structured_form_session_snapshot(session, view));
    CHECK(view->record.expected_revision == 1 && view->generation == prior->generation);
    goto cleanup;
  }
  if (strcmp(name, "pending") == 0 || strcmp(name, "request-lifetime") == 0) {
    OK(umi_structured_form_session_prepare_save(session, view->generation, &attribution, 0,
                                                request));
    cancelled_id = request->request_id;
    CHECK(umi_structured_form_session_review(session, view->generation, incoming, &refused) ==
              UMI_STATUS_BUSY &&
          refused == NULL);
    CHECK(umi_structured_form_session_accept_review(session, view->generation, review,
                                                    merge_state.revision, 0) == UMI_STATUS_BUSY);
    CHECK(umi_structured_form_session_finish_save(
              session, request->request_id, UMI_STATUS_CANCELLED, NULL) == UMI_STATUS_CANCELLED);
    if (strcmp(name, "pending") == 0)
      goto cleanup;
  }
  OK(umi_structured_form_session_accept_review(session, view->generation, review,
                                               merge_state.revision, 0));
  OK(umi_structured_form_session_snapshot(session, view));
  CHECK(view->generation > prior->generation && view->record.expected_revision == 2);
  if (strcmp(name, "incoming") == 0) {
    CHECK(!view->record.dirty && view->form.answers[0].value.decimal_value == 30);
    goto cleanup;
  }
  CHECK(view->record.dirty);
  CHECK(view->form.answers[0].value.decimal_value == (strcmp(name, "base") == 0     ? 10
                                                      : strcmp(name, "custom") == 0 ? 40
                                                                                    : 20));
  if (strcmp(name, "disjoint") == 0)
    CHECK(strcmp(view->form.answers[2].value.text_value, "Remote note") == 0);
  if (strcmp(name, "recovery") == 0) {
    OK(umi_structured_form_session_capture(session, snapshot));
    OK(umi_record_draft_snapshot_check_store(server, snapshot));
    OK(umi_structured_form_session_restore(other, snapshot));
    OK(umi_structured_form_session_snapshot(other, view));
    CHECK(view->record.expected_revision == 2 && view->form.answers[0].value.decimal_value == 20);
    goto cleanup;
  }
  if (strcmp(name, "stale-after-review") == 0) {
    mass = form_mass(60);
    OK(umi_structured_form_set(&f->schema, incoming_form, incoming_form->revision, "parcel.mass",
                               &mass));
    OK(umi_structured_form_record_save(server, &metadata, &f->schema, incoming_form, 2, &saved));
  }
  OK(umi_structured_form_session_prepare_save(session, view->generation, &attribution, 0, request));
  CHECK(request->request_id > cancelled_id && request->proposed.previous_revision == 2);
  UmiStatus commit = umi_record_save_request_commit(server, request, &saved);
  if (strcmp(name, "stale-after-review") == 0) {
    CHECK(commit == UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_session_finish_save(session, request->request_id, commit, NULL) ==
          commit);
    OK(umi_structured_form_session_snapshot(session, view));
    CHECK(view->record.dirty && view->record.expected_revision == 2);
  } else {
    OK(commit);
    CHECK(saved.revision == 3);
    OK(umi_structured_form_session_finish_save(session, request->request_id, commit, &saved));
    OK(umi_structured_form_session_snapshot(session, view));
    CHECK(!view->record.dirty);
  }
cleanup:
  umi_structured_form_conflict_destroy(refused);
  umi_structured_form_conflict_destroy(review);
  umi_structured_form_session_destroy(other);
  umi_structured_form_session_destroy(session);
  umi_data_server_destroy(server);
  free(snapshot);
  free(request);
  free(incoming);
  free(baseline);
  free(incoming_form);
  free(preview);
  free(edit);
  free(view);
  free(prior);
  free(f);
  return result;
}
