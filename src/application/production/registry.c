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

/*
 * Initialise application production registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_production_registry_init(
    UmiApplicationProductionRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) (void)memset(registry, 0, sizeof(*registry));
}

/*
 * Find application production registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationProductionBinding *
umi_application_production_registry_find(
    const UmiApplicationProductionRegistry *registry,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || application_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].experience->application_id,
                   application_id) == 0)
            return &registry->entries[index];
    return NULL;
}

/*
 * Add application production registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_application_production_registry_register(
    UmiApplicationProductionRegistry *registry,
    const UmiProductApplicationAdoption *adoption)
{
    UmiApplicationProductionBinding binding;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || adoption == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_APPLICATION_PRODUCTION_MAX_APPLICATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_production_registry_find(
            registry, adoption->application_id) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    status = umi_application_production_binding_init(adoption, &binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    registry->entries[registry->count] = binding;
    registry->count += 1U;
    return UMI_STATUS_OK;
}

