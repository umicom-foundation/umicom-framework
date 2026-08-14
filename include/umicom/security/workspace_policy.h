/* Umicom Framework | Workspace trust policy | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_SECURITY_WORKSPACE_POLICY_H
#define UMICOM_SECURITY_WORKSPACE_POLICY_H
#include "umicom/security/governance.h"
#include "umicom/security/workspace_trust.h"
#include "umicom/security/permission_catalogue_v2.h"
UmiStatus umi_security_workspace_policy_evaluate(UmiWorkspaceTrustLevel trust,const UmiSecurityPermissionDescriptor *permission,UmiSecurityGovernanceDecision *out_decision);
#endif
