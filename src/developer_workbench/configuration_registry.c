/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/configuration_registry.c
 *
 * PURPOSE:
 *   Implement bounded named build/run configuration storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

static size_t find_index(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id)
{
    size_t index;

    if (registry == NULL || configuration_id == NULL) return (size_t)-1;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].configuration_id,
                   configuration_id) == 0) {
            return index;
        }
    }

    return (size_t)-1;
}

UmiStatus umi_developer_workbench_configuration_registry_create(
    UmiDeveloperWorkbenchConfigurationRegistry **out_registry)
{
    UmiDeveloperWorkbenchConfigurationRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry =
        (UmiDeveloperWorkbenchConfigurationRegistry *)calloc(
            1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    registry->revision = 1U;
    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_workbench_configuration_registry_destroy(
    UmiDeveloperWorkbenchConfigurationRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_workbench_configuration_registry_upsert(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const UmiDeveloperWorkbenchConfiguration *configuration)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_configuration_validate(
        configuration, NULL, 0U);
    if (status != UMI_STATUS_OK) return status;

    index = find_index(registry, configuration->configuration_id);
    if (index == (size_t)-1) {
        if (registry->count >= UMI_DEVELOPER_WORKBENCH_MAX_CONFIGURATIONS) {
            return UMI_STATUS_CAPACITY_EXCEEDED;
        }
        index = registry->count++;
    }

    registry->items[index] = *configuration;
    registry->items[index].revision = ++registry->revision;

    if (registry->active_id[0] == '\0') {
        (void)snprintf(registry->active_id,
                       sizeof(registry->active_id),
                       "%s",
                       configuration->configuration_id);
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_configuration_registry_find(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id,
    UmiDeveloperWorkbenchConfiguration *out_configuration)
{
    size_t index;

    if (registry == NULL || configuration_id == NULL ||
        out_configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    index = find_index(registry, configuration_id);
    if (index == (size_t)-1) return UMI_STATUS_NOT_FOUND;

    *out_configuration = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_configuration_registry_at(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    size_t index,
    UmiDeveloperWorkbenchConfiguration *out_configuration)
{
    if (registry == NULL || out_configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= registry->count) return UMI_STATUS_NOT_FOUND;

    *out_configuration = registry->items[index];
    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_configuration_registry_activate(
    UmiDeveloperWorkbenchConfigurationRegistry *registry,
    const char *configuration_id)
{
    if (registry == NULL || configuration_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
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

UmiStatus umi_developer_workbench_configuration_registry_active(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry,
    UmiDeveloperWorkbenchConfiguration *out_configuration)
{
    if (registry == NULL || out_configuration == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (registry->active_id[0] == '\0') return UMI_STATUS_NOT_FOUND;

    return umi_developer_workbench_configuration_registry_find(
        registry,
        registry->active_id,
        out_configuration);
}

size_t umi_developer_workbench_configuration_registry_count(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}

uint64_t umi_developer_workbench_configuration_registry_revision(
    const UmiDeveloperWorkbenchConfigurationRegistry *registry)
{
    return registry != NULL ? registry->revision : 0U;
}
