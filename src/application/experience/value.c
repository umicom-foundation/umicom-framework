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

/*
 * Release or reset state held by application experience value so the same storage can be
 * reused safely.
 */
void umi_application_experience_value_clear(UmiApplicationExperienceValue *value) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value != NULL)
    (void)memset(value, 0, sizeof(*value));
}

/*
 * Provide the application experience value boolean operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_boolean(UmiApplicationExperienceValue *value,
                                                   int boolean_value) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL || (boolean_value != 0 && boolean_value != 1))
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  value->type = UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN;
  value->boolean_value = boolean_value;
  return UMI_STATUS_OK;
}

/*
 * Provide the application experience value integer operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_integer(UmiApplicationExperienceValue *value,
                                                   int64_t integer_value) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  value->type = UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER;
  value->integer_value = integer_value;
  return UMI_STATUS_OK;
}

/*
 * Provide the application experience value decimal operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_decimal(UmiApplicationExperienceValue *value,
                                                   double decimal_value) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL || !isfinite(decimal_value))
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  value->type = UMI_APPLICATION_EXPERIENCE_VALUE_DECIMAL;
  value->decimal_value = decimal_value;
  return UMI_STATUS_OK;
}

/*
 * Provide the application experience value text operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_text(UmiApplicationExperienceValue *value,
                                                const char *text_value) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL || text_value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  umi_application_experience_value_clear(value);
  status = umi_text_copy(value->text_value, sizeof(value->text_value), text_value);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    value->type = UMI_APPLICATION_EXPERIENCE_VALUE_TEXT;
  return status;
}

/*
 * Check that application experience value satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_experience_value_validate(const UmiApplicationExperienceValue *value) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (value == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the application experience value equal operation used by this module and its
 * client applications.
 */
int umi_application_experience_value_equal(const UmiApplicationExperienceValue *left,
                                           const UmiApplicationExperienceValue *right) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (left == NULL || right == NULL || left->type != right->type)
    return 0;
  /* Select the behaviour associated with the requested command or state value. */
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

/*
 * Provide the application experience value empty operation used by this module and its
 * client applications.
 */
int umi_application_experience_value_empty(const UmiApplicationExperienceValue *value) {
  return value == NULL || value->type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE ||
         (value->type == UMI_APPLICATION_EXPERIENCE_VALUE_TEXT && value->text_value[0] == '\0');
}
