/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience_registry.c
 *
 * PURPOSE:
 *   Register and resolve canonical application experience definitions without heap allocation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience_registry.h"

#include <string.h>

/*
 * Initialise application experience registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_registry_init(
    UmiApplicationExperienceRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) memset(registry, 0, sizeof(*registry));
}

/*
 * Add application experience registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_experience_registry_register(
    UmiApplicationExperienceRegistry *registry,
    const UmiApplicationExperienceDefinition *definition)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || definition == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_application_experience_validate(definition) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_experience_registry_find(
            registry, definition->application_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    registry->definitions[registry->count++] = definition;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by application experience registry without
 * changing their state.
 */
size_t umi_application_experience_registry_count(
    const UmiApplicationExperienceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Find application experience registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationExperienceDefinition *
umi_application_experience_registry_at(
    const UmiApplicationExperienceRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? registry->definitions[index] : NULL;
}

/*
 * Find application experience registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationExperienceDefinition *
umi_application_experience_registry_find(
    const UmiApplicationExperienceRegistry *registry,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || application_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->definitions[index]->application_id,
                   application_id) == 0)
            return registry->definitions[index];
    }
    return NULL;
}
