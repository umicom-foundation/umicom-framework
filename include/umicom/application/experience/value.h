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

typedef struct UmiApplicationExperienceValue {
  UmiApplicationExperienceValueType type;
  int boolean_value;
  int64_t integer_value;
  double decimal_value;
  char text_value[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
} UmiApplicationExperienceValue;

void umi_application_experience_value_clear(UmiApplicationExperienceValue *value);
UmiStatus umi_application_experience_value_boolean(UmiApplicationExperienceValue *value,
                                                   int boolean_value);
UmiStatus umi_application_experience_value_integer(UmiApplicationExperienceValue *value,
                                                   int64_t integer_value);
UmiStatus umi_application_experience_value_decimal(UmiApplicationExperienceValue *value,
                                                   double decimal_value);
UmiStatus umi_application_experience_value_text(UmiApplicationExperienceValue *value,
                                                const char *text_value);
UmiStatus umi_application_experience_value_validate(const UmiApplicationExperienceValue *value);
int umi_application_experience_value_equal(const UmiApplicationExperienceValue *left,
                                           const UmiApplicationExperienceValue *right);
int umi_application_experience_value_empty(const UmiApplicationExperienceValue *value);

#ifdef __cplusplus
}
#endif

#endif
