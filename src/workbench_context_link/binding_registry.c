/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/binding_registry.c
 *
 * PURPOSE:
 *   Implement bounded dynamic panel-binding registration and deterministic detachment.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/binding_registry.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(UmiWorkbenchContextLinkBindingRegistry *registry)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkPanelBinding *items;
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    if (registry->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_BINDINGS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = registry->capacity == 0U ? 16U : registry->capacity * 2U;
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_BINDINGS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_BINDINGS;
    }
    items = (UmiWorkbenchContextLinkPanelBinding *)realloc(
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

void umi_workbench_context_link_binding_registry_init(
    UmiWorkbenchContextLinkBindingRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

void umi_workbench_context_link_binding_registry_destroy(
    UmiWorkbenchContextLinkBindingRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    memset(registry, 0, sizeof(*registry));
}

const UmiWorkbenchContextLinkPanelBinding *
umi_workbench_context_link_binding_registry_find(
    const UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id)
{
    size_t index;
    if (registry == NULL || binding_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].binding_id, binding_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_context_link_binding_registry_upsert(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const UmiWorkbenchContextLinkPanelBinding *binding)
{
    size_t index;
    UmiStatus status;
    if (registry == NULL || binding == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_workbench_context_link_panel_binding_validate(binding) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].binding_id, binding->binding_id) == 0) {
            registry->items[index] = *binding;
            ++registry->items[index].revision;
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    status = ensure_capacity(registry);
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *binding;
    ++registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_link_binding_registry_remove(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *binding_id)
{
    size_t index;
    if (registry == NULL || binding_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].binding_id, binding_id) == 0) {
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

UmiStatus umi_workbench_context_link_binding_registry_remove_panel(
    UmiWorkbenchContextLinkBindingRegistry *registry,
    const char *panel_id)
{
    size_t read_index;
    size_t write_index = 0U;
    size_t original_count;
    if (registry == NULL || panel_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    original_count = registry->count;
    for (read_index = 0U; read_index < original_count; ++read_index) {
        if (strcmp(registry->items[read_index].panel_id, panel_id) == 0) continue;
        if (write_index != read_index) {
            registry->items[write_index] = registry->items[read_index];
        }
        ++write_index;
    }
    if (write_index == original_count) return UMI_STATUS_NOT_FOUND;
    if (write_index < original_count) {
        memset(&registry->items[write_index], 0,
               (original_count - write_index) * sizeof(registry->items[0]));
    }
    registry->count = write_index;
    ++registry->revision;
    return UMI_STATUS_OK;
}

size_t umi_workbench_context_link_binding_registry_count(
    const UmiWorkbenchContextLinkBindingRegistry *registry)
{
    return registry == NULL ? 0U : registry->count;
}
