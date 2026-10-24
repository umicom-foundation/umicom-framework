/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/structured_form_text.c
 * PURPOSE: Centralise answer conversion so every frontend enforces the same policy.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/structured_form_text.h"
#include "umicom/base/ascii_number.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_structured_form_answer_parse(const UmiStructuredFormSchema *schema, size_t index,
                                           const UmiFormTextAnswer *text, UmiFormAnswer *out) {
  UmiStatus status = umi_structured_form_schema_validate(schema);
  if (status != UMI_STATUS_OK)
    return status;
  if (text == NULL || out == NULL || index >= schema->rule_count)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (memchr(text->value, '\0', sizeof(text->value)) == NULL ||
      memchr(text->unit_id, '\0', sizeof(text->unit_id)) == NULL ||
      memchr(text->reason, '\0', sizeof(text->reason)) == NULL)
    return UMI_STATUS_INVALID_STATE;
  UmiFormAnswer candidate = {0};
  candidate.state = text->state;
  memcpy(candidate.unit_id, text->unit_id, strlen(text->unit_id) + 1U);
  memcpy(candidate.reason, text->reason, strlen(text->reason) + 1U);
  if (text->state == UMI_FORM_KNOWN) {
    switch (schema->base->fields[index].value_type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      status = umi_application_experience_value_text(&candidate.value, text->value);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
      if (strcmp(text->value, "true") != 0 && strcmp(text->value, "false") != 0)
        return UMI_STATUS_PARSE_ERROR;
      status = umi_application_experience_value_boolean(&candidate.value,
                                                        strcmp(text->value, "true") == 0);
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER: {
      int64_t value;
      status = umi_ascii_int64_parse(text->value, strlen(text->value), &value);
      if (status == UMI_STATUS_OK)
        status = umi_application_experience_value_integer(&candidate.value, value);
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL: {
      double value;
      status = umi_ascii_double_parse(text->value, strlen(text->value), &value);
      if (status == UMI_STATUS_OK)
        status = umi_application_experience_value_decimal(&candidate.value, value);
      break;
    }
    default:
      return UMI_STATUS_INVALID_STATE;
    }
    if (status != UMI_STATUS_OK)
      return status;
  } else if (text->value[0] != '\0')
    return UMI_STATUS_INVALID_STATE;
  status = umi_structured_form_answer_validate(schema, index, &candidate, 0);
  if (status == UMI_STATUS_OK)
    *out = candidate;
  return status;
}
UmiStatus umi_structured_form_answer_format(const UmiStructuredFormSchema *schema, size_t index,
                                            const UmiFormAnswer *answer, UmiFormTextAnswer *out) {
  if (out == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  UmiStatus status = umi_structured_form_answer_validate(schema, index, answer, 0);
  if (status != UMI_STATUS_OK)
    return status;
  UmiFormTextAnswer candidate = {0};
  candidate.state = answer->state;
  memcpy(candidate.unit_id, answer->unit_id, strlen(answer->unit_id) + 1U);
  memcpy(candidate.reason, answer->reason, strlen(answer->reason) + 1U);
  if (answer->state == UMI_FORM_KNOWN) {
    switch (answer->value.type) {
    case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
      strcpy(candidate.value, answer->value.boolean_value ? "true" : "false");
      break;
    case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER: {
      int length = snprintf(candidate.value, sizeof(candidate.value), "%lld",
                            (long long)answer->value.integer_value);
      if (length < 0 || (size_t)length >= sizeof(candidate.value))
        return UMI_STATUS_INTERNAL_ERROR;
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL: {
      size_t length;
      status = umi_ascii_double_format(answer->value.decimal_value, candidate.value,
                                       sizeof(candidate.value), &length);
      if (status != UMI_STATUS_OK)
        return status;
      break;
    }
    case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
      memcpy(candidate.value, answer->value.text_value, strlen(answer->value.text_value) + 1U);
      break;
    default:
      return UMI_STATUS_INVALID_STATE;
    }
  }
  *out = candidate;
  return UMI_STATUS_OK;
}
