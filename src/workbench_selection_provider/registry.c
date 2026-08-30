/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_selection_provider/registry.c
 *
 * PURPOSE:
 *   Implement provider registration, replacement, lookup and removal with explicit bounds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_selection_provider/registry.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(
    UmiWorkbenchSelectionProviderRegistry *registry)
{
    UmiWorkbenchSelectionProviderDescriptor *items;
    size_t next;
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    if (registry->capacity >=
        UMI_WORKBENCH_SELECTION_PROVIDER_MAX_PROVIDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next = registry->capacity == 0U ? 8U : registry->capacity * 2U;
    if (next > UMI_WORKBENCH_SELECTION_PROVIDER_MAX_PROVIDERS) {
        next = UMI_WORKBENCH_SELECTION_PROVIDER_MAX_PROVIDERS;
    }
    items = (UmiWorkbenchSelectionProviderDescriptor *)realloc(
        registry->items,
        next * sizeof(registry->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (next > registry->capacity) {
        memset(
            items + registry->capacity,
            0,
            (next - registry->capacity) * sizeof(items[0]));
    }
    registry->items = items;
    registry->capacity = next;
    return UMI_STATUS_OK;
}

void umi_workbench_selection_provider_registry_init(
    UmiWorkbenchSelectionProviderRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

void umi_workbench_selection_provider_registry_destroy(
    UmiWorkbenchSelectionProviderRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    memset(registry, 0, sizeof(*registry));
}

const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find(
    const UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;
    if (registry == NULL || provider_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].provider_id, provider_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find_panel(
    const UmiWorkbenchSelectionProviderRegistry *registry,
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

UmiStatus umi_workbench_selection_provider_registry_upsert(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const UmiWorkbenchSelectionProviderDescriptor *descriptor)
{
    size_t index;
    UmiStatus status;
    if (registry == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_descriptor_validate(descriptor);
    if (status != UMI_STATUS_OK) return status;

    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].provider_id,
                   descriptor->provider_id) == 0) {
            registry->items[index] = *descriptor;
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    status = ensure_capacity(registry);
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *descriptor;
    ++registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_selection_provider_registry_remove(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;
    if (registry == NULL || provider_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].provider_id, provider_id) == 0) {
            if (index + 1U < registry->count) {
                memmove(
                    &registry->items[index],
                    &registry->items[index + 1U],
                    (registry->count - index - 1U) *
                        sizeof(registry->items[0]));
            }
            --registry->count;
            memset(
                &registry->items[registry->count],
                0,
                sizeof(registry->items[0]));
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
