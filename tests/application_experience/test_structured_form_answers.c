/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_answers.c
 * PURPOSE: Keep missing answers, units, revisions and timestamp meaning explicit.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  const char *name = argv[1];
  UmiFormAnswer a = form_mass(3.5);
  UmiApplicationExperienceValidationReport *report = malloc(sizeof(*report));
  CHECK(report != NULL);
  if (strcmp(name, "submit") == 0) {
    CHECK(umi_structured_form_validate(&f->schema, &f->after, 0, report) == UMI_STATUS_OK &&
          report->error_count == 0U);
    CHECK(umi_structured_form_validate(&f->schema, &f->after, 1, report) == UMI_STATUS_OK &&
          report->error_count == 2U);
    CHECK(umi_structured_form_set(&f->schema, &f->after, 1U, "parcel.mass", &a) == UMI_STATUS_OK);
    a = form_text("2024-02-29T12:00:00+01:00");
    CHECK(umi_structured_form_set(&f->schema, &f->after, 2U, "parcel.observed", &a) ==
          UMI_STATUS_OK);
    CHECK(umi_structured_form_validate(&f->schema, &f->after, 1, report) == UMI_STATUS_OK &&
          report->error_count == 0U);
    CHECK(strcmp(f->after.answers[1].value.text_value, "2024-02-29T12:00:00+01:00") == 0);
  } else if (strcmp(name, "missing") == 0) {
    a = form_missing(UMI_FORM_UNKNOWN, "Not measured");
    CHECK(umi_structured_form_set(&f->schema, &f->after, 1U, "parcel.mass", &a) == UMI_STATUS_OK);
    CHECK(f->after.answers[0].state == UMI_FORM_UNKNOWN &&
          f->after.answers[0].value.type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE);
    CHECK(umi_structured_form_validate(&f->schema, &f->after, 1, report) == UMI_STATUS_OK &&
          report->error_count == 1U);
    a = form_missing(UMI_FORM_NOT_APPLICABLE, "No note required");
    CHECK(umi_structured_form_set(&f->schema, &f->after, 2U, "parcel.note", &a) == UMI_STATUS_OK);
    a = form_missing(UMI_FORM_UNANSWERED, "");
    CHECK(umi_structured_form_set(&f->schema, &f->after, 3U, "parcel.mass", &a) == UMI_STATUS_OK);
  } else if (strcmp(name, "unit") == 0) {
    memcpy(a.unit_id, "mass.lb", 8U);
    CHECK(umi_structured_form_set(&f->schema, &f->after, 1U, "parcel.mass", &a) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&f->after, &f->saved, sizeof(f->after)) == 0);
  } else if (strcmp(name, "stale") == 0) {
    CHECK(umi_structured_form_set(&f->schema, &f->after, 2U, "parcel.mass", &a) ==
          UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&f->after, &f->saved, sizeof(f->after)) == 0);
  } else if (strcmp(name, "saved") == 0) {
    CHECK(umi_structured_form_set(&f->schema, &f->after, 1U, "parcel.mass", &a) == UMI_STATUS_OK);
    CHECK(umi_structured_form_mark_saved(&f->after, 1U) == UMI_STATUS_INVALID_STATE &&
          f->after.saved_revision == 1U);
    CHECK(umi_structured_form_mark_saved(&f->after, 2U) == UMI_STATUS_OK &&
          f->after.saved_revision == 2U);
  } else if (strcmp(name, "overflow") == 0) {
    f->after.revision = UINT64_MAX;
    f->saved = f->after;
    CHECK(umi_structured_form_set(&f->schema, &f->after, UINT64_MAX, "parcel.mass", &a) ==
          UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&f->after, &f->saved, sizeof(f->after)) == 0);
    a = form_missing(UMI_FORM_UNANSWERED, "");
    CHECK(umi_structured_form_set(&f->schema, &f->after, UINT64_MAX, "parcel.mass", &a) ==
          UMI_STATUS_OK);
  } else if (strcmp(name, "version") == 0) {
    memset(report, 0x5a, sizeof(*report));
    f->after.schema_version = 2U;
    CHECK(umi_structured_form_validate(&f->schema, &f->after, 0, report) ==
          UMI_STATUS_INVALID_STATE);
    const unsigned char *bytes = (const unsigned char *)report;
    for (size_t i = 0U; i < sizeof(*report); ++i)
      CHECK(bytes[i] == 0x5aU);
  } else if (strcmp(name, "shape") == 0) {
    a = form_missing(UMI_FORM_UNKNOWN, "");
    CHECK(umi_structured_form_answer_validate(&f->schema, 0U, &a, 0) == UMI_STATUS_INVALID_STATE);
    a = form_missing(UMI_FORM_UNKNOWN, "Not measured");
    a.value.integer_value = 42;
    CHECK(umi_structured_form_answer_validate(&f->schema, 0U, &a, 0) == UMI_STATUS_INVALID_STATE);
    a = form_mass(3.5);
    memset(a.unit_id, 'x', sizeof(a.unit_id));
    CHECK(umi_structured_form_answer_validate(&f->schema, 0U, &a, 0) == UMI_STATUS_INVALID_STATE);
    CHECK(!umi_form_answer_equal(&a, &a));
  } else if (strcmp(name, "time") == 0) {
    a = form_text("2024-02-29T12:00:00");
    CHECK(umi_structured_form_set(&f->schema, &f->after, 1U, "parcel.observed", &a) ==
          UMI_STATUS_PARSE_ERROR);
    CHECK(memcmp(&f->after, &f->saved, sizeof(f->after)) == 0);
  } else if (strcmp(name, "readonly") == 0) {
    f->fields[0].read_only = 1;
    CHECK(umi_structured_form_set(&f->schema, &f->after, 1U, "parcel.mass", &a) ==
          UMI_STATUS_PERMISSION_DENIED);
    CHECK(memcmp(&f->after, &f->saved, sizeof(f->after)) == 0);
  } else
    CHECK(0);
  free(report);
  free(f);
  return 0;
}
