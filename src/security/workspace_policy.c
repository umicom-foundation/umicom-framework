/* Umicom Framework | Workspace trust policy | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/security/workspace_policy.h"
#include <stdio.h>
UmiStatus umi_security_workspace_policy_evaluate(UmiWorkspaceTrustLevel trust,const UmiSecurityPermissionDescriptor *permission,UmiSecurityGovernanceDecision *out_decision)
{
    if (permission == NULL || out_decision == NULL || trust < UMI_WORKSPACE_UNTRUSTED || trust > UMI_WORKSPACE_TRUSTED) return UMI_STATUS_INVALID_ARGUMENT;
    out_decision->allowed = true; out_decision->approval_required = permission->approval_required;
    if (permission->trusted_workspace_required && trust != UMI_WORKSPACE_TRUSTED) { out_decision->allowed = false; (void)snprintf(out_decision->reason,sizeof(out_decision->reason),"Permission %s requires a trusted workspace",permission->name); }
    else if (trust == UMI_WORKSPACE_UNTRUSTED && permission->risk >= UMI_SECURITY_RISK_HIGH) { out_decision->allowed = false; (void)snprintf(out_decision->reason,sizeof(out_decision->reason),"High-risk action denied in an untrusted workspace"); }
    else (void)snprintf(out_decision->reason,sizeof(out_decision->reason),"Workspace trust policy satisfied");
    return UMI_STATUS_OK;
}
