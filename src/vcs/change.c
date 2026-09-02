/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/change.c
 *
 * PURPOSE:
 *   Implement bounded version-control change ownership and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/change.h"

#include <stdlib.h>
#include <string.h>

struct UmiVcsChangeList {
    UmiVcsChange *items;
    size_t count;
    size_t capacity;
};

/*
 * Provide the ensure change capacity operation used by this module and its client
 * applications.
 */
static UmiStatus ensure_change_capacity(UmiVcsChangeList *list)
{
    UmiVcsChange *resized;
    size_t next_capacity;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->count < list->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (list->capacity >= UMI_VCS_MAX_CHANGES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    next_capacity = list->capacity == 0U ? 128U : list->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next_capacity > UMI_VCS_MAX_CHANGES) {
        next_capacity = UMI_VCS_MAX_CHANGES;
    }
    resized = realloc(list->items,
                      next_capacity * sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    list->items = resized;
    list->capacity = next_capacity;
    return UMI_STATUS_OK;
}

/*
 * Initialise vcs change list from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_change_list_create(UmiVcsChangeList **out_list)
{
    UmiVcsChangeList *list;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_list == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_list = NULL;
    list = (UmiVcsChangeList *)calloc(1U, sizeof(*list));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    list->capacity = 128U;
    list->items = calloc(list->capacity, sizeof(list->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list->items == NULL) {
        free(list);
        return UMI_STATUS_OUT_OF_MEMORY;
    }
    *out_list = list;
    return UMI_STATUS_OK;
}

/* Release or reset state held by vcs change list so the same storage can be reused safely. */
void umi_vcs_change_list_destroy(UmiVcsChangeList *list)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list != NULL) {
        free(list->items);
        free(list);
    }
}

/* Release or reset state held by vcs change list so the same storage can be reused safely. */
void umi_vcs_change_list_clear(UmiVcsChangeList *list)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list != NULL) {
        list->count = 0U;
        (void)memset(list->items, 0,
                     list->capacity * sizeof(list->items[0]));
    }
}

/* Add vcs change list only after its inputs and available capacity have been checked. */
UmiStatus umi_vcs_change_list_add(UmiVcsChangeList *list,
                                  const UmiVcsChange *change)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || change == NULL || change->path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_vcs_change_list_find(list, change->path) != NULL) {
        return UMI_STATUS_ALREADY_EXISTS;
    }
    {
        UmiStatus status = ensure_change_capacity(list);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    list->items[list->count++] = *change;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by vcs change list without changing their
 * state.
 */
size_t umi_vcs_change_list_count(const UmiVcsChangeList *list)
{
    return list != NULL ? list->count : 0U;
}

/*
 * Find vcs change list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsChange *umi_vcs_change_list_at(const UmiVcsChangeList *list,
                                           size_t index)
{
    return list != NULL && index < list->count
        ? &list->items[index]
        : NULL;
}

/*
 * Find vcs change list while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsChange *umi_vcs_change_list_find(const UmiVcsChangeList *list,
                                             const char *path)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL || path == NULL) {
        return NULL;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(list->items[index].path, path) == 0) {
            return &list->items[index];
        }
    }
    return NULL;
}

/*
 * Return the number of records represented by vcs change list staged without changing
 * their state.
 */
size_t umi_vcs_change_list_staged_count(const UmiVcsChangeList *list)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (list == NULL) {
        return 0U;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < list->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (list->items[index].staged) {
            count += 1U;
        }
    }
    return count;
}
