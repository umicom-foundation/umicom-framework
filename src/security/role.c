/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/role.c
 *
 * PURPOSE:
 *   Implement fixed-capacity role definitions and wildcard-aware permission
 *   grants.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/role.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "umicom/security/permission.h"
#include "umicom/platform/threading.h"
typedef struct UmiStoredRole {
    char role_id[UMI_ROLE_ID_CAPACITY];
    char description[UMI_ROLE_DESCRIPTION_CAPACITY];
    char permissions[UMI_ROLE_PERMISSION_MAX][UMI_PERMISSION_CAPACITY];
    size_t permission_count;
} UmiStoredRole;
struct UmiRoleRegistry { UmiStoredRole roles[UMI_ROLE_MAX]; size_t count; UmiMutex *mutex; };
static size_t role_find(const UmiRoleRegistry *registry, const char *role_id)
{
    size_t i;
    for (i = 0U; i < registry->count; ++i) if (strcmp(registry->roles[i].role_id, role_id) == 0) return i;
    return SIZE_MAX;
}
static int role_permission_match(const char *pattern, const char *permission)
{
    size_t n;
    if (strcmp(pattern, "*") == 0) return 1;
    n = strlen(pattern);
    return n > 0U && pattern[n - 1U] == '*'
        ? strncmp(pattern, permission, n - 1U) == 0
        : strcmp(pattern, permission) == 0;
}
UmiStatus umi_role_registry_create(UmiRoleRegistry **out_registry)
{
    UmiRoleRegistry *registry; UmiStatus status;
    if (out_registry == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_registry = NULL;
    registry = (UmiRoleRegistry *)calloc(1U, sizeof(*registry));
    if (registry == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&registry->mutex);
    if (status != UMI_STATUS_OK) { free(registry); return status; }
    *out_registry = registry; return UMI_STATUS_OK;
}
void umi_role_registry_destroy(UmiRoleRegistry *registry)
{ if (registry != NULL) { umi_mutex_destroy(registry->mutex); free(registry); } }
UmiStatus umi_role_registry_define(UmiRoleRegistry *registry,
                                   const char *role_id,
                                   const char *description)
{
    UmiStoredRole *role; UmiStatus status;
    if (registry == NULL || role_id == NULL || role_id[0] == '\0' || description == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (strlen(role_id) >= UMI_ROLE_ID_CAPACITY || strlen(description) >= UMI_ROLE_DESCRIPTION_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    if (role_find(registry, role_id) != SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    if (registry->count >= UMI_ROLE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    role = &registry->roles[registry->count++];
    (void)snprintf(role->role_id, sizeof(role->role_id), "%s", role_id);
    (void)snprintf(role->description, sizeof(role->description), "%s", description);
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_role_registry_grant(UmiRoleRegistry *registry,
                                  const char *role_id,
                                  const char *permission)
{
    size_t ri, i; UmiStoredRole *role; UmiStatus status;
    if (registry == NULL || role_id == NULL || !umi_permission_name_valid(permission)) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    ri = role_find(registry, role_id); if (ri == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    role = &registry->roles[ri];
    for (i = 0U; i < role->permission_count; ++i) if (strcmp(role->permissions[i], permission) == 0) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_ALREADY_EXISTS; }
    if (role->permission_count >= UMI_ROLE_PERMISSION_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_CAPACITY_EXCEEDED; }
    (void)snprintf(role->permissions[role->permission_count++], UMI_PERMISSION_CAPACITY, "%s", permission);
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
UmiStatus umi_role_registry_revoke(UmiRoleRegistry *registry,
                                   const char *role_id,
                                   const char *permission)
{
    size_t ri, i; UmiStoredRole *role; UmiStatus status;
    if (registry == NULL || role_id == NULL || permission == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    ri = role_find(registry, role_id); if (ri == SIZE_MAX) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    role = &registry->roles[ri];
    for (i = 0U; i < role->permission_count; ++i) {
        if (strcmp(role->permissions[i], permission) == 0) {
            if (i + 1U < role->permission_count) (void)memmove(&role->permissions[i], &role->permissions[i + 1U], (role->permission_count - i - 1U) * sizeof(role->permissions[0]));
            --role->permission_count; (void)memset(&role->permissions[role->permission_count], 0, sizeof(role->permissions[0]));
            (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
        }
    }
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND;
}
int umi_role_registry_allows(const UmiRoleRegistry *registry,
                             const char *role_id,
                             const char *permission)
{
    size_t ri, i; int allowed = 0;
    if (registry == NULL || role_id == NULL || permission == NULL) return 0;
    if (umi_mutex_lock(registry->mutex) != UMI_STATUS_OK) return 0;
    ri = role_find(registry, role_id);
    if (ri != SIZE_MAX) for (i = 0U; i < registry->roles[ri].permission_count; ++i) if (role_permission_match(registry->roles[ri].permissions[i], permission)) { allowed = 1; break; }
    (void)umi_mutex_unlock(registry->mutex); return allowed;
}
size_t umi_role_registry_count(const UmiRoleRegistry *registry)
{ size_t n = 0U; if (registry != NULL && umi_mutex_lock(registry->mutex) == UMI_STATUS_OK) { n = registry->count; (void)umi_mutex_unlock(registry->mutex); } return n; }
UmiStatus umi_role_registry_at(const UmiRoleRegistry *registry,
                               size_t index,
                               UmiRoleSnapshot *out_role)
{
    UmiStatus status;
    if (registry == NULL || out_role == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(registry->mutex); if (status != UMI_STATUS_OK) return status;
    if (index >= registry->count) { (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_NOT_FOUND; }
    (void)snprintf(out_role->role_id, sizeof(out_role->role_id), "%s", registry->roles[index].role_id);
    (void)snprintf(out_role->description, sizeof(out_role->description), "%s", registry->roles[index].description);
    out_role->permission_count = registry->roles[index].permission_count;
    (void)umi_mutex_unlock(registry->mutex); return UMI_STATUS_OK;
}
