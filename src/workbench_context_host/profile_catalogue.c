/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/profile_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded dynamic profile insertion, replacement, lookup and removal.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/profile_catalogue.h"

#include <stdlib.h>
#include <string.h>

/* Provide the grow operation used by this module and its client applications. */
static UmiStatus grow(UmiWorkbenchContextHostProfileCatalogue *catalogue)
{
    UmiWorkbenchContextHostProfile *items;
    size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->count < catalogue->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (catalogue->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    next = catalogue->capacity == 0U ? 4U : catalogue->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES) {
        next = UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES;
    }
    items = (UmiWorkbenchContextHostProfile *)realloc(
        catalogue->items, next * sizeof(catalogue->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (next > catalogue->capacity) {
        memset(
            items + catalogue->capacity,
            0,
            (next - catalogue->capacity) * sizeof(items[0]));
    }
    catalogue->items = items;
    catalogue->capacity = next;
    return UMI_STATUS_OK;
}

/*
 * Initialise workbench context host profile catalogue from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_profile_catalogue_init(
    UmiWorkbenchContextHostProfileCatalogue *catalogue)
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
 * Release or reset state held by workbench context host profile catalogue so the same
 * storage can be reused safely.
 */
void umi_workbench_context_host_profile_catalogue_destroy(
    UmiWorkbenchContextHostProfileCatalogue *catalogue)
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
 * Find workbench context host profile catalogue while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiWorkbenchContextHostProfile *
umi_workbench_context_host_profile_catalogue_find(
    const UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const char *profile_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].profile_id, profile_id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}

/*
 * Provide the workbench context host profile catalogue upsert operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_profile_catalogue_upsert(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const UmiWorkbenchContextHostProfile *profile)
{
    UmiStatus status;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_host_profile_validate(profile);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].profile_id, profile->profile_id) == 0) {
            catalogue->items[index] = *profile;
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    status = grow(catalogue);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    catalogue->items[catalogue->count++] = *profile;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

/*
 * Remove workbench context host profile catalogue while keeping the remaining records in a
 * valid and discoverable state.
 */
UmiStatus umi_workbench_context_host_profile_catalogue_remove(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const char *profile_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < catalogue->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(catalogue->items[index].profile_id, profile_id) == 0) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (index + 1U < catalogue->count) {
                memmove(
                    &catalogue->items[index],
                    &catalogue->items[index + 1U],
                    (catalogue->count - index - 1U) *
                        sizeof(catalogue->items[0]));
            }
            --catalogue->count;
            memset(
                &catalogue->items[catalogue->count],
                0,
                sizeof(catalogue->items[0]));
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}
