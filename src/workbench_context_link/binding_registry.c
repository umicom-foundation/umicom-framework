/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/binding_registry.c
 *
 * PURPOSE:
 *   Implement bounded dynamic panel-binding registration and deterministic detachment.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/binding_registry.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiWorkbenchContextLinkBindingRegistry *registry)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkPanelBinding *items;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_BINDINGS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = registry->capacity == 0U ? 16U : registry->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_BINDINGS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_BINDINGS;
    }
    items = (UmiWorkbenchContextLinkPanelBinding *)realloc(
        registry->items, new_capacity * sizeof(registry->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (new_capacity > registry->capacity) {
        memset(items + registry->capacity, 0,
               (new_capacity - registry->capacity) * sizeof(items[0]));
    }
    registry->items = items;
    registry->capacity = new_capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench context link binding registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_binding_registry_init(
    UmiWorkbenchContextLinkBindingRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

/*
 * Release or reset state held by workbench context link binding registry so the same
 * storage can be reused safely.
 */
void umi_workbench_context_link_binding_registry_destroy(
    UmiWorkbenchContextLinkBindingRegistry *registry)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL) return;
    free(registry->items);
    memset(registry, 0, sizeof(*registry));
}

/*
 * Find workbench context link binding registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchContextLinkPanelBinding *
umi_workbench_context_link_binding_registry_find(
    const UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || binding_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].binding_id, binding_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context link binding registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_binding_registry_upsert(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const UmiWorkbenchContextLinkPanelBinding *binding)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || binding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_context_link_panel_binding_validate(binding) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].binding_id, binding->binding_id) == 0) {
            registry->items[index] = *binding;
            ++registry->items[index].revision;
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    status = ensure_capacity(registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *binding;
    ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench context link binding registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_link_binding_registry_remove(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || binding_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].binding_id, binding_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < registry->count) {
                memmove(&registry->items[index],
                        &registry->items[index + 1U],
                        (registry->count - index - 1U) * sizeof(registry->items[0]));
            }
            --registry->count;
            memset(&registry->items[registry->count], 0, sizeof(registry->items[0]));
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the workbench context link binding registry remove panel operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_binding_registry_remove_panel(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *panel_id)
{
    size_t read_index;
    size_t write_index = 0U;
    size_t original_count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    original_count = registry->count;
    /* Visit each bounded item once so every record receives the same rule. */
    for (read_index = 0U; read_index < original_count; ++read_index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(registry->items[read_index].panel_id, panel_id) == 0) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (write_index != read_index) {
            registry->items[write_index] = registry->items[read_index];
        }
        ++write_index;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (write_index == original_count) return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (write_index < original_count) {
        memset(&registry->items[write_index], 0,
               (original_count - write_index) * sizeof(registry->items[0]));
    }
    registry->count = write_index;
    ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by workbench context link binding registry
 * without changing their state.
 */
size_t umi_workbench_context_link_binding_registry_count(
    const UmiWorkbenchContextLinkBindingRegistry *registry)
{
    return registry == NULL ? 0U : registry->count;
}
