/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_text.c
 * PURPOSE: Cover semantic conversion without weakening shared answer rules.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "structured_form_fixture.h"
#include "umicom/application/experience/structured_form_text.h"
int main(int argc, char **argv) {
  CHECK(argc == 2);
  FormFixture *f = form_fixture_new();
  CHECK(f != NULL);
  UmiFormTextAnswer text = {0}, rendered;
  UmiFormAnswer answer = form_mass(9.0), original = answer;
  text.state = UMI_FORM_KNOWN;
  strcpy(text.value, "3.5");
  strcpy(text.unit_id, "mass.kg");
  const char *name = argv[1];
  if (strcmp(name, "decimal") == 0) {
    CHECK(umi_structured_form_answer_parse(&f->schema, 0U, &text, &answer) == UMI_STATUS_OK &&
          answer.value.decimal_value == 3.5);
    CHECK(umi_structured_form_answer_format(&f->schema, 0U, &answer, &rendered) == UMI_STATUS_OK &&
          strcmp(rendered.value, "3.5") == 0);
  } else if (strcmp(name, "integer") == 0) {
    f->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    f->fields[0].kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER;
    f->fields[0].enforce_number_range = 0;
    strcpy(text.value, "-9223372036854775808");
    CHECK(umi_structured_form_answer_parse(&f->schema, 0U, &text, &answer) == UMI_STATUS_OK &&
          answer.value.integer_value == INT64_MIN);
    CHECK(umi_structured_form_answer_format(&f->schema, 0U, &answer, &rendered) == UMI_STATUS_OK &&
          strcmp(text.value, rendered.value) == 0);
  } else if (strcmp(name, "boolean") == 0) {
    f->fields[2].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN;
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_BOOLEAN;
    text.unit_id[0] = '\0';
    strcpy(text.value, "false");
    CHECK(umi_structured_form_answer_parse(&f->schema, 2U, &text, &answer) == UMI_STATUS_OK &&
          answer.value.boolean_value == 0);
    strcpy(text.value, "0");
    CHECK(umi_structured_form_answer_parse(&f->schema, 2U, &text, &answer) ==
          UMI_STATUS_PARSE_ERROR);
  } else if (strcmp(name, "text") == 0) {
    text.unit_id[0] = '\0';
    strcpy(text.value, "A <note> & a second line\n");
    CHECK(umi_structured_form_answer_parse(&f->schema, 2U, &text, &answer) == UMI_STATUS_OK);
    CHECK(umi_structured_form_answer_format(&f->schema, 2U, &answer, &rendered) == UMI_STATUS_OK &&
          strcmp(text.value, rendered.value) == 0);
  } else if (strcmp(name, "missing") == 0) {
    memset(&text, 0, sizeof(text));
    text.state = UMI_FORM_UNKNOWN;
    strcpy(text.reason, "Not measured");
    CHECK(umi_structured_form_answer_parse(&f->schema, 0U, &text, &answer) == UMI_STATUS_OK &&
          answer.value.type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE);
    CHECK(umi_structured_form_answer_format(&f->schema, 0U, &answer, &rendered) == UMI_STATUS_OK &&
          strcmp(rendered.reason, text.reason) == 0);
  } else if (strcmp(name, "unit") == 0) {
    strcpy(text.unit_id, "mass.lb");
    CHECK(umi_structured_form_answer_parse(&f->schema, 0U, &text, &answer) ==
              UMI_STATUS_INVALID_STATE &&
          memcmp(&answer, &original, sizeof(answer)) == 0);
  } else if (strcmp(name, "stale-storage") == 0) {
    text.state = UMI_FORM_UNKNOWN;
    strcpy(text.reason, "Unavailable");
    CHECK(umi_structured_form_answer_parse(&f->schema, 0U, &text, &answer) ==
              UMI_STATUS_INVALID_STATE &&
          memcmp(&answer, &original, sizeof(answer)) == 0);
  } else if (strcmp(name, "unterminated") == 0) {
    memset(text.value, 'x', sizeof(text.value));
    CHECK(umi_structured_form_answer_parse(&f->schema, 0U, &text, &answer) ==
              UMI_STATUS_INVALID_STATE &&
          memcmp(&answer, &original, sizeof(answer)) == 0);
  } else if (strcmp(name, "range") == 0) {
    strcpy(text.value, "101");
    CHECK(umi_structured_form_answer_parse(&f->schema, 0U, &text, &answer) != UMI_STATUS_OK &&
          memcmp(&answer, &original, sizeof(answer)) == 0);
  } else if (strcmp(name, "format") == 0) {
    text.unit_id[0] = '\0';
    strcpy(text.value, "2024-02-29T12:00:00");
    CHECK(umi_structured_form_answer_parse(&f->schema, 1U, &text, &answer) ==
          UMI_STATUS_PARSE_ERROR);
    strcpy(text.value, "2024-02-29T12:00:00Z");
    CHECK(umi_structured_form_answer_parse(&f->schema, 1U, &text, &answer) == UMI_STATUS_OK);
  } else if (strcmp(name, "capacity") == 0) {
    memset(&rendered, 0xA5, sizeof(rendered));
    UmiFormTextAnswer prior = rendered;
    CHECK(umi_structured_form_answer_format(&f->schema, 3U, &answer, &rendered) ==
              UMI_STATUS_INVALID_ARGUMENT &&
          memcmp(&rendered, &prior, sizeof(prior)) == 0);
  } else if (strcmp(name, "choice") == 0) {
    UmiApplicationExperienceFieldOption options[] = {{"ready", "Ready", 0},
                                                     {"closed", "Closed", 1}};
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE;
    f->fields[2].options = options;
    f->fields[2].option_count = 2U;
    text.unit_id[0] = '\0';
    strcpy(text.value, "ready");
    CHECK(umi_structured_form_answer_parse(&f->schema, 2U, &text, &answer) == UMI_STATUS_OK);
    strcpy(text.value, "closed");
    CHECK(umi_structured_form_answer_parse(&f->schema, 2U, &text, &answer) != UMI_STATUS_OK);
  } else
    CHECK(0);
  free(f);
  return 0;
}
