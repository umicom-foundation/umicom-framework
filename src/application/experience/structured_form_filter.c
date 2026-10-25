/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_filter.c
 *
 * PURPOSE:
 *   Filter boundaries validate meaning before comparisons and never coerce units.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Filter boundaries validate meaning before comparisons and never coerce units. */
#include "umicom/application/experience/structured_form_filter.h"
#include "umicom/base/ascii_number.h"
#include "umicom/base/sha256.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static UmiStatus term_validate(const UmiStructuredFormSchema *schema,
                               const UmiFormFilterTerm *term) {
  if (!memchr(term->field_id, '\0', sizeof term->field_id) || !term->field_id[0])
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = umi_application_experience_form_schema_index(schema->base, term->field_id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  const UmiFormAnswer *value = &term->operand;
  if (!memchr(value->unit_id, '\0', sizeof value->unit_id) ||
      !memchr(value->reason, '\0', sizeof value->reason))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (value->state < UMI_FORM_UNANSWERED || value->state > UMI_FORM_NOT_APPLICABLE)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_application_experience_value_validate(&value->value);
  if (status != UMI_STATUS_OK)
    return status;
  if (term->operation == UMI_FORM_FILTER_STATE)
    return value->value.type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE && !value->unit_id[0] &&
                   !value->reason[0]
               ? UMI_STATUS_OK
               : UMI_STATUS_INVALID_ARGUMENT;
  if (value->state != UMI_FORM_KNOWN || value->reason[0] ||
      value->value.type != schema->base->fields[index].value_type ||
      strcmp(value->unit_id, schema->rules[index].unit_id) != 0)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (term->operation == UMI_FORM_FILTER_EQUAL)
    return umi_structured_form_answer_validate(schema, index, value, 0);
  if (term->operation == UMI_FORM_FILTER_CONTAINS) {
    UmiApplicationExperienceFieldKind kind = schema->base->fields[index].kind;
    return value->value.type == UMI_APPLICATION_EXPERIENCE_VALUE_TEXT &&
                   (kind == UMI_APPLICATION_EXPERIENCE_FIELD_TEXT ||
                    kind == UMI_APPLICATION_EXPERIENCE_FIELD_MULTILINE) &&
                   schema->rules[index].text_format == UMI_FORM_TEXT_PLAIN
               ? UMI_STATUS_OK
               : UMI_STATUS_INVALID_ARGUMENT;
  }
  if (term->operation < UMI_FORM_FILTER_LESS || term->operation > UMI_FORM_FILTER_GREATER_EQUAL)
    return UMI_STATUS_INVALID_ARGUMENT;
  return value->value.type == UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER ||
                 value->value.type == UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_ARGUMENT;
}
UmiStatus umi_structured_form_filter_validate(const UmiStructuredFormSchema *schema,
                                              const UmiStructuredFormFilter *filter) {
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (!filter || filter->count > UMI_FORM_FILTER_CAPACITY)
    return UMI_STATUS_INVALID_ARGUMENT;
  for (size_t i = 0; i < filter->count; ++i) {
    status = term_validate(schema, &filter->terms[i]);
    if (status != UMI_STATUS_OK)
      return status;
  }
  return UMI_STATUS_OK;
}
static bool term_matches(const UmiFormAnswer *answer, const UmiFormFilterTerm *term) {
  if (term->operation == UMI_FORM_FILTER_STATE)
    return answer->state == term->operand.state;
  if (answer->state != UMI_FORM_KNOWN)
    return false;
  const UmiApplicationExperienceValue *left = &answer->value, *right = &term->operand.value;
  if (term->operation == UMI_FORM_FILTER_EQUAL)
    return umi_application_experience_value_equal(left, right) != 0;
  if (term->operation == UMI_FORM_FILTER_CONTAINS)
    return strstr(left->text_value, right->text_value) != NULL;
  /* Ordered comparisons never subtract large integers and never convert them
   * through binary floating point. Operand validation excludes NaN/infinity. */
  int order = left->type == UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER
                  ? (left->integer_value > right->integer_value) -
                        (left->integer_value < right->integer_value)
                  : (left->decimal_value > right->decimal_value) -
                        (left->decimal_value < right->decimal_value);
  switch (term->operation) {
  case UMI_FORM_FILTER_LESS:
    return order < 0;
  case UMI_FORM_FILTER_LESS_EQUAL:
    return order <= 0;
  case UMI_FORM_FILTER_GREATER:
    return order > 0;
  case UMI_FORM_FILTER_GREATER_EQUAL:
    return order >= 0;
  default:
    return false;
  }
}
UmiStatus umi_structured_form_filter_match(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *form,
                                           const UmiStructuredFormFilter *filter, bool *out) {
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_filter_validate(schema, filter);
  if (status != UMI_STATUS_OK)
    return status;
  UmiApplicationExperienceValidationReport *report = malloc(sizeof *report);
  if (!report)
    return UMI_STATUS_OUT_OF_MEMORY;
  status = umi_structured_form_validate(schema, form, 0, report);
  if (status == UMI_STATUS_OK && !umi_application_experience_validation_report_passed(report))
    status = UMI_STATUS_INVALID_STATE;
  free(report);
  if (status != UMI_STATUS_OK)
    return status;
  bool matched = true;
  for (size_t i = 0; i < filter->count; ++i) {
    size_t index =
        umi_application_experience_form_schema_index(schema->base, filter->terms[i].field_id);
    if (!term_matches(&form->answers[index], &filter->terms[i]))
      matched = false;
  }
  *out = matched;
  return UMI_STATUS_OK;
}
static UmiStatus hash_text(UmiSha256 *hash, const char *text) {
  return UmiSha256Update(hash, text, strlen(text) + 1);
}
UmiStatus umi_structured_form_filter_digest(const UmiStructuredFormSchema *schema,
                                            const UmiStructuredFormFilter *filter, char out[65]) {
  if (!out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_filter_validate(schema, filter);
  if (status != UMI_STATUS_OK)
    return status;
  UmiSha256 hash;
  UmiSha256Init(&hash);
  status = hash_text(&hash, "Umicom typed answer filter 1");
  if (status == UMI_STATUS_OK)
    status = hash_text(&hash, schema->base->form_id);
  char number[128];
  (void)snprintf(number, sizeof number, "%llu", (unsigned long long)schema->version);
  if (status == UMI_STATUS_OK)
    status = hash_text(&hash, number);
  for (size_t i = 0; status == UMI_STATUS_OK && i < filter->count; ++i) {
    const UmiFormFilterTerm *term = &filter->terms[i];
    size_t index = umi_application_experience_form_schema_index(schema->base, term->field_id);
    status = hash_text(&hash, term->field_id);
    (void)snprintf(number, sizeof number, "%u:%u:%u:%u", (unsigned)term->operation,
                   (unsigned)term->operand.state, (unsigned)schema->base->fields[index].value_type,
                   (unsigned)schema->rules[index].text_format);
    if (status == UMI_STATUS_OK)
      status = hash_text(&hash, number);
    if (status == UMI_STATUS_OK)
      status = hash_text(&hash, schema->rules[index].unit_id);
    if (status != UMI_STATUS_OK || term->operation == UMI_FORM_FILTER_STATE)
      continue;
    const UmiApplicationExperienceValue *value = &term->operand.value;
    const char *text = number;
    switch (value->type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
      text = value->boolean_value ? "true" : "false";
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER:
      (void)snprintf(number, sizeof number, "%lld", (long long)value->integer_value);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL: {
      size_t n;
      status = umi_ascii_double_format(value->decimal_value, number, sizeof number, &n);
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      text = value->text_value;
      break;
    default:
      status = UMI_STATUS_INVALID_STATE;
      break;
    }
    if (status == UMI_STATUS_OK)
      status = hash_text(&hash, text);
  }
  unsigned char bytes[UMI_SHA256_BYTES];
  if (status == UMI_STATUS_OK)
    status = UmiSha256Final(&hash, bytes);
  if (status == UMI_STATUS_OK)
    UmiSha256Hex(bytes, out);
  return status;
}
