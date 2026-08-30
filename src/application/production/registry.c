/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/registry.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/registry.h"

#include <string.h>

void umi_application_production_registry_init(
    UmiApplicationProductionRegistry *registry)
{
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

const UmiApplicationProductionBinding *
umi_application_production_registry_find(
    const UmiApplicationProductionRegistry *registry,
    const char *application_id)
{
    size_t index;
    if (registry == NULL || application_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index)
        if (strcmp(registry->entries[index].experience->application_id,
                   application_id) == 0)
            return &registry->entries[index];
    return NULL;
}

UmiStatus umi_application_production_registry_register(
    UmiApplicationProductionRegistry *registry,
    const UmiProductApplicationAdoption *adoption)
{
    UmiApplicationProductionBinding binding;
    UmiStatus status;
    if (registry == NULL || adoption == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (registry->count >= UMI_APPLICATION_PRODUCTION_MAX_APPLICATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_application_production_registry_find(
            registry, adoption->application_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    status = umi_application_production_binding_init(adoption, &binding);
    if (status != UMI_STATUS_OK) return status;
    registry->entries[registry->count] = binding;
    registry->count += 1U;
    return UMI_STATUS_OK;
}

