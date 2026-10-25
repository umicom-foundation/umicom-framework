/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_filter_text.c
 *
 * PURPOSE:
 *   Human input is parsed before it becomes a typed query; locale never changes it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Human input is parsed before it becomes a typed query; locale never changes it. */
#include "umicom/application/experience/structured_form_filter_text.h"
#include "umicom/base/ascii_number.h"
#include <stdio.h>
#include <string.h>
static UmiStatus validate_one(const UmiStructuredFormSchema *schema,
                              const UmiFormFilterTerm *term) {
  UmiStructuredFormFilter filter = {.count = 1};
  filter.terms[0] = *term;
  return umi_structured_form_filter_validate(schema, &filter);
}
UmiStatus umi_structured_form_filter_term_parse(const UmiStructuredFormSchema *schema,
                                                const UmiFormFilterTextTerm *text,
                                                UmiFormFilterTerm *out) {
  if (!text || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (!memchr(text->field_id, '\0', sizeof text->field_id) ||
      !memchr(text->value, '\0', sizeof text->value))
    return UMI_STATUS_INVALID_ARGUMENT;
  size_t index = umi_application_experience_form_schema_index(schema->base, text->field_id);
  if (index == SIZE_MAX)
    return UMI_STATUS_NOT_FOUND;
  UmiFormFilterTerm candidate = {.operation = text->operation, .operand = {.state = text->state}};
  memcpy(candidate.field_id, text->field_id, strlen(text->field_id) + 1);
  if (text->operation == UMI_FORM_FILTER_STATE) {
    if (text->value[0])
      return UMI_STATUS_INVALID_ARGUMENT;
  } else {
    if (text->state != UMI_FORM_KNOWN)
      return UMI_STATUS_INVALID_ARGUMENT;
    memcpy(candidate.operand.unit_id, schema->rules[index].unit_id,
           strlen(schema->rules[index].unit_id) + 1);
    switch (schema->base->fields[index].value_type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      status = umi_application_experience_value_text(&candidate.operand.value, text->value);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
      if (strcmp(text->value, "true") != 0 && strcmp(text->value, "false") != 0)
        return UMI_STATUS_PARSE_ERROR;
      status = umi_application_experience_value_boolean(&candidate.operand.value,
                                                        strcmp(text->value, "true") == 0);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER: {
      int64_t value;
      status = umi_ascii_int64_parse(text->value, strlen(text->value), &value);
      if (status == UMI_STATUS_OK)
        status = umi_application_experience_value_integer(&candidate.operand.value, value);
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL: {
      double value;
      status = umi_ascii_double_parse(text->value, strlen(text->value), &value);
      if (status == UMI_STATUS_OK)
        status = umi_application_experience_value_decimal(&candidate.operand.value, value);
      break;
    }
    default:
      status = UMI_STATUS_INVALID_ARGUMENT;
      break;
    }
  }
  if (status == UMI_STATUS_OK)
    status = validate_one(schema, &candidate);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  return status;
}
UmiStatus umi_structured_form_filter_term_format(const UmiStructuredFormSchema *schema,
                                                 const UmiFormFilterTerm *term,
                                                 UmiFormFilterTextTerm *out) {
  if (!term || !out)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = validate_one(schema, term);
  if (status != UMI_STATUS_OK)
    return status;
  UmiFormFilterTextTerm candidate = {.operation = term->operation, .state = term->operand.state};
  memcpy(candidate.field_id, term->field_id, strlen(term->field_id) + 1);
  if (term->operation != UMI_FORM_FILTER_STATE) {
    const UmiApplicationExperienceValue *value = &term->operand.value;
    switch (value->type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      memcpy(candidate.value, value->text_value, strlen(value->text_value) + 1);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
      strcpy(candidate.value, value->boolean_value ? "true" : "false");
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER: {
      int n = snprintf(candidate.value, sizeof candidate.value, "%lld",
                       (long long)value->integer_value);
      if (n < 0 || (size_t)n >= sizeof candidate.value)
        status = UMI_STATUS_CAPACITY_EXCEEDED;
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL: {
      size_t n;
      status = umi_ascii_double_format(value->decimal_value, candidate.value,
                                       sizeof candidate.value, &n);
      break;
    }
    default:
      status = UMI_STATUS_INVALID_STATE;
      break;
    }
  }
  if (status == UMI_STATUS_OK)
    *out = candidate;
  return status;
}
