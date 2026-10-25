/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_structured_form_filter.c
 *
 * PURPOSE:
 *   Filters compare typed answers, never locale text or silently converted units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Filters compare typed answers, never locale text or silently converted units. */
#include "structured_form_fixture.h"
#undef CHECK
#include "umicom/application/experience/structured_form_filter.h"
#include <math.h>
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
                         "equal",
                         "unequal",
                         "less",
                         "less-equal",
                         "greater",
                         "greater-equal",
                         "outside-range",
                         "contains",
                         "contains-case",
                         "empty-needle",
                         "boolean",
                         "integer-min",
                         "integer-max",
                         "unanswered",
                         "unknown",
                         "not-applicable",
                         "missing-value",
                         "all-terms",
                         "later-invalid",
                         "field",
                         "unit",
                         "type",
                         "nan",
                         "infinite",
                         "state-value",
                         "state-unit",
                         "state-reason",
                         "count",
                         "operator",
                         "unterminated-field",
                         "unterminated-text",
                         "unterminated-unit",
                         "date-contains",
                         "choice-contains",
                         "ordered-text",
                         "invalid-form",
                         "identity",
                         "null",
                         "digest-stable",
                         "digest-inactive",
                         "digest-value",
                         "digest-version",
                         "digest-order"};
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
  UmiStructuredFormFilter *filter = calloc(1, sizeof *filter), *other = calloc(1, sizeof *other);
  CHECK(f && filter && other);
  filter->count = 1;
  UmiFormFilterTerm *term = &filter->terms[0];
  strcpy(term->field_id, "parcel.mass");
  term->operation = UMI_FORM_FILTER_EQUAL;
  term->operand = form_mass(5);
  UmiFormAnswer answer = form_mass(5);
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.mass", &answer));
  answer = form_text("Blue parcel");
  OK(umi_structured_form_set(&f->schema, &f->before, f->before.revision, "parcel.note", &answer));
  bool expected = true, invalid = false;
  if (strcmp(name, "empty") == 0)
    filter->count = 0;
  if (strcmp(name, "unequal") == 0) {
    term->operand = form_mass(6);
    expected = false;
  }
  if (strcmp(name, "less") == 0) {
    term->operation = UMI_FORM_FILTER_LESS;
    term->operand = form_mass(6);
  }
  if (strcmp(name, "less-equal") == 0)
    term->operation = UMI_FORM_FILTER_LESS_EQUAL;
  if (strcmp(name, "greater") == 0) {
    term->operation = UMI_FORM_FILTER_GREATER;
    term->operand = form_mass(4);
  }
  if (strcmp(name, "greater-equal") == 0)
    term->operation = UMI_FORM_FILTER_GREATER_EQUAL;
  if (strcmp(name, "outside-range") == 0) {
    term->operation = UMI_FORM_FILTER_LESS;
    term->operand = form_mass(1000);
  }
  if (strcmp(name, "contains") == 0 || strcmp(name, "contains-case") == 0 ||
      strcmp(name, "empty-needle") == 0) {
    strcpy(term->field_id, "parcel.note");
    term->operation = UMI_FORM_FILTER_CONTAINS;
    term->operand = form_text(strcmp(name, "contains-case") == 0  ? "blue"
                              : strcmp(name, "empty-needle") == 0 ? ""
                                                                  : "parcel");
    expected = strcmp(name, "contains-case") != 0;
  }
  if (strcmp(name, "boolean") == 0 || strcmp(name, "integer-min") == 0 ||
      strcmp(name, "integer-max") == 0) {
    bool boolean = strcmp(name, "boolean") == 0;
    f->fields[0].kind = boolean ? UMI_APPLICATION_EXPERIENCE_FIELD_BOOLEAN
                                : UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER;
    f->fields[0].value_type = boolean ? UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN
                                      : UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    f->fields[0].enforce_number_range = 0;
    f->before.answers[0].value.type = f->fields[0].value_type;
    term->operand.value.type = f->fields[0].value_type;
    if (boolean) {
      /* Boolean answers are unitless; preserve the numeric unit only in integer fixtures. */
      f->rules[0].unit_id = "";
      f->before.answers[0].unit_id[0] = term->operand.unit_id[0] = '\0';
      f->before.answers[0].value.boolean_value = term->operand.value.boolean_value = 0;
    } else {
      bool minimum = strcmp(name, "integer-min") == 0;
      f->before.answers[0].value.integer_value = minimum ? INT64_MIN : INT64_MAX;
      term->operand.value.integer_value = minimum ? INT64_MAX : INT64_MIN;
      term->operation = minimum ? UMI_FORM_FILTER_LESS : UMI_FORM_FILTER_GREATER;
    }
  }
  if (strcmp(name, "unanswered") == 0 || strcmp(name, "unknown") == 0 ||
      strcmp(name, "not-applicable") == 0 || strcmp(name, "missing-value") == 0) {
    UmiFormAnswerState state = strcmp(name, "unanswered") == 0       ? UMI_FORM_UNANSWERED
                               : strcmp(name, "not-applicable") == 0 ? UMI_FORM_NOT_APPLICABLE
                                                                     : UMI_FORM_UNKNOWN;
    strcpy(term->field_id, "parcel.note");
    f->before.answers[2] = form_missing(state, state == UMI_FORM_UNANSWERED ? "" : "Not supplied");
    if (strcmp(name, "missing-value") == 0) {
      term->operand = form_text("Blue parcel");
      expected = false;
    } else {
      memset(&term->operand, 0, sizeof term->operand);
      term->operand.state = state;
      term->operation = UMI_FORM_FILTER_STATE;
    }
  }
  if (strcmp(name, "all-terms") == 0 || strcmp(name, "later-invalid") == 0) {
    filter->count = 2;
    filter->terms[1] = *term;
    term->operand = form_mass(6);
    expected = false;
    if (strcmp(name, "later-invalid") == 0) {
      strcpy(filter->terms[1].field_id, "missing");
      invalid = true;
    }
  }
  if (strcmp(name, "field") == 0) {
    strcpy(term->field_id, "missing");
    invalid = true;
  }
  if (strcmp(name, "unit") == 0) {
    strcpy(term->operand.unit_id, "mass.lb");
    invalid = true;
  }
  if (strcmp(name, "type") == 0) {
    term->operand.value.type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    invalid = true;
  }
  if (strcmp(name, "nan") == 0 || strcmp(name, "infinite") == 0) {
    term->operand.value.decimal_value = strcmp(name, "nan") == 0 ? NAN : INFINITY;
    invalid = true;
  }
  if (strncmp(name, "state-", 6) == 0) {
    term->operation = UMI_FORM_FILTER_STATE;
    memset(&term->operand, 0, sizeof term->operand);
    term->operand.state = UMI_FORM_KNOWN;
    if (strcmp(name, "state-value") == 0)
      term->operand.value.type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
    if (strcmp(name, "state-unit") == 0)
      strcpy(term->operand.unit_id, "mass.kg");
    if (strcmp(name, "state-reason") == 0)
      strcpy(term->operand.reason, "unused");
    invalid = true;
  }
  if (strcmp(name, "count") == 0) {
    filter->count = UMI_FORM_FILTER_CAPACITY + 1;
    invalid = true;
  }
  if (strcmp(name, "operator") == 0) {
    term->operation = (UmiFormFilterOperator)99;
    invalid = true;
  }
  if (strcmp(name, "unterminated-field") == 0) {
    memset(term->field_id, 'x', sizeof term->field_id);
    invalid = true;
  }
  if (strcmp(name, "unterminated-unit") == 0) {
    memset(term->operand.unit_id, 'x', sizeof term->operand.unit_id);
    invalid = true;
  }
  if (strcmp(name, "unterminated-text") == 0) {
    strcpy(term->field_id, "parcel.note");
    term->operand = form_text("x");
    memset(term->operand.value.text_value, 'x', sizeof term->operand.value.text_value);
    invalid = true;
  }
  if (strcmp(name, "date-contains") == 0) {
    strcpy(term->field_id, "parcel.observed");
    term->operation = UMI_FORM_FILTER_CONTAINS;
    term->operand = form_text("2026");
    invalid = true;
  }
  static const UmiApplicationExperienceFieldOption choices[] = {{"Blue parcel", "Blue parcel", 0}};
  if (strcmp(name, "choice-contains") == 0) {
    f->fields[2].kind = UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE;
    f->fields[2].options = choices;
    f->fields[2].option_count = 1;
    strcpy(term->field_id, "parcel.note");
    term->operation = UMI_FORM_FILTER_CONTAINS;
    term->operand = form_text("Blue");
    invalid = true;
  }
  if (strcmp(name, "ordered-text") == 0) {
    strcpy(term->field_id, "parcel.note");
    term->operation = UMI_FORM_FILTER_LESS;
    term->operand = form_text("Blue");
    invalid = true;
  }
  if (strcmp(name, "invalid-form") == 0) {
    strcpy(f->before.answers[0].unit_id, "mass.lb");
    invalid = true;
  }
  if (strcmp(name, "identity") == 0) {
    f->before.schema_version = 99;
    invalid = true;
  }
  if (strcmp(name, "null") == 0) {
    bool matched = true;
    CHECK(umi_structured_form_filter_match(&f->schema, &f->before, NULL, &matched) !=
              UMI_STATUS_OK &&
          matched);
    CHECK(umi_structured_form_filter_match(&f->schema, &f->before, filter, NULL) != UMI_STATUS_OK);
    CHECK(umi_structured_form_filter_digest(NULL, filter, NULL) != UMI_STATUS_OK);
    goto cleanup;
  }
  if (strncmp(name, "digest-", 7) == 0) {
    char first[65], second[65];
    *other = *filter;
    if (strcmp(name, "digest-order") == 0) {
      filter->count = 2;
      filter->terms[1] = *term;
      filter->terms[1].operand = form_mass(6);
      *other = *filter;
      other->terms[0] = filter->terms[1];
      other->terms[1] = filter->terms[0];
    }
    OK(umi_structured_form_filter_digest(&f->schema, filter, first));
    if (strcmp(name, "digest-inactive") == 0) {
      memset(&other->terms[7], 0x7f, sizeof other->terms[7]);
      other->terms[0].operand.value.integer_value = 77;
    }
    if (strcmp(name, "digest-value") == 0)
      other->terms[0].operand = form_mass(9);
    if (strcmp(name, "digest-version") == 0)
      ++f->schema.version;
    OK(umi_structured_form_filter_digest(&f->schema, other, second));
    CHECK((strcmp(first, second) == 0) ==
          (strcmp(name, "digest-stable") == 0 || strcmp(name, "digest-inactive") == 0));
    goto cleanup;
  }
  bool matched = true;
  UmiStatus status = umi_structured_form_filter_match(&f->schema, &f->before, filter, &matched);
  if (invalid)
    CHECK(status != UMI_STATUS_OK && matched);
  else {
    OK(status);
    CHECK(matched == expected);
  }
cleanup:
  free(other);
  free(filter);
  free(f);
  return result;
}
