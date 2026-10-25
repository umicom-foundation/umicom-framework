/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_filter_text.c
 *
 * PURPOSE:
 *   Text filters preserve exact type, missing-state and numeric boundary meaning.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Text filters preserve exact type, missing-state and numeric boundary meaning. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/application/experience/structured_form_filter_text.h"
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
  const char *cases[] = {"decimal",           "integer-min",   "integer-max",  "integer-overflow",
                         "boolean",           "boolean-case",  "text",         "contains",
                         "outside-threshold", "outside-equal", "state",        "state-value",
                         "missing-value",     "unit",          "comma",        "nan",
                         "trailing",          "empty-number",  "field",        "field-termination",
                         "value-termination", "date",          "date-invalid", "alias-parse",
                         "alias-format",      "format-unit",   "null"};
  if (argc != 2)
    return 2;
  bool known = false;
  for (size_t i = 0; i < sizeof cases / sizeof cases[0]; ++i)
    if (strcmp(argv[1], cases[i]) == 0)
      known = true;
  if (!known)
    return 2;
  int result = 0;
  const char *name = argv[1];
  FormFixture *f = form_fixture_new();
  CHECK(f);
  UmiFormFilterTextTerm input = {.field_id = "parcel.mass",
                                 .operation = UMI_FORM_FILTER_EQUAL,
                                 .state = UMI_FORM_KNOWN,
                                 .value = "10.125"};
  UmiFormFilterTerm term, sentinel;
  memset(&sentinel, 0x6a, sizeof sentinel);
  term = sentinel;
  UmiFormFilterTextTerm formatted;
  bool invalid = false;
  if (strncmp(name, "integer-", 8) == 0) {
    f->fields[0].kind = UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER;
    f->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    f->fields[0].enforce_number_range = 0;
    strcpy(input.value, strcmp(name, "integer-min") == 0   ? "-9223372036854775808"
                        : strcmp(name, "integer-max") == 0 ? "9223372036854775807"
                                                           : "9223372036854775808");
    invalid = strcmp(name, "integer-overflow") == 0;
  }
  if (strncmp(name, "boolean", 7) == 0) {
    f->fields[0].kind = UMI_APPLICATION_EXPERIENCE_FIELD_BOOLEAN;
    f->fields[0].value_type = UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN;
    f->fields[0].enforce_number_range = 0;
    f->rules[0].unit_id = "";
    strcpy(input.value, strcmp(name, "boolean") == 0 ? "false" : "False");
    invalid = strcmp(name, "boolean-case") == 0;
  }
  if (strcmp(name, "text") == 0 || strcmp(name, "contains") == 0) {
    strcpy(input.field_id, "parcel.note");
    strcpy(input.value, "parcel");
    if (strcmp(name, "contains") == 0)
      input.operation = UMI_FORM_FILTER_CONTAINS;
  }
  if (strcmp(name, "outside-threshold") == 0 || strcmp(name, "outside-equal") == 0) {
    strcpy(input.value, "1000");
    input.operation =
        strcmp(name, "outside-threshold") == 0 ? UMI_FORM_FILTER_LESS : UMI_FORM_FILTER_EQUAL;
    invalid = strcmp(name, "outside-equal") == 0;
  }
  if (strcmp(name, "state") == 0 || strcmp(name, "state-value") == 0) {
    input.operation = UMI_FORM_FILTER_STATE;
    input.state = UMI_FORM_UNKNOWN;
    if (strcmp(name, "state") == 0)
      input.value[0] = '\0';
    else
      invalid = true;
  }
  if (strcmp(name, "missing-value") == 0) {
    input.state = UMI_FORM_UNKNOWN;
    invalid = true;
  }
  if (strcmp(name, "comma") == 0) {
    strcpy(input.value, "10,125");
    invalid = true;
  }
  if (strcmp(name, "nan") == 0) {
    strcpy(input.value, "nan");
    invalid = true;
  }
  if (strcmp(name, "trailing") == 0) {
    strcpy(input.value, "10.125kg");
    invalid = true;
  }
  if (strcmp(name, "empty-number") == 0) {
    input.value[0] = '\0';
    invalid = true;
  }
  if (strcmp(name, "field") == 0) {
    strcpy(input.field_id, "missing");
    invalid = true;
  }
  if (strcmp(name, "field-termination") == 0) {
    memset(input.field_id, 'x', sizeof input.field_id);
    invalid = true;
  }
  if (strcmp(name, "value-termination") == 0) {
    memset(input.value, 'x', sizeof input.value);
    invalid = true;
  }
  if (strcmp(name, "date") == 0 || strcmp(name, "date-invalid") == 0) {
    strcpy(input.field_id, "parcel.observed");
    strcpy(input.value,
           strcmp(name, "date") == 0 ? "2026-10-24T12:30:00+00:00" : "2026-10-24T12:30:00");
    invalid = strcmp(name, "date-invalid") == 0;
  }
  if (strcmp(name, "null") == 0) {
    CHECK(umi_structured_form_filter_term_parse(NULL, &input, &term) != UMI_STATUS_OK);
    CHECK(umi_structured_form_filter_term_parse(&f->schema, NULL, &term) != UMI_STATUS_OK);
    CHECK(umi_structured_form_filter_term_parse(&f->schema, &input, NULL) != UMI_STATUS_OK);
    CHECK(umi_structured_form_filter_term_format(&f->schema, NULL, &formatted) != UMI_STATUS_OK);
    CHECK(memcmp(&term, &sentinel, sizeof term) == 0);
    goto cleanup;
  }
  UmiStatus status = umi_structured_form_filter_term_parse(&f->schema, &input, &term);
  if (invalid) {
    CHECK(status != UMI_STATUS_OK && memcmp(&term, &sentinel, sizeof term) == 0);
    goto cleanup;
  }
  OK(status);
  if (strcmp(name, "unit") == 0)
    CHECK(strcmp(term.operand.unit_id, "mass.kg") == 0);
  if (strcmp(name, "format-unit") == 0) {
    strcpy(term.operand.unit_id, "mass.lb");
    memset(&formatted, 0x4d, sizeof formatted);
    UmiFormFilterTextTerm before = formatted;
    CHECK(umi_structured_form_filter_term_format(&f->schema, &term, &formatted) != UMI_STATUS_OK);
    CHECK(memcmp(&before, &formatted, sizeof formatted) == 0);
    goto cleanup;
  }
  OK(umi_structured_form_filter_term_format(&f->schema, &term, &formatted));
  UmiFormFilterTerm second;
  OK(umi_structured_form_filter_term_parse(&f->schema, &formatted, &second));
  CHECK(term.operation == second.operation && term.operand.state == second.operand.state);
  CHECK(umi_application_experience_value_equal(&term.operand.value, &second.operand.value));
  if (strcmp(name, "alias-parse") == 0 || strcmp(name, "alias-format") == 0) {
    union {
      UmiFormFilterTerm term;
      UmiFormFilterTextTerm text;
    } alias;
    if (strcmp(name, "alias-parse") == 0) {
      alias.text = input;
      OK(umi_structured_form_filter_term_parse(&f->schema, &alias.text, &alias.term));
      CHECK(umi_application_experience_value_equal(&term.operand.value, &alias.term.operand.value));
    } else {
      alias.term = term;
      OK(umi_structured_form_filter_term_format(&f->schema, &alias.term, &alias.text));
      CHECK(strcmp(alias.text.value, formatted.value) == 0);
    }
  }
cleanup:
  free(f);
  return result;
}
