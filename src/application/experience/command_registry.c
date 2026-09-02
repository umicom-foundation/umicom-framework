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

/*
 * Initialise application experience command registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_command_registry_init(
    UmiApplicationExperienceCommandRegistry *registry) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry != NULL) {
    (void)memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
  }
}

/*
 * Find application experience command registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceCommand *umi_application_experience_command_registry_find(
    const UmiApplicationExperienceCommandRegistry *registry, const char *command_id) {
  size_t index;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || command_id == NULL ||
      registry->count > UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return NULL;
  /* Visit each bounded item once so every record receives the same rule. */
  for (index = 0U; index < registry->count; ++index) {
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strcmp(registry->items[index]->command_id, command_id) == 0)
      return registry->items[index];
  }
  return NULL;
}

/*
 * Add application experience command registry only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_application_experience_command_registry_register(
    UmiApplicationExperienceCommandRegistry *registry,
    const UmiApplicationExperienceCommand *command) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (registry == NULL || command == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (registry->count > UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  status = umi_application_experience_command_validate(command);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status != UMI_STATUS_OK)
    return status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (umi_application_experience_command_registry_find(registry, command->command_id) != NULL)
    return UMI_STATUS_ALREADY_EXISTS;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (registry->count >= UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY)
    return UMI_STATUS_CAPACITY_EXCEEDED;
  registry->items[registry->count++] = command;
  registry->revision += 1U;
  return UMI_STATUS_OK;
}

/*
 * Find application experience command registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceCommand *umi_application_experience_command_registry_at(
    const UmiApplicationExperienceCommandRegistry *registry, size_t index) {
  return registry != NULL && registry->count <= UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY &&
                 index < registry->count
             ? registry->items[index]
             : NULL;
}
