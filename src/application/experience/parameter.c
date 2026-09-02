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

/*
 * Check that application experience parameter satisfies its contract before another
 * service relies on it.
 */
UmiStatus
umi_application_experience_parameter_validate(const UmiApplicationExperienceParameter *parameter) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (parameter == NULL || parameter->label == NULL || parameter->description == NULL ||
      parameter->label[0] == '\0' ||
      !umi_application_experience_identifier_valid(parameter->parameter_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (parameter->value_type < UMI_APPLICATION_EXPERIENCE_VALUE_BOOLEAN ||
      parameter->value_type > UMI_APPLICATION_EXPERIENCE_VALUE_TEXT ||
      (parameter->required != 0 && parameter->required != 1) ||
      (parameter->secret != 0 && parameter->secret != 1))
    return UMI_STATUS_INVALID_STATE;
  status = umi_application_experience_value_validate(&parameter->default_value);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  return parameter->default_value.type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE ||
                 parameter->default_value.type == parameter->value_type
             ? UMI_STATUS_OK
             : UMI_STATUS_INVALID_STATE;
}

/*
 * Provide the application experience parameter accepts operation used by this module and
 * its client applications.
 */
UmiStatus
umi_application_experience_parameter_accepts(const UmiApplicationExperienceParameter *parameter,
                                             const UmiApplicationExperienceValue *value) {
  UmiStatus status = umi_application_experience_parameter_validate(parameter);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status != UMI_STATUS_OK || value == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  status = umi_application_experience_value_validate(value);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /* Apply this branch only when its contract condition is satisfied. */
  if (value->type == UMI_APPLICATION_EXPERIENCE_VALUE_NONE)
    return parameter->required ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
  return value->type == parameter->value_type ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
