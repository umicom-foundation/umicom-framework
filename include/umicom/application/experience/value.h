/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/value.h
 *
 * PURPOSE:
 *   Store typed command and form values without exposing a graphical toolkit
 *   or requiring heap allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_VALUE_H
#define UMICOM_APPLICATION_EXPERIENCE_VALUE_H

#include "umicom/application/experience/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience value data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceValue {
  UmiApplicationExperienceValueType type;
  int boolean_value;
  int64_t integer_value;
  double decimal_value;
  char text_value[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
} UmiApplicationExperienceValue;

/**
 * Release or reset state held by application experience value so the same storage can be
 * reused safely.
 */
void umi_application_experience_value_clear(UmiApplicationExperienceValue *value);
/**
 * Provide the application experience value boolean operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_boolean(UmiApplicationExperienceValue *value,
                                                   int boolean_value);
/**
 * Provide the application experience value integer operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_integer(UmiApplicationExperienceValue *value,
                                                   int64_t integer_value);
/**
 * Provide the application experience value decimal operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_decimal(UmiApplicationExperienceValue *value,
                                                   double decimal_value);
/**
 * Provide the application experience value text operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_value_text(UmiApplicationExperienceValue *value,
                                                const char *text_value);
/**
 * Check that application experience value satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_experience_value_validate(const UmiApplicationExperienceValue *value);
/**
 * Provide the application experience value equal operation used by this module and its
 * client applications.
 */
int umi_application_experience_value_equal(const UmiApplicationExperienceValue *left,
                                           const UmiApplicationExperienceValue *right);
/**
 * Provide the application experience value empty operation used by this module and its
 * client applications.
 */
int umi_application_experience_value_empty(const UmiApplicationExperienceValue *value);

#ifdef __cplusplus
}
#endif

#endif
