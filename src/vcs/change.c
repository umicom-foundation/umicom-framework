/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/change.c
 *
 * PURPOSE:
 *   Implement bounded version-control change ownership and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/change.h"

#include <stdlib.h>
#include <string.h>

struct UmiVcsChangeList {
    UmiVcsChange *items;
    size_t count;
    size_t capacity;
};

static UmiStatus ensure_change_capacity(UmiVcsChangeList *list)
{
    UmiVcsChange *resized;
    size_t next_capacity;
    if (list->count < list->capacity) return UMI_STATUS_OK;
    if (list->capacity >= UMI_VCS_MAX_CHANGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next_capacity = list->capacity == 0U ? 128U : list->capacity * 2U;
    if (next_capacity > UMI_VCS_MAX_CHANGES) {
        next_capacity = UMI_VCS_MAX_CHANGES;
    }
    resized = realloc(list->items,
                      next_capacity * sizeof(list->items[0]));
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized;
    list->capacity = next_capacity;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_change_list_create(UmiVcsChangeList **out_list)
{
    UmiVcsChangeList *list;
    if (out_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_list = NULL;
    list = (UmiVcsChangeList *)calloc(1U, sizeof(*list));
    if (list == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    list->capacity = 128U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    if (list->items == NULL) {
        free(list);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_list = list;
    return UMI_STATUS_OK;
}

void umi_vcs_change_list_destroy(UmiVcsChangeList *list)
{
    if (list != NULL) {
        free(list->items);
        free(list);
    }
}

void umi_vcs_change_list_clear(UmiVcsChangeList *list)
{
    if (list != NULL) {
        list->count = 0U;
        (void)memset(list->items, 0,
                     list->capacity * sizeof(list->items[0]));
    }
}

UmiStatus umi_vcs_change_list_add(UmiVcsChangeList *list,
                                  const UmiVcsChange *change)
{
    if (list == NULL || change == NULL || change->path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (umi_vcs_change_list_find(list, change->path) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    {
        UmiStatus status = ensure_change_capacity(list);
        if (status != UMI_STATUS_OK) return status;
    }
    list->items[list->count++] = *change;
    return UMI_STATUS_OK;
}

size_t umi_vcs_change_list_count(const UmiVcsChangeList *list)
{
    return list != NULL ? list->count : 0U;
}

const UmiVcsChange *umi_vcs_change_list_at(const UmiVcsChangeList *list,
                                           size_t index)
{
    return list != NULL && index < list->count
        ? &list->items[index]
        : NULL;
}

const UmiVcsChange *umi_vcs_change_list_find(const UmiVcsChangeList *list,
                                             const char *path)
{
    size_t index;
    if (list == NULL || path == NULL) {
        return NULL;
    }
    for (index = 0U; index < list->count; ++index) {
        if (strcmp(list->items[index].path, path) == 0) {
            return &list->items[index];
        }
    }
    return NULL;
}

size_t umi_vcs_change_list_staged_count(const UmiVcsChangeList *list)
{
    size_t index;
    size_t count = 0U;
    if (list == NULL) {
        return 0U;
    }
    for (index = 0U; index < list->count; ++index) {
        if (list->items[index].staged) {
            count += 1U;
        }
    }
    return count;
}
