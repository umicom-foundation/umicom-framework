/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/command.c
 *
 * PURPOSE:
 *   Validate reusable command definitions and provide deterministic parameter
 *   and capability lookup for every application frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/command.h"

#include <string.h>

UmiStatus
umi_application_experience_command_validate(const UmiApplicationExperienceCommand *command) {
  size_t index;
  if (command == NULL || command->title == NULL || command->description == NULL ||
      command->category == NULL || command->title[0] == '\0' || command->category[0] == '\0' ||
      !umi_application_experience_identifier_valid(command->command_id))
    return UMI_STATUS_INVALID_ARGUMENT;
  if (command->struct_size < sizeof(*command) ||
      command->api_version != UMI_APPLICATION_EXPERIENCE_API_VERSION ||
      command->risk < UMI_APPLICATION_EXPERIENCE_COMMAND_SAFE ||
      command->risk > UMI_APPLICATION_EXPERIENCE_COMMAND_RESTRICTED ||
      command->parameter_count > UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY ||
      (command->parameter_count > 0U && command->parameters == NULL))
    return UMI_STATUS_INVALID_STATE;
  if (command->permission_id != NULL && command->permission_id[0] != '\0' &&
      !umi_application_experience_identifier_valid(command->permission_id))
    return UMI_STATUS_INVALID_STATE;
  for (index = 0U; index < command->parameter_count; ++index) {
    size_t previous;
    if (umi_application_experience_parameter_validate(&command->parameters[index]) != UMI_STATUS_OK)
      return UMI_STATUS_INVALID_STATE;
    for (previous = 0U; previous < index; ++previous) {
      if (strcmp(command->parameters[previous].parameter_id,
                 command->parameters[index].parameter_id) == 0)
        return UMI_STATUS_ALREADY_EXISTS;
    }
  }
  return UMI_STATUS_OK;
}

const UmiApplicationExperienceParameter *
umi_application_experience_command_parameter(const UmiApplicationExperienceCommand *command,
                                             const char *parameter_id) {
  size_t index;
  if (command == NULL || parameter_id == NULL ||
      command->parameter_count > UMI_APPLICATION_EXPERIENCE_PARAMETER_CAPACITY)
    return NULL;
  for (index = 0U; index < command->parameter_count; ++index) {
    if (strcmp(command->parameters[index].parameter_id, parameter_id) == 0)
      return &command->parameters[index];
  }
  return NULL;
}

int umi_application_experience_command_has_flag(const UmiApplicationExperienceCommand *command,
                                                UmiApplicationExperienceCommandFlags flag) {
  return command != NULL && (command->flags & (uint32_t)flag) != 0U;
}
