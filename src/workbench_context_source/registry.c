/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_source/registry.c
 *
 * PURPOSE:
 *   Implement bounded dynamic source definition registration, replacement, lookup and removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_source/registry.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(
    UmiWorkbenchContextSourceRegistry *registry)
{
    UmiWorkbenchContextSourceDefinition *items;
    size_t next;
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    if (registry->capacity >= UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next = registry->capacity == 0U ? 16U : registry->capacity * 2U;
    if (next > UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES) {
        next = UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES;
    }
    items = (UmiWorkbenchContextSourceDefinition *)realloc(
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

void umi_workbench_context_source_registry_init(
    UmiWorkbenchContextSourceRegistry *registry)
{
    if (registry == NULL) return;
    memset(registry, 0, sizeof(*registry));
    registry->revision = 1U;
}

void umi_workbench_context_source_registry_destroy(
    UmiWorkbenchContextSourceRegistry *registry)
{
    if (registry == NULL) return;
    free(registry->items);
    memset(registry, 0, sizeof(*registry));
}

const UmiWorkbenchContextSourceDefinition *
umi_workbench_context_source_registry_find(
    const UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id)
{
    size_t index;
    if (registry == NULL || source_id == NULL) return NULL;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].source_id, source_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_context_source_registry_upsert(
    UmiWorkbenchContextSourceRegistry *registry,
    const UmiWorkbenchContextSourceDefinition *definition)
{
    size_t index;
    UmiStatus status;
    if (registry == NULL || definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_definition_validate(definition);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].source_id,
                   definition->source_id) == 0) {
            registry->items[index] = *definition;
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    status = ensure_capacity(registry);
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *definition;
    ++registry->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_source_registry_remove(
    UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id)
{
    size_t index;
    if (registry == NULL || source_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < registry->count; ++index) {
        if (strcmp(registry->items[index].source_id, source_id) == 0) {
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

size_t umi_workbench_context_source_registry_count(
    const UmiWorkbenchContextSourceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
