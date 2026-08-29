/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/command_registry.c
 *
 * PURPOSE:
 *   Register and discover reusable commands with duplicate protection and a
 *   revision suitable for frontend command-palette refreshes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/command_registry.h"

#include <string.h>

void umi_application_experience_command_registry_init(
    UmiApplicationExperienceCommandRegistry *registry) {
  if (registry != NULL) {
    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
  }
}

const UmiApplicationExperienceCommand *umi_application_experience_command_registry_find(
    const UmiApplicationExperienceCommandRegistry *registry, const char *command_id) {
  size_t index;
  if (registry == NULL || command_id == NULL ||
      registry->count > UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return NULL;
  for (index = 0U; index < registry->count; ++index) {
    if (strcmp(registry->items[index]->command_id, command_id) == 0)
      return registry->items[index];
  }
  return NULL;
}

UmiStatus umi_application_experience_command_registry_register(
    UmiApplicationExperienceCommandRegistry *registry,
    const UmiApplicationExperienceCommand *command) {
  UmiStatus status;
  if (registry == NULL || command == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (registry->count > UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  status = umi_application_experience_command_validate(command);
  if (status != UMI_STATUS_OK)
    return status;
  if (umi_application_experience_command_registry_find(registry, command->command_id) != NULL)
    return UMI_STATUS_ALREADY_EXISTS;
  if (registry->count >= UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  registry->items[registry->count++] = command;
  registry->revision += 1U;
  return UMI_STATUS_OK;
}

const UmiApplicationExperienceCommand *umi_application_experience_command_registry_at(
    const UmiApplicationExperienceCommandRegistry *registry, size_t index) {
  return registry != NULL && registry->count <= UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY &&
                 index < registry->count
             ? registry->items[index]
             : NULL;
}
