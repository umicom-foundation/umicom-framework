/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/security_centre.c
 *
 * PURPOSE:
 *   Implement the security centre behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Security governance centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/security_centre.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise security governance centre from caller-provided values so later operations
 * receive a known state.
 */
void umi_security_governance_centre_init(UmiSecurityGovernanceCentre *centre)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL) return;
    (void)memset(centre,0,sizeof(*centre));
    umi_security_redaction_policy_default(&centre->redaction);
    centre->revision = 1U;
}
/* Provide the decide operation used by this module and its client applications. */
static void decide(UmiSecurityGovernanceDecision *decision,bool allowed,bool approval,const char *reason)
{
    decision->allowed = allowed; decision->approval_required = approval;
    (void)snprintf(decision->reason,sizeof(decision->reason),"%s",reason);
}
/* Provide the audit decision operation used by this module and its client applications. */
static UmiStatus audit_decision(UmiSecurityGovernanceCentre *centre,const UmiSecurityAccessRequest *request,const UmiSecurityGovernanceDecision *decision)
{
    UmiSecurityAuditEntry entry = {0};
    entry.timestamp_ns = request->now_ns; entry.correlation_id = request->correlation_id; entry.outcome = decision->allowed ? UMI_SECURITY_AUDIT_ALLOWED : UMI_SECURITY_AUDIT_DENIED;
    (void)snprintf(entry.principal,sizeof(entry.principal),"%s",request->identity_id);
    (void)snprintf(entry.action,sizeof(entry.action),"%s",request->permission);
    (void)snprintf(entry.resource,sizeof(entry.resource),"%s",request->resource);
    (void)snprintf(entry.reason,sizeof(entry.reason),"%s",decision->reason);
    return umi_security_audit_append(&centre->audit,&entry);
}
/*
 * Provide the security governance authorise operation used by this module and its client
 * applications.
 */
UmiStatus umi_security_governance_authorise(UmiSecurityGovernanceCentre *centre,const UmiSecurityAccessRequest *request,UmiSecurityGovernanceDecision *out_decision)
{
    const UmiSecurityIdentityProfile *identity;
    const UmiSecurityPermissionDescriptor *permission;
    const UmiSecurityRoleAssignment *assignment;
    const UmiSecurityApproval *approval;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL || request == NULL || out_decision == NULL || request->identity_id[0] == '\0' || request->role_id[0] == '\0' || request->permission[0] == '\0' || request->resource[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    decide(out_decision,false,false,"Default deny: no applicable allow decision");
    identity = umi_security_identity_directory_find(&centre->identities,request->identity_id);
    permission = umi_security_permission_catalogue_find(&centre->permissions,request->permission);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (identity == NULL || !identity->enabled) decide(out_decision,false,false,"Identity is missing or disabled");
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (permission == NULL) decide(out_decision,false,false,"Permission is not registered");
    /* Use this fallback path when the earlier condition does not apply. */
    else {
        assignment = umi_security_role_assignment_find(&centre->assignments,request->identity_id,request->role_id,request->resource,request->now_ns);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (assignment == NULL) decide(out_decision,false,false,"No active role assignment covers the resource");
        else /* Use the stable identifier comparison to choose the matching record or policy. */ if (!umi_security_role_policy_allows(&centre->roles,request->role_id,request->permission)) decide(out_decision,false,false,"Assigned role does not grant the permission");
        /* Use this fallback path when the earlier condition does not apply. */
        else {
            status = umi_security_workspace_policy_evaluate(request->workspace_trust,permission,out_decision);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
            /* Apply this operation only while the related capability or state is available. */
            if (out_decision->allowed && permission->approval_required) {
                approval = umi_security_approval_find(&centre->approvals,request->approval_id,request->now_ns);
                /* Use the stable identifier comparison to choose the matching record or policy. */
                if (!umi_security_approval_valid(approval,request->now_ns) || strcmp(approval->subject_id,request->permission) != 0) decide(out_decision,false,true,"Valid approval is required for this permission");
                /* Use this fallback path when the earlier condition does not apply. */
                else decide(out_decision,true,false,"Identity, role, workspace and approval policy satisfied");
            }
        }
    }
    status = audit_decision(centre,request,out_decision);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) centre->revision += 1U;
    return status;
}
/*
 * Provide the security governance snapshot operation used by this module and its client
 * applications.
 */
void umi_security_governance_snapshot(const UmiSecurityGovernanceCentre *centre,UmiSecurityGovernanceSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_snapshot == NULL) return;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (centre == NULL) return;
    out_snapshot->identities = centre->identities.count; out_snapshot->roles = centre->roles.count; out_snapshot->assignments = centre->assignments.count; out_snapshot->permissions = centre->permissions.count; out_snapshot->secret_references = centre->secrets.count; out_snapshot->approvals = centre->approvals.count; out_snapshot->trusted_signers = centre->signers.count; out_snapshot->audit_entries = centre->audit.count; out_snapshot->denied_entries = umi_security_audit_denied_count(&centre->audit); out_snapshot->revision = centre->revision;
}
