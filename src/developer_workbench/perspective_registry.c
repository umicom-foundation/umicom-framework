/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/perspective_registry.c
 *
 * PURPOSE:
 *   Implement bounded perspective registration and activation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/*
 * Initialise developer workbench perspective registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_perspective_registry_create(
    UmiDeveloperWorkbenchPerspectiveRegistry **out_registry)
{
    UmiDeveloperWorkbenchPerspectiveRegistry *registry;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;

    registry =
        (UmiDeveloperWorkbenchPerspectiveRegistry *)calloc(
            1U, sizeof(*registry));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_registry = registry;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer workbench perspective registry so the same
 * storage can be reused safely.
 */
void umi_developer_workbench_perspective_registry_destroy(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    free(registry);
}

/*
 * Add developer workbench perspective registry only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_developer_workbench_perspective_registry_register(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const UmiDeveloperWorkbenchPerspectiveDefinition *perspective)
{
    size_t index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || perspective == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_developer_workbench_perspective_validate(perspective);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index]->perspective_id,
                   perspective->perspective_id) == 0) {
            registry->items[index] = perspective;
            return UMI_STATUS_OK;
        }
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count >= UMI_DEVELOPER_WORKBENCH_MAX_PERSPECTIVES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    registry->items[registry->count++] = perspective;
    /* Apply this operation only while the related capability or state is available. */
    if (!registry->has_active) {
        registry->active_index = 0U;
        registry->has_active = 1;
    }

    return UMI_STATUS_OK;
}

/*
 * Find developer workbench perspective registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_developer_workbench_perspective_registry_find(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id,
    const UmiDeveloperWorkbenchPerspectiveDefinition **out_perspective)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || perspective_id == NULL ||
        out_perspective == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index]->perspective_id,
                   perspective_id) == 0) {
            *out_perspective = registry->items[index];
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the developer workbench perspective registry activate operation used by this
 * module and its client applications.
 */
UmiStatus umi_developer_workbench_perspective_registry_activate(
    UmiDeveloperWorkbenchPerspectiveRegistry *registry,
    const char *perspective_id)
{
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || perspective_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index]->perspective_id,
                   perspective_id) == 0) {
            registry->active_index = index;
            registry->has_active = 1;
            return UMI_STATUS_OK;
        }
    }

    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the developer workbench perspective registry active operation used by this
 * module and its client applications.
 */
const UmiDeveloperWorkbenchPerspectiveDefinition *
umi_developer_workbench_perspective_registry_active(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || !registry->has_active ||
        registry->active_index >= registry->count) {
        return NULL;
    }

    return registry->items[registry->active_index];
}

/*
 * Return the number of records represented by developer workbench perspective registry
 * without changing their state.
 */
size_t umi_developer_workbench_perspective_registry_count(
    const UmiDeveloperWorkbenchPerspectiveRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
