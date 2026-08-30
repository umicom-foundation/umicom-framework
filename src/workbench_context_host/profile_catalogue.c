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

static UmiStatus grow(UmiWorkbenchContextHostProfileCatalogue *catalogue)
{
    UmiWorkbenchContextHostProfile *items;
    size_t next;
    if (catalogue->count < catalogue->capacity) return UMI_STATUS_OK;
    if (catalogue->capacity >= UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    next = catalogue->capacity == 0U ? 4U : catalogue->capacity * 2U;
    if (next > UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES) {
        next = UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES;
    }
    items = (UmiWorkbenchContextHostProfile *)realloc(
        catalogue->items, next * sizeof(catalogue->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
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

void umi_workbench_context_host_profile_catalogue_init(
    UmiWorkbenchContextHostProfileCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

void umi_workbench_context_host_profile_catalogue_destroy(
    UmiWorkbenchContextHostProfileCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    free(catalogue->items);
    memset(catalogue, 0, sizeof(*catalogue));
}

const UmiWorkbenchContextHostProfile *
umi_workbench_context_host_profile_catalogue_find(
    const UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const char *profile_id)
{
    size_t index;
    if (catalogue == NULL || profile_id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].profile_id, profile_id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_context_host_profile_catalogue_upsert(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const UmiWorkbenchContextHostProfile *profile)
{
    UmiStatus status;
    size_t index;
    if (catalogue == NULL || profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_workbench_context_host_profile_validate(profile);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].profile_id, profile->profile_id) == 0) {
            catalogue->items[index] = *profile;
            ++catalogue->revision;
            return UMI_STATUS_OK;
        }
    }
    status = grow(catalogue);
    if (status != UMI_STATUS_OK) return status;
    catalogue->items[catalogue->count++] = *profile;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_host_profile_catalogue_remove(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const char *profile_id)
{
    size_t index;
    if (catalogue == NULL || profile_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].profile_id, profile_id) == 0) {
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
