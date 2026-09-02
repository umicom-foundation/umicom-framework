/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/group_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded dynamic group-profile insertion, replacement, lookup and removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/group_catalogue.h"

#include <stdlib.h>
#include <string.h>

/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkGroupProfile *items;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count < catalogue->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = catalogue->capacity == 0U ? 4U : catalogue->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS;
    }
    items = (UmiWorkbenchContextLinkGroupProfile *)realloc(
        catalogue->items, new_capacity * sizeof(catalogue->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (new_capacity > catalogue->capacity) {
        memset(items + catalogue->capacity, 0,
               (new_capacity - catalogue->capacity) * sizeof(items[0]));
    }
    catalogue->items = items;
    catalogue->capacity = new_capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench context link group catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_group_catalogue_init(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

/*
 * Release or reset state held by workbench context link group catalogue so the same
 * storage can be reused safely.
 */
void umi_workbench_context_link_group_catalogue_destroy(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL) return;
    free(catalogue->items);
    memset(catalogue, 0, sizeof(*catalogue));
}

/*
 * Find workbench context link group catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || group_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].group_id, group_id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context link group catalogue find const operation used by this
 * module and its client applications.
 */
const UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find_const(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || group_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].group_id, group_id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context link group catalogue upsert operation used by this module
 * and its client applications.
 */
UmiStatus umi_workbench_context_link_group_catalogue_upsert(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const UmiWorkbenchContextLinkGroupProfile *profile)
{
    UmiWorkbenchContextLinkGroupProfile *existing;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_workbench_context_link_group_profile_validate(profile) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    existing = umi_workbench_context_link_group_catalogue_find(
        catalogue, profile->group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing != NULL) {
        *existing = *profile;
        ++existing->revision;
        ++catalogue->revision;
        return UMI_STATUS_OK;
    }
    status = ensure_capacity(catalogue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    catalogue->items[catalogue->count++] = *profile;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench context link group catalogue while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_link_group_catalogue_remove(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].group_id, group_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < catalogue->count) {
                memmove(&catalogue->items[index],
                        &catalogue->items[index + 1U],
                        (catalogue->count - index - 1U) * sizeof(catalogue->items[0]));
            }
            --catalogue->count;
            memset(&catalogue->items[catalogue->count], 0, sizeof(catalogue->items[0]));
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by workbench context link group catalogue
 * without changing their state.
 */
size_t umi_workbench_context_link_group_catalogue_count(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
