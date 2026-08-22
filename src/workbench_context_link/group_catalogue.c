/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_link/group_catalogue.c
 *
 * PURPOSE:
 *   Implement bounded dynamic group-profile insertion, replacement, lookup and removal.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_link/group_catalogue.h"

#include <stdlib.h>
#include <string.h>

static UmiStatus ensure_capacity(UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    size_t new_capacity;
    UmiWorkbenchContextLinkGroupProfile *items;
    if (catalogue->count < catalogue->capacity) return UMI_STATUS_OK;
    if (catalogue->capacity >= UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    new_capacity = catalogue->capacity == 0U ? 4U : catalogue->capacity * 2U;
    if (new_capacity > UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS) {
        new_capacity = UMI_WORKBENCH_CONTEXT_LINK_MAX_GROUPS;
    }
    items = (UmiWorkbenchContextLinkGroupProfile *)realloc(
        catalogue->items, new_capacity * sizeof(catalogue->items[0]));
    if (items == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    if (new_capacity > catalogue->capacity) {
        memset(items + catalogue->capacity, 0,
               (new_capacity - catalogue->capacity) * sizeof(items[0]));
    }
    catalogue->items = items;
    catalogue->capacity = new_capacity;
    return UMI_STATUS_OK;
}

void umi_workbench_context_link_group_catalogue_init(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    memset(catalogue, 0, sizeof(*catalogue));
    catalogue->revision = 1U;
}

void umi_workbench_context_link_group_catalogue_destroy(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    if (catalogue == NULL) return;
    free(catalogue->items);
    memset(catalogue, 0, sizeof(*catalogue));
}

UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id)
{
    size_t index;
    if (catalogue == NULL || group_id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].group_id, group_id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}

const UmiWorkbenchContextLinkGroupProfile *
umi_workbench_context_link_group_catalogue_find_const(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id)
{
    size_t index;
    if (catalogue == NULL || group_id == NULL) return NULL;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].group_id, group_id) == 0) {
            return &catalogue->items[index];
        }
    }
    return NULL;
}

UmiStatus umi_workbench_context_link_group_catalogue_upsert(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const UmiWorkbenchContextLinkGroupProfile *profile)
{
    UmiWorkbenchContextLinkGroupProfile *existing;
    UmiStatus status;
    if (catalogue == NULL || profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_workbench_context_link_group_profile_validate(profile) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    existing = umi_workbench_context_link_group_catalogue_find(
        catalogue, profile->group_id);
    if (existing != NULL) {
        *existing = *profile;
        ++existing->revision;
        ++catalogue->revision;
        return UMI_STATUS_OK;
    }
    status = ensure_capacity(catalogue);
    if (status != UMI_STATUS_OK) return status;
    catalogue->items[catalogue->count++] = *profile;
    ++catalogue->revision;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_context_link_group_catalogue_remove(
    UmiWorkbenchContextLinkGroupCatalogue *catalogue,
    const char *group_id)
{
    size_t index;
    if (catalogue == NULL || group_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < catalogue->count; ++index) {
        if (strcmp(catalogue->items[index].group_id, group_id) == 0) {
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

size_t umi_workbench_context_link_group_catalogue_count(
    const UmiWorkbenchContextLinkGroupCatalogue *catalogue)
{
    return catalogue == NULL ? 0U : catalogue->count;
}
