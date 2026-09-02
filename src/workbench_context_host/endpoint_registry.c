/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/endpoint_registry.c
 *
 * PURPOSE:
 *   Implement bounded dynamic endpoint registration, replacement, lookup and removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/endpoint_registry.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiWorkbenchContextHostEndpointRegistry *registry)
{
    size_t new_capacity;
    UmiWorkbenchContextHostEndpoint *items;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = registry->capacity == 0U ? 16U : registry->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS;
    }
    items = (UmiWorkbenchContextHostEndpoint *)realloc(
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
 * Initialise workbench context host endpoint registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_endpoint_registry_init(
    UmiWorkbenchContextHostEndpointRegistry *registry)
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
 * Release or reset state held by workbench context host endpoint registry so the same
 * storage can be reused safely.
 */
void umi_workbench_context_host_endpoint_registry_destroy(
    UmiWorkbenchContextHostEndpointRegistry *registry)
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
 * Find workbench context host endpoint registry while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || endpoint_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].endpoint_id, endpoint_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context host endpoint registry find const operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_const(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || endpoint_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].endpoint_id, endpoint_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context host endpoint registry find panel operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_panel(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *panel_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || panel_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].panel_id, panel_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context host endpoint registry upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_endpoint_registry_upsert(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const UmiWorkbenchContextHostEndpoint *endpoint)
{
    UmiWorkbenchContextHostEndpoint *existing;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || endpoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_endpoint_validate(endpoint);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    existing = umi_workbench_context_host_endpoint_registry_find(
        registry, endpoint->endpoint_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing != NULL) {
        *existing = *endpoint;
        ++existing->revision;
        ++registry->revision;
        return UMI_STATUS_OK;
    }
    status = ensure_capacity(registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *endpoint;
    ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench context host endpoint registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_host_endpoint_registry_remove(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || endpoint_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].endpoint_id, endpoint_id) == 0) {
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
 * Return the number of records represented by workbench context host endpoint registry
 * without changing their state.
 */
size_t umi_workbench_context_host_endpoint_registry_count(
    const UmiWorkbenchContextHostEndpointRegistry *registry)
{
    return registry == NULL ? 0U : registry->count;
}
