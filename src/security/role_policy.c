/* Umicom Framework | Governed roles and permission grants | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/role_policy.h"
#include <stdio.h>
#include <string.h>
static UmiSecurityRolePolicy *find_mutable(UmiSecurityRolePolicyStore *store,const char *role_id)
{
    size_t index;
    if (store == NULL || role_id == NULL) return NULL;
    for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].role_id,role_id) == 0) return &store->items[index];
    return NULL;
}
const UmiSecurityRolePolicy *umi_security_role_policy_find(const UmiSecurityRolePolicyStore *store,const char *role_id) { return find_mutable((UmiSecurityRolePolicyStore *)(void *)store,role_id); }
UmiStatus umi_security_role_policy_define(UmiSecurityRolePolicyStore *store,const char *role_id,const char *description)
{
    UmiSecurityRolePolicy *role;
    if (store == NULL || role_id == NULL || description == NULL || role_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    if (find_mutable(store,role_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= UMI_SECURITY_GOVERNANCE_MAX_ROLES) return UMI_STATUS_CAPACITY_EXCEEDED;
    role = &store->items[store->count];
    if (snprintf(role->role_id,sizeof(role->role_id),"%s",role_id) < 0 || snprintf(role->description,sizeof(role->description),"%s",description) < 0) return UMI_STATUS_INTERNAL_ERROR;
    store->count += 1U;
    return UMI_STATUS_OK;
}
UmiStatus umi_security_role_policy_grant(UmiSecurityRolePolicyStore *store,const char *role_id,const char *permission)
{
    size_t index;
    UmiSecurityRolePolicy *role = find_mutable(store,role_id);
    if (role == NULL || permission == NULL || permission[0] == '\0') return UMI_STATUS_NOT_FOUND;
    for (index = 0U; index < role->permission_count; ++index) if (strcmp(role->permissions[index],permission) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (role->permission_count >= UMI_SECURITY_GOVERNANCE_MAX_GRANTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    if ((size_t)snprintf(role->permissions[role->permission_count],sizeof(role->permissions[0]),"%s",permission) >= sizeof(role->permissions[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    role->permission_count += 1U;
    return UMI_STATUS_OK;
}
bool umi_security_role_policy_allows(const UmiSecurityRolePolicyStore *store,const char *role_id,const char *permission)
{
    size_t index;
    const UmiSecurityRolePolicy *role = umi_security_role_policy_find(store,role_id);
    if (role == NULL || permission == NULL) return false;
    for (index = 0U; index < role->permission_count; ++index) if (strcmp(role->permissions[index],permission) == 0) return true;
    return false;
}
