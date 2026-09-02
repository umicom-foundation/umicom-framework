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

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(
    UmiWorkbenchContextSourceRegistry *registry)
{
    UmiWorkbenchContextSourceDefinition *items;
    size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->capacity >= UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next = registry->capacity == 0U ? 16U : registry->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES) {
        next = UMI_WORKBENCH_CONTEXT_SOURCE_MAX_SOURCES;
    }
    items = (UmiWorkbenchContextSourceDefinition *)realloc(
        registry->items,
        next * sizeof(registry->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Initialise workbench context source registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_source_registry_init(
    UmiWorkbenchContextSourceRegistry *registry)
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
 * Release or reset state held by workbench context source registry so the same storage can
 * be reused safely.
 */
void umi_workbench_context_source_registry_destroy(
    UmiWorkbenchContextSourceRegistry *registry)
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
 * Find workbench context source registry while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchContextSourceDefinition *
umi_workbench_context_source_registry_find(
    const UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || source_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].source_id, source_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context source registry upsert operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_context_source_registry_upsert(
    UmiWorkbenchContextSourceRegistry *registry,
    const UmiWorkbenchContextSourceDefinition *definition)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || definition == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_source_definition_validate(definition);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].source_id,
                   definition->source_id) == 0) {
            registry->items[index] = *definition;
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    status = ensure_capacity(registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *definition;
    ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench context source registry while keeping the remaining records in a valid
 * and discoverable state.
 */
UmiStatus umi_workbench_context_source_registry_remove(
    UmiWorkbenchContextSourceRegistry *registry,
    const char *source_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || source_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].source_id, source_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Return the number of records represented by workbench context source registry without
 * changing their state.
 */
size_t umi_workbench_context_source_registry_count(
    const UmiWorkbenchContextSourceRegistry *registry)
{
    return registry != NULL ? registry->count : 0U;
}
