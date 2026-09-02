/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/role_policy.h
 *
 * PURPOSE:
 *   Publish the public role policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_ROLE_POLICY_H
#define UMICOM_SECURITY_ROLE_POLICY_H
#include "umicom/security/governance.h"
/**
 * Represent the security role policy data shared with callers of this public contract.
 */
typedef struct UmiSecurityRolePolicy {
    char role_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char description[UMI_SECURITY_GOVERNANCE_TEXT_CAPACITY];
    char permissions[UMI_SECURITY_GOVERNANCE_MAX_GRANTS][UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    size_t permission_count;
} UmiSecurityRolePolicy;
/**
 * Represent the security role policy store data shared with callers of this public
 * contract.
 */
typedef struct UmiSecurityRolePolicyStore { UmiSecurityRolePolicy items[UMI_SECURITY_GOVERNANCE_MAX_ROLES]; size_t count; } UmiSecurityRolePolicyStore;
/**
 * Provide the security role policy define operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_role_policy_define(UmiSecurityRolePolicyStore *store,const char *role_id,const char *description);
/**
 * Provide the security role policy grant operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_role_policy_grant(UmiSecurityRolePolicyStore *store,const char *role_id,const char *permission);
/**
 * Find security role policy while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiSecurityRolePolicy *umi_security_role_policy_find(const UmiSecurityRolePolicyStore *store,const char *role_id);
/**
 * Provide the security role policy allows operation used by this module and its client
 * applications.
 */
bool umi_security_role_policy_allows(const UmiSecurityRolePolicyStore *store,const char *role_id,const char *permission);
#endif
