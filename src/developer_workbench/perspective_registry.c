/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspective_registry.c
 *
 * PURPOSE:
 *   Implement bounded perspective registration and activation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/perspective_registry.h"

#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchPerspectiveRegistry {
    const UmiDeveloperWorkbenchPerspectiveDefinition *
        items[UMI_DEVELOPER_WORKBENCH_MAX_PERSPECTIVES];
    size_t count;
    size_t active_index;
    int has_active;
};

UmiStatus umi_developer_workbench_perspective_registry_create(
    UmiDeveloperWorkbenchPerspectiveRegistry **out_registry)
{
    UmiDeveloperWorkbenchPerspectiveRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry =
        (UmiDeveloperWorkbenchPerspectiveRegistry *)calloc(
            1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_developer_workbench_perspective_registry_destroy(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    free(registry);
}

UmiStatus umi_developer_workbench_perspective_registry_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective)
{
    size_t index;
    UmiStatus status;

    if (registry == NULL || perspective == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_perspective_validate(perspective);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index]->perspective_id,
                   perspective->perspective_id) == 0) {
            registry->items[index] = perspective;
            return UMI_STATUS_OK;
        }
    }

    if (registry->count >= UMI_DEVELOPER_WORKBENCH_MAX_PERSPECTIVES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->items[registry->count++] = perspective;
    if (!registry->has_active) {
        registry->active_index = 0U;
        registry->has_active = 1;
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_perspective_registry_find(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id,
    const UmiDeveloperWorkbenchPerspectiveDefinition **out_perspective)
{
    size_t index;

    if (registry == NULL || perspective_id == NULL ||
        out_perspective == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index]->perspective_id,
                   perspective_id) == 0) {
            *out_perspective = registry->items[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

UmiStatus umi_developer_workbench_perspective_registry_activate(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id)
{
    size_t index;

    if (registry == NULL || perspective_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index]->perspective_id,
                   perspective_id) == 0) {
            registry->active_index = index;
            registry->has_active = 1;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_perspective_registry_active(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    if (registry == NULL || !registry->has_active ||
        registry->active_index >= registry->count) {
        return NULL;
    }

    return registry->items[registry->active_index];
}

size_t umi_developer_workbench_perspective_registry_count(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
