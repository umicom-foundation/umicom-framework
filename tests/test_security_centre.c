/* Umicom Framework Tests | Security governance centre v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "security_governance_fixture.h"
int main(void)
{
    UmiSecurityGovernanceCentre centre;
    UmiSecurityAccessRequest request = {0};
    UmiSecurityGovernanceDecision decision;
    UmiSecurityGovernanceSnapshot snapshot;
    umi_test_security_governance(&centre);
    (void)snprintf(request.identity_id,sizeof(request.identity_id),"sammy"); (void)snprintf(request.role_id,sizeof(request.role_id),"developer"); (void)snprintf(request.permission,sizeof(request.permission),"project.build"); (void)snprintf(request.resource,sizeof(request.resource),"workspace://demo"); request.workspace_trust = UMI_WORKSPACE_TRUSTED; request.environment = UMI_SECURITY_ENV_DEVELOPMENT; request.now_ns = 10U; request.correlation_id = 56U;
    assert(umi_security_governance_authorise(&centre,&request,&decision) == UMI_STATUS_OK && decision.allowed);
    request.workspace_trust = UMI_WORKSPACE_UNTRUSTED;
    assert(umi_security_governance_authorise(&centre,&request,&decision) == UMI_STATUS_OK && !decision.allowed);
    umi_security_governance_snapshot(&centre,&snapshot);
    assert(snapshot.audit_entries == 2U && snapshot.denied_entries == 1U);
    assert(umi_security_catalogue_count() == 10U && strstr(umi_security_catalogue_at(0U),"identity-directory") != NULL);
    return 0;
}
