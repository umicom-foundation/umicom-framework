/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/role_policy.c
 *
 * PURPOSE:
 *   Implement the role policy behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Governed roles and permission grants | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/role_policy.h"
#include <stdio.h>
#include <string.h>
/* Provide the find mutable operation used by this module and its client applications. */
static UmiSecurityRolePolicy *find_mutable(UmiSecurityRolePolicyStore *store,const char *role_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || role_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(store->items[index].role_id,role_id) == 0) return &store->items[index];
    return NULL;
}
/*
 * Find security role policy while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSecurityRolePolicy *umi_security_role_policy_find(const UmiSecurityRolePolicyStore *store,const char *role_id) { return find_mutable((UmiSecurityRolePolicyStore *)(void *)store,role_id); }
/*
 * Provide the security role policy define operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_role_policy_define(UmiSecurityRolePolicyStore *store,const char *role_id,const char *description)
{
    UmiSecurityRolePolicy *role;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (store == NULL || role_id == NULL || description == NULL || role_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (find_mutable(store,role_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (store->count >= UMI_SECURITY_GOVERNANCE_MAX_ROLES) return UMI_STATUS_CAPACITY_EXCEEDED;
    role = &store->items[store->count];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (snprintf(role->role_id,sizeof(role->role_id),"%s",role_id) < 0 || snprintf(role->description,sizeof(role->description),"%s",description) < 0) return UMI_STATUS_INTERNAL_ERROR;
    store->count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the security role policy grant operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_role_policy_grant(UmiSecurityRolePolicyStore *store,const char *role_id,const char *permission)
{
    size_t index;
    UmiSecurityRolePolicy *role = find_mutable(store,role_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (role == NULL || permission == NULL || permission[0] == '\0') return UMI_STATUS_NOT_FOUND;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < role->permission_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(role->permissions[index],permission) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (role->permission_count >= UMI_SECURITY_GOVERNANCE_MAX_GRANTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if ((size_t)snprintf(role->permissions[role->permission_count],sizeof(role->permissions[0]),"%s",permission) >= sizeof(role->permissions[0])) return UMI_STATUS_CAPACITY_EXCEEDED;
    role->permission_count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the security role policy allows operation used by this module and its client
 * applications.
 */
bool umi_security_role_policy_allows(const UmiSecurityRolePolicyStore *store,const char *role_id,const char *permission)
{
    size_t index;
    const UmiSecurityRolePolicy *role = umi_security_role_policy_find(store,role_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (role == NULL || permission == NULL) return false;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < role->permission_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(role->permissions[index],permission) == 0) return true;
    return false;
}
