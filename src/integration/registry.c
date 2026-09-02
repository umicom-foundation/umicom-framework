/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/registry.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/registry.h"

#include <string.h>

/*
 * Initialise integration registry from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_registry_init(UmiIntegrationRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry != NULL) {
        (void)memset(registry, 0, sizeof(*registry));
    }
}

/*
 * Find integration registry while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiIntegrationRegistryEntry *umi_integration_registry_find(
    UmiIntegrationRegistry *registry,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || application_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].application.id, application_id) == 0) {
            return &registry->entries[index];
        }
    }
    return NULL;
}

/*
 * Provide the integration registry find const operation used by this module and its client
 * applications.
 */
const UmiIntegrationRegistryEntry *umi_integration_registry_find_const(
    const UmiIntegrationRegistry *registry,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || application_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].application.id, application_id) == 0) {
            return &registry->entries[index];
        }
    }
    return NULL;
}

/* Add integration registry only after its inputs and available capacity have been checked. */
UmiStatus umi_integration_registry_register(
    UmiIntegrationRegistry *registry,
    const UmiIntegrationApplication *application)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || application == NULL || application->id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_integration_registry_find(registry, application->id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_INTEGRATION_MAX_APPLICATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    registry->entries[registry->count].application = *application;
    registry->entries[registry->count].state = UMI_INTEGRATION_APP_DISCOVERED;
    ++registry->count;
    return UMI_STATUS_OK;
}

/*
 * Remove integration registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_integration_registry_unregister(
    UmiIntegrationRegistry *registry,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || application_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->entries[index].application.id, application_id) == 0) {
            size_t move_index;
            /* Visit each bounded item once so every record receives the same rule. */
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

/*
 * Provide the integration registry find capability operation used by this module and its
 * client applications.
 */
const UmiIntegrationRegistryEntry *umi_integration_registry_find_capability(
    const UmiIntegrationRegistry *registry,
    const char *capability_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || capability_id == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->entries[index].application.enabled &&
            umi_integration_application_has_capability(
                &registry->entries[index].application,
                capability_id)) {
            return &registry->entries[index];
        }
    }
    return NULL;
}

/*
 * Provide the integration registry set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_integration_registry_set_state(
    UmiIntegrationRegistry *registry,
    const char *application_id,
    UmiIntegrationApplicationState state)
{
    UmiIntegrationRegistryEntry *entry =
        umi_integration_registry_find(registry, application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) {
        return UMI_STATUS_NOT_FOUND;
    }
    entry->state = state;
    return UMI_STATUS_OK;
}
