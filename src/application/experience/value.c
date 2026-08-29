/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/value.c
 *
 * PURPOSE:
 *   Implement bounded typed values shared by command parameters and form
 *   fields with deterministic validation and equality behavior.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/value.h"

#include <math.h>
#include <string.h>

#include "umicom/base/text.h"

void umi_application_experience_value_clear(UmiApplicationExperienceValue *value) {
  if (value != NULL)
    (void)memset(value, 0, sizeof(*value));
}

UmiStatus umi_application_experience_value_boolean(UmiApplicationExperienceValue *value,
                                                   int boolean_value) {
  if (value == NULL || (boolean_value != 0 && boolean_value != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  value->type = UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN;
  value->boolean_value = boolean_value;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_experience_value_integer(UmiApplicationExperienceValue *value,
                                                   int64_t integer_value) {
  if (value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  value->type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
  value->integer_value = integer_value;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_experience_value_decimal(UmiApplicationExperienceValue *value,
                                                   double decimal_value) {
  if (value == NULL || !isfinite(decimal_value))
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  value->type = UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL;
  value->decimal_value = decimal_value;
  return UMI_STATUS_OK;
}

UmiStatus umi_application_experience_value_text(UmiApplicationExperienceValue *value,
                                                const char *text_value) {
  UmiStatus status;
  if (value == NULL || text_value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  status = umi_text_copy(value->text_value, sizeof(value->text_value), text_value);
  if (status == UMI_STATUS_OK)
    value->type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT;
  return status;
}

UmiStatus umi_application_experience_value_validate(const UmiApplicationExperienceValue *value) {
  if (value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  switch (value->type) {
  case UMI_APPLICATION_EXPERIENCE_VALUE_NONE:
  case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER:
    return UMI_STATUS_OK;
  case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
    return value->boolean_value == 0 || value->boolean_value == 1 ? UMI_STATUS_OK
                                                                  : UMI_STATUS_INVALID_STATE;
  case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL:
    return isfinite(value->decimal_value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
  case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
    return memchr(value->text_value, '\0', sizeof(value->text_value)) != NULL
               ? UMI_STATUS_OK
               : UMI_STATUS_INVALID_STATE;
  default:
    return UMI_STATUS_INVALID_STATE;
  }
}

int umi_application_experience_value_equal(const UmiApplicationExperienceValue *left,
                                           const UmiApplicationExperienceValue *right) {
  if (left == NULL || right == NULL || left->type != right->type)
    return 0;
  switch (left->type) {
  case UMI_APPLICATION_EXPERIENCE_VALUE_NONE:
    return 1;
  case UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN:
    return left->boolean_value == right->boolean_value;
  case UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER:
    return left->integer_value == right->integer_value;
  case UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL:
    return left->decimal_value == right->decimal_value;
  case UMI_APPLICATION_EXPERIENCE_VALUE_TEXT:
    return strcmp(left->text_value, right->text_value) == 0;
  default:
    return 0;
  }
}

int umi_application_experience_value_empty(const UmiApplicationExperienceValue *value) {
  return value == NULL || value->type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE ||
         (value->type == UMI_APPLICATION_EXPERIENCE_VALUE_TEXT && value->text_value[0] == '\0');
}
