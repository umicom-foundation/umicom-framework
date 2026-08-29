/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/field.h
 *
 * PURPOSE:
 *   Describe reusable form fields, choices, constraints and default values
 *   without prescribing how GTK4, Qt6, web or headless frontends render them.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_FIELD_H
#define UMICOM_APPLICATION_EXPERIENCE_FIELD_H

#include "umicom/application/experience/value.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceFieldOption {
  const char *option_id;
  const char *label;
  int disabled;
} UmiApplicationExperienceFieldOption;

typedef struct UmiApplicationExperienceField {
  const char *field_id;
  const char *label;
  const char *help_text;
  UmiApplicationExperienceFieldKind kind;
  UmiApplicationExperienceValueType value_type;
  int required;
  int read_only;
  int enforce_number_range;
  int enforce_length_range;
  double minimum_number;
  double maximum_number;
  size_t minimum_length;
  size_t maximum_length;
  const UmiApplicationExperienceFieldOption *options;
  size_t option_count;
  UmiApplicationExperienceValue default_value;
} UmiApplicationExperienceField;

UmiStatus umi_application_experience_field_validate(const UmiApplicationExperienceField *field);
UmiStatus umi_application_experience_field_accepts(const UmiApplicationExperienceField *field,
                                                   const UmiApplicationExperienceValue *value);
const UmiApplicationExperienceFieldOption *
umi_application_experience_field_option(const UmiApplicationExperienceField *field,
                                        const char *option_id);

#ifdef __cplusplus
}
#endif

#endif
