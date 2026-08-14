/* Umicom Framework | Security governance centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_CENTRE_V2_H
#define UMICOM_SECURITY_CENTRE_V2_H
#include "umicom/security/identity_directory.h"
#include "umicom/security/role_policy.h"
#include "umicom/security/role_assignment.h"
#include "umicom/security/permission_catalogue_v2.h"
#include "umicom/security/workspace_policy.h"
#include "umicom/security/secret_inventory.h"
#include "umicom/security/redaction_policy_v2.h"
#include "umicom/security/approval.h"
#include "umicom/security/package_trust.h"
#include "umicom/security/security_audit_v2.h"
typedef struct UmiSecurityAccessRequest {
    char identity_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char role_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char permission[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    char resource[UMI_SECURITY_GOVERNANCE_PATH_CAPACITY];
    UmiWorkspaceTrustLevel workspace_trust;
    UmiSecurityEnvironment environment;
    char approval_id[UMI_SECURITY_GOVERNANCE_ID_CAPACITY];
    uint64_t now_ns;
    uint64_t correlation_id;
} UmiSecurityAccessRequest;
typedef struct UmiSecurityGovernanceCentre {
    UmiSecurityIdentityDirectory identities;
    UmiSecurityRolePolicyStore roles;
    UmiSecurityRoleAssignmentStore assignments;
    UmiSecurityPermissionCatalogue permissions;
    UmiSecuritySecretInventory secrets;
    UmiSecurityRedactionPolicy redaction;
    UmiSecurityApprovalStore approvals;
    UmiSecuritySignerStore signers;
    UmiSecurityAuditTrail audit;
    uint64_t revision;
} UmiSecurityGovernanceCentre;
typedef struct UmiSecurityGovernanceSnapshot {
    size_t identities;
    size_t roles;
    size_t assignments;
    size_t permissions;
    size_t secret_references;
    size_t approvals;
    size_t trusted_signers;
    size_t audit_entries;
    size_t denied_entries;
    uint64_t revision;
} UmiSecurityGovernanceSnapshot;
void umi_security_governance_centre_init(UmiSecurityGovernanceCentre *centre);
UmiStatus umi_security_governance_authorise(UmiSecurityGovernanceCentre *centre,const UmiSecurityAccessRequest *request,UmiSecurityGovernanceDecision *out_decision);
void umi_security_governance_snapshot(const UmiSecurityGovernanceCentre *centre,UmiSecurityGovernanceSnapshot *out_snapshot);
#endif
