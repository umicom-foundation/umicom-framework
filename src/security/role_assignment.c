/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/role_assignment.c
 *
 * PURPOSE:
 *   Implement the role assignment behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Scoped role assignments | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/role_assignment.h"
#include <string.h>
UmiStatus umi_security_role_assignment_add(UmiSecurityRoleAssignmentStore *store,const UmiSecurityRoleAssignment *assignment)
{
    size_t index;
    if (store == NULL || assignment == NULL || assignment->assignment_id[0] == '\0' || assignment->identity_id[0] == '\0' || assignment->role_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].assignment_id,assignment->assignment_id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    if (store->count >= UMI_SECURITY_GOVERNANCE_MAX_ASSIGNMENTS) return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count++] = *assignment;
    return UMI_STATUS_OK;
}
const UmiSecurityRoleAssignment *umi_security_role_assignment_find(const UmiSecurityRoleAssignmentStore *store,const char *identity_id,const char *role_id,const char *resource,uint64_t now_ns)
{
    size_t index;
    if (store == NULL || identity_id == NULL || role_id == NULL || resource == NULL) return NULL;
    for (index = 0U; index < store->count; ++index) {
        const UmiSecurityRoleAssignment *item = &store->items[index];
        bool scope_matches = item->resource_scope[0] == '\0' || strcmp(item->resource_scope,"*") == 0 || strncmp(resource,item->resource_scope,strlen(item->resource_scope)) == 0;
        bool active = item->enabled && (item->expires_at_ns == 0U || now_ns < item->expires_at_ns);
        if (active && scope_matches && strcmp(item->identity_id,identity_id) == 0 && strcmp(item->role_id,role_id) == 0) return item;
    }
    return NULL;
}
UmiStatus umi_security_role_assignment_revoke(UmiSecurityRoleAssignmentStore *store,const char *assignment_id)
{
    size_t index;
    if (store == NULL || assignment_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < store->count; ++index) if (strcmp(store->items[index].assignment_id,assignment_id) == 0) { store->items[index].enabled = false; return UMI_STATUS_OK; }
    return UMI_STATUS_NOT_FOUND;
}
