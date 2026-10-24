/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_forms/test_submit.c
 * PURPOSE: Exercise browser identity, required answers and atomic edit preparation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  CHECK(umi_structured_form_init(&f->schema, &f->before) == UMI_STATUS_OK);
  UmiStructuredFormEdit *edit = malloc(sizeof(*edit)), *prior = malloc(sizeof(*prior));
  CHECK(edit != NULL && prior != NULL);
  memset(edit, 0xA5, sizeof(*edit));
  *prior = *edit;
  char body[WEB_BODY_CAPACITY];
  if (strcmp(name, "readonly") == 0)
    f->fields[2].read_only = 1;
  if (strcmp(name, "revision-overflow") == 0)
    f->before.revision = UINT64_MAX;
  if (strcmp(name, "no-op") == 0) {
    f->before.answers[0] = form_mass(3.5);
    f->before.answers[1] = form_text("2024-02-29T12:00:00Z");
    f->before.answers[2] = form_text("A note");
  }
  f->saved = f->before;
  web_body(f, body);
  UmiStatus expected = UMI_STATUS_OK;
  int submitting = 19;
  if (strcmp(name, "draft") == 0 || strcmp(name, "changed-after") == 0 ||
      strcmp(name, "no-op") == 0 || strcmp(name, "readonly") == 0) {
  } else if (strcmp(name, "submit") == 0)
    web_replace(body, "intent=draft", "intent=submit");
  else if (strcmp(name, "incomplete") == 0) {
    web_replace(body, "intent=draft", "intent=submit");
    web_replace(body, "state.parcel.mass=1", "state.parcel.mass=0");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (strcmp(name, "stale-revision") == 0) {
    web_replace(body, "revision=1", "revision=2");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (strcmp(name, "schema") == 0) {
    web_replace(body, "version=1", "version=2");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (strcmp(name, "token") == 0) {
    web_replace(body, "csrf=test_token", "csrf=wrong_token");
    expected = UMI_STATUS_PERMISSION_DENIED;
  } else if (strcmp(name, "unknown") == 0) {
    strcat(body, "&another=1");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (strcmp(name, "duplicate") == 0) {
    strcat(body, "&revision=1");
    expected = UMI_STATUS_ALREADY_EXISTS;
  } else if (strcmp(name, "missing") == 0) {
    web_replace(body, "&reason.parcel.mass=", "");
    expected = UMI_STATUS_NOT_FOUND;
  } else if (strcmp(name, "unit") == 0) {
    web_replace(body, "mass.kg", "mass.lb");
    expected = UMI_STATUS_INVALID_STATE;
  } else if (strcmp(name, "invalid-second") == 0) {
    web_replace(body, "2024-02-29T12%3A00%3A00Z", "2024-02-29T12%3A00%3A00");
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (strcmp(name, "missing-state") == 0) {
    web_replace(body, "state.parcel.mass=1", "state.parcel.mass=2");
    web_replace(body, "reason.parcel.mass=", "reason.parcel.mass=Not+measured");
  } else if (strcmp(name, "newlines") == 0) {
    web_replace(body, "value.parcel.note=A+note", "value.parcel.note=%0D%0Asecond%0D%0Aline");
  } else if (strcmp(name, "bare-cr") == 0) {
    web_replace(body, "value.parcel.note=A+note", "value.parcel.note=first%0Dsecond");
    expected = UMI_STATUS_PARSE_ERROR;
  } else if (strcmp(name, "revision-overflow") == 0)
    expected = UMI_STATUS_CAPACITY_EXCEEDED;
  else
    CHECK(0);
  CHECK(umi_web_structured_form_prepare(&f->schema, &f->before, body, strlen(body), "test_token",
                                        edit, &submitting) == expected);
  CHECK(memcmp(&f->before, &f->saved, sizeof(f->before)) == 0);
  if (expected != UMI_STATUS_OK) {
    CHECK(submitting == 19 && memcmp(edit, prior, sizeof(*edit)) == 0);
  } else {
    CHECK(submitting == (strcmp(name, "submit") == 0));
    if (strcmp(name, "changed-after") == 0) {
      UmiFormAnswer later = form_mass(4.0);
      CHECK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass",
                                    &later) == UMI_STATUS_OK);
      f->saved = f->before;
      CHECK(umi_structured_form_edit_apply(&f->schema, &f->before, edit) ==
                UMI_STATUS_INVALID_STATE &&
            memcmp(&f->before, &f->saved, sizeof(f->before)) == 0);
    } else {
      CHECK(umi_structured_form_edit_apply(&f->schema, &f->before, edit) == UMI_STATUS_OK);
      CHECK(f->before.revision == (strcmp(name, "no-op") == 0 ? 1U : 2U));
      if (strcmp(name, "readonly") == 0) {
        CHECK(edit->count == 2U && f->before.answers[2].state == UMI_FORM_UNANSWERED);
        strcat(body, "&state.parcel.note=1");
        CHECK(umi_web_structured_form_prepare(&f->schema, &f->saved, body, strlen(body),
                                              "test_token", edit,
                                              &submitting) == UMI_STATUS_INVALID_STATE);
      }
      if (strcmp(name, "newlines") == 0)
        CHECK(strcmp(f->before.answers[2].value.text_value, "\nsecond\nline") == 0);
      if (strcmp(name, "missing-state") == 0)
        CHECK(f->before.answers[0].state == UMI_FORM_UNKNOWN &&
              f->before.answers[0].value.type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE);
    }
  }
  free(prior);
  free(edit);
  free(f);
  return 0;
}
