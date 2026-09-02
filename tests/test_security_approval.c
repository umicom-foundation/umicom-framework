/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_approval.c
 *
 * PURPOSE:
 *   Implement the test security approval behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Governed approvals v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include "umicom/security/approval.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiSecurityApprovalStore store = {0};
    UmiSecurityApproval approval = {0};
    (void)snprintf(approval.approval_id,sizeof(approval.approval_id),"approval-1");
    (void)snprintf(approval.subject_id,sizeof(approval.subject_id),"plugin.install");
    (void)snprintf(approval.requested_by,sizeof(approval.requested_by),"sammy");
    approval.subject_kind = UMI_SECURITY_APPROVAL_PLUGIN; approval.requested_at_ns = 1U; approval.expires_at_ns = 100U;
    assert(umi_security_approval_request(&store,&approval) == UMI_STATUS_OK);
    assert(umi_security_approval_decide(&store,"approval-1","security-admin",UMI_SECURITY_APPROVAL_APPROVED,"Reviewed package evidence") == UMI_STATUS_OK);
    assert(umi_security_approval_valid(umi_security_approval_find(&store,"approval-1",50U),50U));
    assert(umi_security_approval_find(&store,"approval-1",100U) == NULL);
    return 0;
}
