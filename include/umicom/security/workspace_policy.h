/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/security/workspace_policy.h
 *
 * PURPOSE:
 *   Publish the public workspace policy contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SECURITY_WORKSPACE_POLICY_H
#define UMICOM_SECURITY_WORKSPACE_POLICY_H
#include "umicom/security/governance.h"
#include "umicom/security/workspace_trust.h"
#include "umicom/security/permission_catalogue.h"
UmiStatus umi_security_workspace_policy_evaluate(UmiWorkspaceTrustLevel trust,const UmiSecurityPermissionDescriptor *permission,UmiSecurityGovernanceDecision *out_decision);
#endif
