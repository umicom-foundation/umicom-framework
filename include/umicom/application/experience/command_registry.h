/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/command_registry.h
 *
 * PURPOSE:
 *   Provide one bounded registry for reusable parameterised commands instead
 *   of application-specific menu and toolbar command copies.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_COMMAND_REGISTRY_H
#define UMICOM_APPLICATION_EXPERIENCE_COMMAND_REGISTRY_H

#include "umicom/application/experience/invocation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceCommandRegistry {
  const UmiApplicationExperienceCommand *items[UMI_APPLICATION_EXPERIENCE_COMMAND_CAPACITY];
  size_t count;
  uint64_t revision;
} UmiApplicationExperienceCommandRegistry;

void umi_application_experience_command_registry_init(
    UmiApplicationExperienceCommandRegistry *registry);
UmiStatus umi_application_experience_command_registry_register(
    UmiApplicationExperienceCommandRegistry *registry,
    const UmiApplicationExperienceCommand *command);
const UmiApplicationExperienceCommand *umi_application_experience_command_registry_find(
    const UmiApplicationExperienceCommandRegistry *registry, const char *command_id);
const UmiApplicationExperienceCommand *umi_application_experience_command_registry_at(
    const UmiApplicationExperienceCommandRegistry *registry, size_t index);

#ifdef __cplusplus
}
#endif

#endif
