/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience_registry.c
 *
 * PURPOSE:
 *   Register and resolve canonical application experience definitions without heap allocation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience_registry.h"

#include <string.h>

void umi_application_experience_registry_init(
    UmiApplicationExperienceRegistry *registry)
{
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}

UmiStatus umi_application_experience_registry_register(
    UmiApplicationExperienceRegistry *registry,
    const UmiApplicationExperienceDefinition *definition)
{
    if (registry == NULL || definition == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_application_experience_validate(definition) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_application_experience_registry_find(
            registry, definition->application_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (registry->count >= UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->definitions[registry->count++] = definition;
    return UMI_STATUS_OK;
}

size_t umi_application_experience_registry_count(
    const UmiApplicationExperienceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

const UmiApplicationExperienceDefinition *
umi_application_experience_registry_at(
    const UmiApplicationExperienceRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? registry->definitions[index] : NULL;
}

const UmiApplicationExperienceDefinition *
umi_application_experience_registry_find(
    const UmiApplicationExperienceRegistry *registry,
    const char *application_id)
{
    size_t index;
    if (registry == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->definitions[index]->application_id,
                   application_id) == 0)
            return registry->definitions[index];
    }
    return NULL;
}
