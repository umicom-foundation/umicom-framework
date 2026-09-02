/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/permission.c
 *
 * PURPOSE:
 *   Implement bounded permission sets with wildcard suffix matching and portable
 *   validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/permission.h"

#include <stddef.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "umicom/platform/threading.h"
struct UmiPermissionSet {
    char items[UMI_PERMISSION_SET_MAX][UMI_PERMISSION_CAPACITY];
    size_t count;
    UmiMutex *mutex;
};
/* Find permission while leaving the underlying catalogue or model owned by this module. */
static size_t permission_find(const UmiPermissionSet *set, const char *value)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(set->items[index], value) == 0) return index;
    }
    return SIZE_MAX;
}
/* Provide the permission match operation used by this module and its client applications. */
static int permission_match(const char *pattern, const char *value)
{
    size_t length;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(pattern, "*") == 0) return 1;
    length = strlen(pattern);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U && pattern[length - 1U] == '*') {
        return strncmp(pattern, value, length - 1U) == 0;
    }
    return strcmp(pattern, value) == 0;
}
/* Check that permission name satisfies its contract before another service relies on it. */
int umi_permission_name_valid(const char *permission)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (permission == NULL || permission[0] == '\0' ||
        strlen(permission) >= UMI_PERMISSION_CAPACITY) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; permission[index] != '\0'; ++index) {
        unsigned char ch = (unsigned char)permission[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!(isalnum(ch) || ch == '.' || ch == '-' || ch == '_' || ch == '*' || ch == ':')) {
            return 0;
        }
    }
    return 1;
}
/*
 * Initialise permission set from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_permission_set_create(UmiPermissionSet **out_set)
{
    UmiPermissionSet *set;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_set = NULL;
    set = (UmiPermissionSet *)calloc(1U, sizeof(*set));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&set->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { free(set); return status; }
    *out_set = set;
    return UMI_STATUS_OK;
}
/* Release or reset state held by permission set so the same storage can be reused safely. */
void umi_permission_set_destroy(UmiPermissionSet *set)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL) return;
    umi_mutex_destroy(set->mutex);
    free(set);
}
/* Add permission set only after its inputs and available capacity have been checked. */
UmiStatus umi_permission_set_add(UmiPermissionSet *set, const char *permission)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || !umi_permission_name_valid(permission)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(set->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (permission_find(set, permission) != SIZE_MAX) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (set->count >= UMI_PERMISSION_SET_MAX) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(set->items[set->count++], UMI_PERMISSION_CAPACITY, "%s", permission);
    (void)umi_mutex_unlock(set->mutex);
    return UMI_STATUS_OK;
}
/*
 * Remove permission set while keeping the remaining records in a valid and discoverable
 * state.
 */
UmiStatus umi_permission_set_remove(UmiPermissionSet *set, const char *permission)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || permission == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(set->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    index = permission_find(set, permission);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index + 1U < set->count) {
        (void)memmove(&set->items[index], &set->items[index + 1U],
                      (set->count - index - 1U) * sizeof(set->items[0]));
    }
    --set->count;
    (void)memset(&set->items[set->count], 0, sizeof(set->items[0]));
    (void)umi_mutex_unlock(set->mutex);
    return UMI_STATUS_OK;
}
/*
 * Provide the permission set contains operation used by this module and its client
 * applications.
 */
int umi_permission_set_contains(const UmiPermissionSet *set, const char *permission)
{
    size_t index;
    int found = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || permission == NULL) return 0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_mutex_lock(set->mutex) != UMI_STATUS_OK) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < set->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (permission_match(set->items[index], permission)) { found = 1; break; }
    }
    (void)umi_mutex_unlock(set->mutex);
    return found;
}
/* Return the number of records represented by permission set without changing their state. */
size_t umi_permission_set_count(const UmiPermissionSet *set)
{
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set != NULL && umi_mutex_lock(set->mutex) == UMI_STATUS_OK) {
        count = set->count;
        (void)umi_mutex_unlock(set->mutex);
    }
    return count;
}
/*
 * Find permission set while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_permission_set_at(const UmiPermissionSet *set, size_t index,
                                char *out_permission, size_t capacity)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (set == NULL || out_permission == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(set->mutex);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= set->count) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (strlen(set->items[index]) + 1U > capacity) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)strcpy(out_permission, set->items[index]);
    (void)umi_mutex_unlock(set->mutex);
    return UMI_STATUS_OK;
}
