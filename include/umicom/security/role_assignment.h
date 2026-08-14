/* Umicom Framework | Scoped role assignments | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_ROLE_ASSIGNMENT_H
#define UMICOM_SECURITY_ROLE_ASSIGNMENT_H
#include "umicom/security/governance.h"
typedef struct UmiSecurityRoleAssignment {
    char assignment_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char identity_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char role_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char resource_scope[UMI_SECURITY_GOVERNANCE_PATH_CAPACITY];
    uint64_t expires_at_ns;
    bool enabled;
} UmiSecurityRoleAssignment;
typedef struct UmiSecurityRoleAssignmentStore { UmiSecurityRoleAssignment items[UMI_SECURITY_GOVERNANCE_MAX_ASSIGNMENTS]; size_t count; } UmiSecurityRoleAssignmentStore;
UmiStatus umi_security_role_assignment_add(UmiSecurityRoleAssignmentStore *store,const UmiSecurityRoleAssignment *assignment);
const UmiSecurityRoleAssignment *umi_security_role_assignment_find(const UmiSecurityRoleAssignmentStore *store,const char *identity_id,const char *role_id,const char *resource,uint64_t now_ns);
UmiStatus umi_security_role_assignment_revoke(UmiSecurityRoleAssignmentStore *store,const char *assignment_id);
#endif
