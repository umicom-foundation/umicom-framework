/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/configuration_registry.c
 *
 * PURPOSE:
 *   Implement bounded named build/run configuration storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/configuration_registry.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchConfigurationRegistry {
    UmiDeveloperWorkbenchConfiguration
        items[UMI_DEVELOPER_WORKBENCH_MAX_CONFIGURATIONS];
    size_t count;
    char active_id[UMI_DEVELOPER_WORKBENCH_ID_CAPACITY];
    uint64_t revision;
};

/* Provide the find index operation used by this module and its client applications. */
static size_t find_index(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || configuration_id == NULL) return (size_t)-1;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].configuration_id,
                   configuration_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

/*
 * Initialise developer workbench configuration registry from caller-provided values so
 * later operations receive a known state.
 */
UmiStatus umi_developer_workbench_configuration_registry_create(
    UmiDeveloperWorkbenchConfigurationRegistry **out_registry)
{
    UmiDeveloperWorkbenchConfigurationRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry =
        (UmiDeveloperWorkbenchConfigurationRegistry *)calloc(
            1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer workbench configuration registry so the same
 * storage can be reused safely.
 */
void umi_developer_workbench_configuration_registry_destroy(
    UmiDeveloperWorkbenchConfigurationRegistry *registry)
{
    free(registry);
}

/*
 * Provide the developer workbench configuration registry upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_configuration_registry_upsert(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const UmiDeveloperWorkbenchConfiguration *configuration)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_configuration_validate(
        configuration, NULL, 0U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    index = find_index(registry, configuration->configuration_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (registry->count >= UMI_DEVELOPER_WORKBENCH_MAX_CONFIGURATIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }

    registry->items[index] = *configuration;
    registry->items[index].revision = ++registry->revision;

    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (registry->active_id[0] == '\0') {
        (void)snprintf(registry->active_id,
                       sizeof(registry->active_id),
                       "%s",
                       configuration->configuration_id);
    }

    return UMI_STATUS_OK;
}

/*
 * Find developer workbench configuration registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_developer_workbench_configuration_registry_find(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id,
    UmiDeveloperWorkbenchConfiguration *out_configuration)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || configuration_id == NULL ||
        out_configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, configuration_id);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_configuration = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Find developer workbench configuration registry while leaving the underlying catalogue
 * or model owned by this module.
 */
UmiStatus umi_developer_workbench_configuration_registry_at(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    size_t index,
    UmiDeveloperWorkbenchConfiguration *out_configuration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_configuration = registry->items[index];
    return UMI_STATUS_OK;
}

/*
 * Provide the developer workbench configuration registry activate operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_configuration_registry_activate(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || configuration_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (find_index(registry, configuration_id) == (size_t)-1) {
        return UMI_STATUS_NOT_FOUND;
    }

    (void)snprintf(registry->active_id,
                   sizeof(registry->active_id),
                   "%s",
                   configuration_id);
    registry->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the developer workbench configuration registry active operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_configuration_registry_active(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    UmiDeveloperWorkbenchConfiguration *out_configuration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || out_configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (registry->active_id[0] == '\0') return UMI_STATUS_NOT_FOUND;

    return umi_developer_workbench_configuration_registry_find(
        registry,
        registry->active_id,
        out_configuration);
}

/*
 * Return the number of records represented by developer workbench configuration registry
 * without changing their state.
 */
size_t umi_developer_workbench_configuration_registry_count(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

/*
 * Provide the developer workbench configuration registry revision operation used by this
 * module and its client applications.
 */
uint64_t umi_developer_workbench_configuration_registry_revision(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
