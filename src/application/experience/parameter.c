/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/parameter.c
 *
 * PURPOSE:
 *   Validate command parameter metadata and values before a command reaches
 *   application logic or a frontend-specific renderer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/parameter.h"

UmiStatus
umi_application_experience_parameter_validate(const UmiApplicationExperienceParameter *parameter) {
  UmiStatus status;
  if (parameter == NULL || parameter->label == NULL || parameter->description == NULL ||
      parameter->label[0] == '\0' ||
      !umi_application_experience_identifier_valid(parameter->parameter_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (parameter->value_type < UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN ||
      parameter->value_type > UMI_APPLICATION_EXPERIENCE_VALUE_TEXT ||
      (parameter->required != 0 && parameter->required != 1) ||
      (parameter->secret != 0 && parameter->secret != 1))
    return UMI_STATUS_INVALID_STATE;
  status = umi_application_experience_value_validate(&parameter->default_value);
  if (status != UMI_STATUS_OK)
    return status;
  return parameter->default_value.type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE ||
                 parameter->default_value.type == parameter->value_type
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_STATE;
}

UmiStatus
umi_application_experience_parameter_accepts(const UmiApplicationExperienceParameter *parameter,
                                             const UmiApplicationExperienceValue *value) {
  UmiStatus status = umi_application_experience_parameter_validate(parameter);
  if (status != UMI_STATUS_OK || value == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_experience_value_validate(value);
  if (status != UMI_STATUS_OK)
    return status;
  if (value->type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE)
    return parameter->required ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
  return value->type == parameter->value_type ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
