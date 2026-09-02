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

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(
    UmiWorkbenchSelectionProviderRegistry *registry)
{
    UmiWorkbenchSelectionProviderDescriptor *items;
    size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->count < registry->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (registry->capacity >=
        UMI_WORKBENCH_SELECTION_PROVIDER_MAX_PROVIDERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next = registry->capacity == 0U ? 8U : registry->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_WORKBENCH_SELECTION_PROVIDER_MAX_PROVIDERS) {
        next = UMI_WORKBENCH_SELECTION_PROVIDER_MAX_PROVIDERS;
    }
    items = (UmiWorkbenchSelectionProviderDescriptor *)realloc(
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
 * Initialise workbench selection provider registry from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_selection_provider_registry_init(
    UmiWorkbenchSelectionProviderRegistry *registry)
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
 * Release or reset state held by workbench selection provider registry so the same storage
 * can be reused safely.
 */
void umi_workbench_selection_provider_registry_destroy(
    UmiWorkbenchSelectionProviderRegistry *registry)
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
 * Find workbench selection provider registry while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find(
    const UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].provider_id, provider_id) == 0) {
            return &registry->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench selection provider registry find panel operation used by this
 * module and its client applications.
 */
const UmiWorkbenchSelectionProviderDescriptor *
umi_workbench_selection_provider_registry_find_panel(
    const UmiWorkbenchSelectionProviderRegistry *registry,
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
 * Provide the workbench selection provider registry upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_selection_provider_registry_upsert(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const UmiWorkbenchSelectionProviderDescriptor *descriptor)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || descriptor == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_selection_provider_descriptor_validate(descriptor);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].provider_id,
                   descriptor->provider_id) == 0) {
            registry->items[index] = *descriptor;
            ++registry->revision;
            return UMI_STATUS_OK;
        }
    }
    status = ensure_capacity(registry);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    registry->items[registry->count++] = *descriptor;
    ++registry->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench selection provider registry while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_selection_provider_registry_remove(
    UmiWorkbenchSelectionProviderRegistry *registry,
    const char *provider_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (registry == NULL || provider_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < registry->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(registry->items[index].provider_id, provider_id) == 0) {
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
