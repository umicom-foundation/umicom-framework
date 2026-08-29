/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/invocation.c
 *
 * PURPOSE:
 *   Build and validate bounded command invocations before dispatch, keeping
 *   malformed or incomplete input away from application services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/invocation.h"

#include <string.h>

#include "umicom/base/text.h"

UmiStatus umi_application_experience_invocation_init(UmiApplicationExperienceInvocation *invocation,
                                                     const char *command_id,
                                                     const char *correlation_id,
                                                     const char *origin_component_id) {
  UmiStatus status;
  if (invocation == NULL || correlation_id == NULL || origin_component_id == NULL ||
      !umi_application_experience_identifier_valid(command_id) ||
      !umi_application_experience_identifier_valid(correlation_id) ||
      !umi_application_experience_identifier_valid(origin_component_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(invocation, 0, sizeof(*invocation));
  status = umi_text_copy(invocation->command_id, sizeof(invocation->command_id), command_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(invocation->correlation_id, sizeof(invocation->correlation_id),
                           correlation_id);
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(invocation->origin_component_id, sizeof(invocation->origin_component_id),
                           origin_component_id);
  return status;
}

UmiStatus umi_application_experience_invocation_set(UmiApplicationExperienceInvocation *invocation,
                                                    const char *parameter_id,
                                                    const UmiApplicationExperienceValue *value) {
  size_t index;
  if (invocation == NULL || value == NULL ||
      !umi_application_experience_identifier_valid(parameter_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (invocation->argument_count > UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY ||
      umi_application_experience_value_validate(value) != UMI_STATUS_OK)
    return UMI_STATUS_INVALID_STATE;
  for (index = 0U; index < invocation->argument_count; ++index) {
    if (strcmp(invocation->arguments[index].parameter_id, parameter_id) == 0) {
      invocation->arguments[index].value = *value;
      return UMI_STATUS_OK;
    }
  }
  if (invocation->argument_count >= UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  index = invocation->argument_count++;
  if (umi_text_copy(invocation->arguments[index].parameter_id,
                    sizeof(invocation->arguments[index].parameter_id),
                    parameter_id) != UMI_STATUS_OK) {
    invocation->argument_count -= 1U;
    return UMI_STATUS_CAPACITY_EXCEEDED;
  }
  invocation->arguments[index].value = *value;
  return UMI_STATUS_OK;
}

const UmiApplicationExperienceValue *
umi_application_experience_invocation_get(const UmiApplicationExperienceInvocation *invocation,
                                          const char *parameter_id) {
  size_t index;
  if (invocation == NULL || parameter_id == NULL ||
      invocation->argument_count > UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY)
    return NULL;
  for (index = 0U; index < invocation->argument_count; ++index) {
    if (strcmp(invocation->arguments[index].parameter_id, parameter_id) == 0)
      return &invocation->arguments[index].value;
  }
  return NULL;
}

UmiStatus umi_application_experience_invocation_validate(
    const UmiApplicationExperienceCommand *command,
    const UmiApplicationExperienceInvocation *invocation) {
  size_t index;
  UmiStatus status = umi_application_experience_command_validate(command);
  if (status != UMI_STATUS_OK || invocation == NULL)
    return status != UMI_STATUS_OK ? status : UMI_STATUS_INVALID_ARGUMENT;
  if (strcmp(command->command_id, invocation->command_id) != 0 ||
      invocation->argument_count > UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY ||
      invocation->correlation_id[0] == '\0' || invocation->origin_component_id[0] == '\0')
    return UMI_STATUS_INVALID_STATE;
  if (command->risk != UMI_APPLICATION_EXPERIENCE_COMMAND_SAFE && !invocation->confirmed)
    return UMI_STATUS_PERMISSION_DENIED;
  for (index = 0U; index < command->parameter_count; ++index) {
    const UmiApplicationExperienceValue *value = umi_application_experience_invocation_get(
        invocation, command->parameters[index].parameter_id);
    if (value == NULL)
      value = &command->parameters[index].default_value;
    status = umi_application_experience_parameter_accepts(&command->parameters[index], value);
    if (status != UMI_STATUS_OK)
      return status;
  }
  for (index = 0U; index < invocation->argument_count; ++index) {
    if (umi_application_experience_command_parameter(
            command, invocation->arguments[index].parameter_id) == NULL)
      return UMI_STATUS_NOT_FOUND;
  }
  return UMI_STATUS_OK;
}
