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
static size_t permission_find(const UmiPermissionSet *set, const char *value)
{
    size_t index;
    for (index = 0U; index < set->count; ++index) {
        if (strcmp(set->items[index], value) == 0) return index;
    }
    return SIZE_MAX;
}
static int permission_match(const char *pattern, const char *value)
{
    size_t length;
    if (strcmp(pattern, "*") == 0) return 1;
    length = strlen(pattern);
    if (length > 0U && pattern[length - 1U] == '*') {
        return strncmp(pattern, value, length - 1U) == 0;
    }
    return strcmp(pattern, value) == 0;
}
int umi_permission_name_valid(const char *permission)
{
    size_t index;
    if (permission == NULL || permission[0] == '\0' ||
        strlen(permission) >= UMI_PERMISSION_CAPACITY) return 0;
    for (index = 0U; permission[index] != '\0'; ++index) {
        unsigned char ch = (unsigned char)permission[index];
        if (!(isalnum(ch) || ch == '.' || ch == '-' || ch == '_' || ch == '*' || ch == ':')) {
            return 0;
        }
    }
    return 1;
}
UmiStatus umi_permission_set_create(UmiPermissionSet **out_set)
{
    UmiPermissionSet *set;
    UmiStatus status;
    if (out_set == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_set = NULL;
    set = (UmiPermissionSet *)calloc(1U, sizeof(*set));
    if (set == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&set->mutex);
    if (status != UMI_STATUS_OK) { free(set); return status; }
    *out_set = set;
    return UMI_STATUS_OK;
}
void umi_permission_set_destroy(UmiPermissionSet *set)
{
    if (set == NULL) return;
    umi_mutex_destroy(set->mutex);
    free(set);
}
UmiStatus umi_permission_set_add(UmiPermissionSet *set, const char *permission)
{
    UmiStatus status;
    if (set == NULL || !umi_permission_name_valid(permission)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(set->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (permission_find(set, permission) != SIZE_MAX) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_ALREADY_EXISTS;
    }
    if (set->count >= UMI_PERMISSION_SET_MAX) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)snprintf(set->items[set->count++], UMI_PERMISSION_CAPACITY, "%s", permission);
    (void)umi_mutex_unlock(set->mutex);
    return UMI_STATUS_OK;
}
UmiStatus umi_permission_set_remove(UmiPermissionSet *set, const char *permission)
{
    size_t index;
    UmiStatus status;
    if (set == NULL || permission == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(set->mutex);
    if (status != UMI_STATUS_OK) return status;
    index = permission_find(set, permission);
    if (index == SIZE_MAX) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (index + 1U < set->count) {
        (void)memmove(&set->items[index], &set->items[index + 1U],
                      (set->count - index - 1U) * sizeof(set->items[0]));
    }
    --set->count;
    (void)memset(&set->items[set->count], 0, sizeof(set->items[0]));
    (void)umi_mutex_unlock(set->mutex);
    return UMI_STATUS_OK;
}
int umi_permission_set_contains(const UmiPermissionSet *set, const char *permission)
{
    size_t index;
    int found = 0;
    if (set == NULL || permission == NULL) return 0;
    if (umi_mutex_lock(set->mutex) != UMI_STATUS_OK) return 0;
    for (index = 0U; index < set->count; ++index) {
        if (permission_match(set->items[index], permission)) { found = 1; break; }
    }
    (void)umi_mutex_unlock(set->mutex);
    return found;
}
size_t umi_permission_set_count(const UmiPermissionSet *set)
{
    size_t count = 0U;
    if (set != NULL && umi_mutex_lock(set->mutex) == UMI_STATUS_OK) {
        count = set->count;
        (void)umi_mutex_unlock(set->mutex);
    }
    return count;
}
UmiStatus umi_permission_set_at(const UmiPermissionSet *set, size_t index,
                                char *out_permission, size_t capacity)
{
    UmiStatus status;
    if (set == NULL || out_permission == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(set->mutex);
    if (status != UMI_STATUS_OK) return status;
    if (index >= set->count) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_NOT_FOUND;
    }
    if (strlen(set->items[index]) + 1U > capacity) {
        (void)umi_mutex_unlock(set->mutex);
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)strcpy(out_permission, set->items[index]);
    (void)umi_mutex_unlock(set->mutex);
    return UMI_STATUS_OK;
}
