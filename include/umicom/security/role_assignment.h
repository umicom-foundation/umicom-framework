/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/role_assignment.h
 *
 * PURPOSE:
 *   Publish the public role assignment contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_ROLE_ASSIGNMENT_H
#define UMICOM_SECURITY_ROLE_ASSIGNMENT_H
#include "umicom/security/governance.h"
/**
 * Represent the security role assignment data shared with callers of this public contract.
 */
typedef struct UmiSecurityRoleAssignment {
    char assignment_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char identity_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char role_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char resource_scope[UMI_SECURITY_GOVERNANCE_PATH_CAPACITY];
    uint64_t expires_at_ns;
    bool enabled;
} UmiSecurityRoleAssignment;
/**
 * Represent the security role assignment store data shared with callers of this public
 * contract.
 */
typedef struct UmiSecurityRoleAssignmentStore { UmiSecurityRoleAssignment items[UMI_SECURITY_GOVERNANCE_MAX_ASSIGNMENTS]; size_t count; } UmiSecurityRoleAssignmentStore;
/**
 * Add security role assignment only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_security_role_assignment_add(UmiSecurityRoleAssignmentStore *store,const UmiSecurityRoleAssignment *assignment);
/**
 * Find security role assignment while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiSecurityRoleAssignment *umi_security_role_assignment_find(const UmiSecurityRoleAssignmentStore *store,const char *identity_id,const char *role_id,const char *resource,uint64_t now_ns);
/**
 * Provide the security role assignment revoke operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_role_assignment_revoke(UmiSecurityRoleAssignmentStore *store,const char *assignment_id);
#endif
