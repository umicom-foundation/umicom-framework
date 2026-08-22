/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/endpoint_registry.c
 *
 * PURPOSE:
 *   Implement bounded dynamic endpoint registration, replacement, lookup and removal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/endpoint_registry.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(UmiWorkbenchContextHostEndpointRegistry *registry)
{
    size_t new_capacity;
    UmiWorkbenchContextHostEndpoint *items;
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    if (registry->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = registry->capacity == 0U ? 16U : registry->capacity * 2U;
    if (new_capacity > UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_HOST_MAX_ENDPOINTS;
    }
    items = (UmiWorkbenchContextHostEndpoint *)realloc(
        registry->items, new_capacity * sizeof(registry->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (new_capacity > registry->capacity) {
        memset(items + registry->capacity, 0,
               (new_capacity - registry->capacity) * sizeof(items[0]));
    }
    registry->items = items;
    registry->capacity = new_capacity;
    return UMI_STATUS_OK;
}

void umi_workbench_context_host_endpoint_registry_init(
    UmiWorkbenchContextHostEndpointRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

void umi_workbench_context_host_endpoint_registry_destroy(
    UmiWorkbenchContextHostEndpointRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    memset(registry, 0, sizeof(*registry));
}

UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id)
{
    size_t index;
    if (registry == NULL || endpoint_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].endpoint_id, endpoint_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_const(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id)
{
    size_t index;
    if (registry == NULL || endpoint_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].endpoint_id, endpoint_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

const UmiWorkbenchContextHostEndpoint *
umi_workbench_context_host_endpoint_registry_find_panel(
    const UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *panel_id)
{
    size_t index;
    if (registry == NULL || panel_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].panel_id, panel_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_context_host_endpoint_registry_upsert(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const UmiWorkbenchContextHostEndpoint *endpoint)
{
    UmiWorkbenchContextHostEndpoint *existing;
    UmiStatus status;
    if (registry == NULL || endpoint == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_workbench_context_host_endpoint_validate(endpoint);
    if (status != UMI_STATUS_OK) return status;
    existing = umi_workbench_context_host_endpoint_registry_find(
        registry, endpoint->endpoint_id);
    if (existing != NULL) {
        *existing = *endpoint;
        ++existing->revision;
        ++registry->revision;
        return UMI_STATUS_OK;
    }
    status = ensure_capacity(registry);
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *endpoint;
    ++registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_endpoint_registry_remove(
    UmiWorkbenchContextHostEndpointRegistry *registry,
    const char *endpoint_id)
{
    size_t index;
    if (registry == NULL || endpoint_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].endpoint_id, endpoint_id) == 0) {
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

size_t umi_workbench_context_host_endpoint_registry_count(
    const UmiWorkbenchContextHostEndpointRegistry *registry)
{
    return registry == NULL ? 0U : registry->count;
}
