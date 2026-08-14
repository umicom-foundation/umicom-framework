/* Umicom Framework Tests | Workspace trust policy v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/security/workspace_policy.h"
int main(void)
{
    UmiSecurityPermissionDescriptor permission = {0};
    UmiSecurityGovernanceDecision decision;
    permission.risk = UMI_SECURITY_RISK_HIGH; permission.trusted_workspace_required = true;
    assert(umi_security_workspace_policy_evaluate(UMI_WORKSPACE_UNTRUSTED,&permission,&decision) == UMI_STATUS_OK);
    assert(!decision.allowed);
    assert(umi_security_workspace_policy_evaluate(UMI_WORKSPACE_TRUSTED,&permission,&decision) == UMI_STATUS_OK);
    assert(decision.allowed);
    return 0;
}
