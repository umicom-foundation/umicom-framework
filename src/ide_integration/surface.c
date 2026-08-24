/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface.c
 *
 * PURPOSE:
 *   Implement bounded IDE surface registration by stable identity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface.h"

#include <stdlib.h>
#include <string.h>

struct UmiIdeSurfaceRegistry {
    UmiIdeSurfaceDescriptor items[UMI_IDE_SURFACE_CAPACITY];
    size_t count;
};

UmiStatus umi_ide_surface_registry_create(UmiIdeSurfaceRegistry **out_registry)
{
    UmiIdeSurfaceRegistry *registry;

    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiIdeSurfaceRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

void umi_ide_surface_registry_destroy(UmiIdeSurfaceRegistry *registry)
{
    free(registry);
}

const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_find(
    const UmiIdeSurfaceRegistry *registry,
    const char *surface_id)
{
    size_t index;

    if (registry == NULL || surface_id == NULL) return NULL;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].surface_id, surface_id) == 0) {
            return &registry->items[index];
        }
    }

    return NULL;
}

UmiStatus umi_ide_surface_registry_add(
    UmiIdeSurfaceRegistry *registry,
    const UmiIdeSurfaceDescriptor *descriptor)
{
    if (registry == NULL || descriptor == NULL ||
        descriptor->surface_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_ide_surface_registry_find(registry, descriptor->surface_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    if (registry->count >= UMI_IDE_SURFACE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->items[registry->count++] = *descriptor;
    return UMI_STATUS_OK;
}

const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_at(
    const UmiIdeSurfaceRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? &registry->items[index]
        : NULL;
}

size_t umi_ide_surface_registry_count(
    const UmiIdeSurfaceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
