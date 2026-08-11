/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/registry.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/registry.h"

#include <string.h>

void umi_integration_registry_init(UmiIntegrationRegistry *registry)
{
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

UmiIntegrationRegistryEntry *umi_integration_registry_find(
    UmiIntegrationRegistry *registry,
    const char *application_id)
{
    size_t index;
    if (registry == NULL || application_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index].application.id, application_id) == 0) {
            return &registry->entries[index];
        }
    }
    return NULL;
}

const UmiIntegrationRegistryEntry *umi_integration_registry_find_const(
    const UmiIntegrationRegistry *registry,
    const char *application_id)
{
    size_t index;
    if (registry == NULL || application_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index].application.id, application_id) == 0) {
            return &registry->entries[index];
        }
    }
    return NULL;
}

UmiStatus umi_integration_registry_register(
    UmiIntegrationRegistry *registry,
    const UmiIntegrationApplication *application)
{
    if (registry == NULL || application == NULL || application->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_integration_registry_find(registry, application->id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (registry->count >= UMI_INTEGRATION_MAX_APPLICATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->entries[registry->count].application = *application;
    registry->entries[registry->count].state = UMI_INTEGRATION_APP_DISCOVERED;
    ++registry->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_integration_registry_unregister(
    UmiIntegrationRegistry *registry,
    const char *application_id)
{
    size_t index;
    if (registry == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->entries[index].application.id, application_id) == 0) {
            size_t move_index;
            for (move_index = index + 1U;
                 move_index < registry->count;
                 ++move_index) {
                registry->entries[move_index - 1U] = registry->entries[move_index];
            }
            --registry->count;
            (void)memset(&registry->entries[registry->count],
                         0,
                         sizeof(registry->entries[registry->count]));
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

const UmiIntegrationRegistryEntry *umi_integration_registry_find_capability(
    const UmiIntegrationRegistry *registry,
    const char *capability_id)
{
    size_t index;
    if (registry == NULL || capability_id == NULL) {
        return NULL;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (registry->entries[index].application.enabled &&
            umi_integration_application_has_capability(
                &registry->entries[index].application,
                capability_id)) {
            return &registry->entries[index];
        }
    }
    return NULL;
}

UmiStatus umi_integration_registry_set_state(
    UmiIntegrationRegistry *registry,
    const char *application_id,
    UmiIntegrationApplicationState state)
{
    UmiIntegrationRegistryEntry *entry =
        umi_integration_registry_find(registry, application_id);
    if (entry == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    entry->state = state;
    return UMI_STATUS_OK;
}
