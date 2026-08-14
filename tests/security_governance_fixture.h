/* Umicom Framework Tests | Security governance fixture | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_TEST_SECURITY_GOVERNANCE_FIXTURE_H
#define UMICOM_TEST_SECURITY_GOVERNANCE_FIXTURE_H
#include <stdio.h>
#include <string.h>
#include "umicom/security/security_admin.h"
static inline void umi_test_security_governance(UmiSecurityGovernanceCentre *centre)
{
    UmiSecurityIdentityProfile identity = {0};
    UmiSecurityPermissionDescriptor permission = {0};
    UmiSecurityRoleAssignment assignment = {0};
    umi_security_governance_centre_init(centre);
    (void)snprintf(identity.identity_id,sizeof(identity.identity_id),"sammy");
    (void)snprintf(identity.display_name,sizeof(identity.display_name),"Sammy Hegab");
    (void)snprintf(identity.organisation,sizeof(identity.organisation),"Umicom Foundation");
    identity.kind = UMI_IDENTITY_HUMAN; identity.assurance_level = 2U; identity.enabled = true;
    (void)umi_security_identity_directory_add(&centre->identities,&identity);
    (void)umi_security_role_policy_define(&centre->roles,"developer","Studio developer");
    (void)snprintf(permission.name,sizeof(permission.name),"project.build");
    (void)snprintf(permission.description,sizeof(permission.description),"Build a trusted project");
    permission.risk = UMI_SECURITY_RISK_MEDIUM; permission.trusted_workspace_required = true;
    (void)umi_security_permission_catalogue_register(&centre->permissions,&permission);
    (void)umi_security_role_policy_grant(&centre->roles,"developer","project.build");
    (void)snprintf(assignment.assignment_id,sizeof(assignment.assignment_id),"assignment-sammy-developer");
    (void)snprintf(assignment.identity_id,sizeof(assignment.identity_id),"sammy");
    (void)snprintf(assignment.role_id,sizeof(assignment.role_id),"developer");
    (void)snprintf(assignment.resource_scope,sizeof(assignment.resource_scope),"workspace://");
    assignment.enabled = true;
    (void)umi_security_role_assignment_add(&centre->assignments,&assignment);
}
#endif
