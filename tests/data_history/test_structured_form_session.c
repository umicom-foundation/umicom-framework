/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_history/test_structured_form_session.c
 * PURPOSE: Check form save ownership across editing, record switches and recovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../application_experience/structured_form_fixture.h"
#include "umicom/application/experience/structured_form_snapshot.h"
#include "umicom/data/structured_form_session.h"
static UmiStatus change_mass(FormFixture *fixture, UmiStructuredFormSession *session,
                             UmiStructuredFormSessionView *view, UmiStructuredFormEdit *edit,
                             double value) {
  UmiStatus status = umi_structured_form_session_snapshot(session, view);
  if (status != UMI_STATUS_OK)
    return status;
  fixture->after = view->form;
  UmiFormAnswer answer = form_mass(value);
  status = umi_structured_form_set(&fixture->schema, &fixture->after, fixture->after.revision,
                                   "parcel.mass", &answer);
  if (status == UMI_STATUS_OK)
    status = umi_structured_form_edit_capture(&fixture->schema, &view->form, &fixture->after, edit);
  return status;
}
int main(int argc, char **argv) {
  CHECK(argc == 2 || argc == 3);
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  UmiStructuredFormSession *session = NULL, *other = NULL;
  CHECK(umi_structured_form_session_create(&f->schema, &session) == UMI_STATUS_OK);
  CHECK(umi_structured_form_session_create(&f->schema, &other) == UMI_STATUS_OK);
  UmiStructuredFormSessionView *view = malloc(sizeof(*view)), *prior = malloc(sizeof(*prior));
  UmiStructuredFormEdit *edit = malloc(sizeof(*edit));
  UmiRecordSaveRequest *request = malloc(sizeof(*request)), *retry = malloc(sizeof(*retry));
  UmiRecordDraftSnapshot *recovery = malloc(sizeof(*recovery));
  UmiRecordDocument *document = malloc(sizeof(*document));
  CHECK(view && prior && edit && request && retry && recovery && document);
  UmiDataServer *server = NULL;
  if (argc == 3) {
    CHECK(strcmp(argv[2], "sqlite") == 0);
    CHECK(umi_data_server_create_sqlite(":memory:", &server) == UMI_STATUS_OK);
  } else
    CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
  UmiRecordDraftAttribution attribution = {.source = "Synthetic fixture",
                                           .actor = "Example reviewer",
                                           .reason = "Retain draft",
                                           .recorded_at_nanoseconds = 1U};
  UmiRecordRevision saved;
  CHECK(umi_structured_form_session_begin(session, "forms.parcel") == UMI_STATUS_OK);
  CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK);
  CHECK(view->record.ready && view->record.dirty && view->record.is_new &&
        view->form.saved_revision == 0U);
  if (strcmp(name, "begin") == 0) {
    *prior = *view;
    CHECK(umi_structured_form_session_begin(session, "forms.another") == UMI_STATUS_INVALID_STATE);
    CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK &&
          memcmp(view, prior, sizeof(*view)) == 0);
  } else if (strcmp(name, "discard-new") == 0) {
    uint64_t generation = view->generation;
    CHECK(umi_structured_form_session_discard(session, generation) == UMI_STATUS_OK);
    CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK &&
          !view->record.ready && view->generation > generation);
  } else if (strcmp(name, "context") == 0) {
    CHECK(change_mass(f, session, view, edit, 3.5) == UMI_STATUS_OK);
    uint64_t old = view->generation;
    CHECK(umi_structured_form_session_discard(session, old) == UMI_STATUS_OK);
    CHECK(umi_structured_form_session_begin(session, "forms.other") == UMI_STATUS_OK);
    CHECK(umi_structured_form_session_apply(session, old, edit) == UMI_STATUS_INVALID_STATE);
  } else if (strcmp(name, "required") == 0) {
    memset(request, 0xA5, sizeof(*request));
    *retry = *request;
    CHECK(umi_structured_form_session_prepare_save(session, view->generation, &attribution, 1,
                                                   request) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(request, retry, sizeof(*request)) == 0);
  } else if (strcmp(name, "recovery-new") == 0 || strcmp(name, "recovery-schema") == 0) {
    CHECK(change_mass(f, session, view, edit, 3.5) == UMI_STATUS_OK);
    CHECK(umi_structured_form_session_apply(session, view->generation, edit) == UMI_STATUS_OK);
    CHECK(umi_structured_form_session_capture(session, recovery) == UMI_STATUS_OK);
    if (strcmp(name, "recovery-schema") == 0) {
      UmiStructuredFormSchema incompatible = f->schema;
      incompatible.version = 2U;
      UmiStructuredFormSession *different = NULL;
      CHECK(umi_structured_form_session_create(&incompatible, &different) == UMI_STATUS_OK);
      CHECK(umi_structured_form_session_restore(different, recovery) == UMI_STATUS_INVALID_STATE);
      CHECK(umi_structured_form_session_snapshot(different, view) == UMI_STATUS_OK &&
            !view->record.ready);
      umi_structured_form_session_destroy(different);
    } else {
      CHECK(umi_record_draft_snapshot_check_store(server, recovery) == UMI_STATUS_OK);
      CHECK(umi_structured_form_session_restore(other, recovery) == UMI_STATUS_OK);
      CHECK(umi_structured_form_session_snapshot(other, view) == UMI_STATUS_OK &&
            view->record.dirty && view->form.answers[0].value.decimal_value == 3.5);
    }
  } else {
    CHECK(change_mass(f, session, view, edit, 3.5) == UMI_STATUS_OK);
    CHECK(umi_structured_form_session_apply(session, view->generation, edit) == UMI_STATUS_OK);
    CHECK(umi_structured_form_session_prepare_save(session, view->generation, &attribution, 0,
                                                   request) == UMI_STATUS_OK);
    if (strcmp(name, "pending") == 0) {
      CHECK(umi_structured_form_session_prepare_save(session, view->generation, &attribution, 0,
                                                     retry) == UMI_STATUS_BUSY);
      CHECK(umi_structured_form_session_begin(session, "forms.other") == UMI_STATUS_BUSY);
      CHECK(umi_structured_form_session_discard(session, view->generation) == UMI_STATUS_BUSY);
      CHECK(umi_structured_form_session_capture(session, recovery) == UMI_STATUS_BUSY);
    } else if (strcmp(name, "failed-save") == 0) {
      CHECK(umi_structured_form_session_finish_save(
                session, request->request_id, UMI_STATUS_IO_ERROR, NULL) == UMI_STATUS_IO_ERROR);
      CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK &&
            view->record.dirty && !view->record.saving);
      CHECK(umi_structured_form_session_prepare_save(session, view->generation, &attribution, 0,
                                                     retry) == UMI_STATUS_OK &&
            retry->request_id > request->request_id);
      CHECK(umi_structured_form_session_finish_save(session, request->request_id,
                                                    UMI_STATUS_IO_ERROR,
                                                    NULL) == UMI_STATUS_INVALID_STATE);
      CHECK(umi_structured_form_session_finish_save(
                session, retry->request_id, UMI_STATUS_CANCELLED, NULL) == UMI_STATUS_CANCELLED);
    } else {
      if (strcmp(name, "later-edit") == 0) {
        CHECK(change_mass(f, session, view, edit, 8.0) == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_apply(session, view->generation, edit) == UMI_STATUS_OK);
      }
      CHECK(umi_record_save_request_commit(server, request, &saved) == UMI_STATUS_OK);
      if (strcmp(name, "wrong-reply") == 0) {
        UmiRecordRevision wrong = saved;
        ++wrong.amendment.recorded_at_nanoseconds;
        CHECK(umi_structured_form_session_finish_save(session, request->request_id, UMI_STATUS_OK,
                                                      &wrong) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK &&
              view->record.saving);
      }
      CHECK(umi_structured_form_session_finish_save(session, request->request_id, UMI_STATUS_OK,
                                                    &saved) == UMI_STATUS_OK);
      CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK &&
            !view->record.saving && view->record.expected_revision == 1U);
      if (strcmp(name, "later-edit") == 0) {
        CHECK(view->record.dirty && view->form.revision == 3U && view->form.saved_revision == 2U &&
              view->form.answers[0].value.decimal_value == 8.0);
        CHECK(umi_structured_form_session_discard(session, view->generation) == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK &&
              !view->record.dirty && view->form.answers[0].value.decimal_value == 3.5);
      } else if (strcmp(name, "roundtrip") == 0 || strcmp(name, "no-op") == 0) {
        CHECK(!view->record.dirty && view->form.saved_revision == view->form.revision);
        CHECK(umi_record_document_open_reviewed(server, &saved, document) == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_open(other, document) == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_snapshot(other, view) == UMI_STATUS_OK &&
              !view->record.dirty && view->form.answers[0].value.decimal_value == 3.5);
        if (strcmp(name, "no-op") == 0) {
          CHECK(change_mass(f, other, view, edit, 3.5) == UMI_STATUS_OK && edit->count == 0U);
          CHECK(umi_structured_form_session_apply(other, view->generation, edit) == UMI_STATUS_OK);
          CHECK(umi_structured_form_session_snapshot(other, view) == UMI_STATUS_OK &&
                !view->record.dirty);
        }
      } else if (strcmp(name, "recovery-saved") == 0 || strcmp(name, "recovery-conflict") == 0) {
        CHECK(change_mass(f, session, view, edit, 8.0) == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_apply(session, view->generation, edit) == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_capture(session, recovery) == UMI_STATUS_OK);
        CHECK(umi_record_draft_snapshot_check_store(server, recovery) == UMI_STATUS_OK);
        if (strcmp(name, "recovery-conflict") == 0) {
          f->after.revision = 2U;
          CHECK(umi_structured_form_snapshot_encode(&f->schema, &f->after, recovery->content,
                                                    sizeof(recovery->content),
                                                    &recovery->length) == UMI_STATUS_OK);
          CHECK(umi_structured_form_session_restore(other, recovery) == UMI_STATUS_INVALID_STATE);
        } else {
          CHECK(umi_structured_form_session_restore(other, recovery) == UMI_STATUS_OK);
          CHECK(umi_structured_form_session_snapshot(other, view) == UMI_STATUS_OK &&
                view->record.dirty && view->form.answers[0].value.decimal_value == 8.0 &&
                view->form.saved_revision == 2U);
        }
      } else if (strcmp(name, "request-lifetime") == 0) {
        CHECK(umi_structured_form_session_begin(session, "forms.another") == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK);
        CHECK(umi_structured_form_session_prepare_save(session, view->generation, &attribution, 0,
                                                       retry) == UMI_STATUS_OK &&
              retry->request_id > request->request_id);
        CHECK(umi_structured_form_session_finish_save(session, request->request_id, UMI_STATUS_OK,
                                                      &saved) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_structured_form_session_finish_save(
                  session, retry->request_id, UMI_STATUS_CANCELLED, NULL) == UMI_STATUS_CANCELLED);
      } else if (strcmp(name, "wrong-reply") != 0)
        CHECK(0);
    }
    CHECK(umi_structured_form_session_snapshot(session, view) == UMI_STATUS_OK);
    if (view->record.saving)
      CHECK(umi_structured_form_session_finish_save(
                session, request->request_id, UMI_STATUS_CANCELLED, NULL) == UMI_STATUS_CANCELLED);
  }
  umi_data_server_destroy(server);
  umi_structured_form_session_destroy(other);
  umi_structured_form_session_destroy(session);
  free(document);
  free(recovery);
  free(retry);
  free(request);
  free(edit);
  free(prior);
  free(view);
  free(f);
  return 0;
}
