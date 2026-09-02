/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ide_integration/surface.c
 *
 * PURPOSE:
 *   Implement bounded IDE surface registration by stable identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ide_integration/surface.h"

#include <stdlib.h>
#include <string.h>

struct UmiIdeSurfaceRegistry {
    UmiIdeSurfaceDescriptor items[UMI_IDE_SURFACE_CAPACITY];
    size_t count;
};

/*
 * Initialise ide surface registry from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_ide_surface_registry_create(UmiIdeSurfaceRegistry **out_registry)
{
    UmiIdeSurfaceRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry = (UmiIdeSurfaceRegistry *)calloc(1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by ide surface registry so the same storage can be reused
 * safely.
 */
void umi_ide_surface_registry_destroy(UmiIdeSurfaceRegistry *registry)
{
    free(registry);
}

/*
 * Find ide surface registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_find(
    const UmiIdeSurfaceRegistry *registry,
    const char *surface_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || surface_id == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].surface_id, surface_id) == 0) {
            return &registry->items[index];
        }
    }

    return NULL;
}

/* Add ide surface registry only after its inputs and available capacity have been checked. */
UmiStatus umi_ide_surface_registry_add(
    UmiIdeSurfaceRegistry *registry,
    const UmiIdeSurfaceDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || descriptor == NULL ||
        descriptor->surface_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_ide_surface_registry_find(registry, descriptor->surface_id) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_IDE_SURFACE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->items[registry->count++] = *descriptor;
    return UMI_STATUS_OK;
}

/*
 * Find ide surface registry while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiIdeSurfaceDescriptor *umi_ide_surface_registry_at(
    const UmiIdeSurfaceRegistry *registry,
    size_t index)
{
    return registry != NULL && index < registry->count
        ? &registry->items[index]
        : NULL;
}

/*
 * Return the number of records represented by ide surface registry without changing their
 * state.
 */
size_t umi_ide_surface_registry_count(
    const UmiIdeSurfaceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
