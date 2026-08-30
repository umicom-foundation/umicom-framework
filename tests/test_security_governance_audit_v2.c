/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_security_governance_audit_v2.c
 *
 * PURPOSE:
 *   Implement the test security governance audit v2 behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Governance audit v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdio.h>
#include "umicom/security/security_audit_v2.h"
int main(void)
{
    UmiSecurityAuditTrail trail = {0};
    UmiSecurityAuditEntry entry = {0};
    (void)snprintf(entry.principal,sizeof(entry.principal),"sammy"); (void)snprintf(entry.action,sizeof(entry.action),"plugin.install"); (void)snprintf(entry.resource,sizeof(entry.resource),"plugin://chart"); entry.outcome = UMI_SECURITY_AUDIT_DENIED;
    assert(umi_security_audit_append(&trail,&entry) == UMI_STATUS_OK);
    assert(umi_security_audit_at(&trail,0U)->sequence == 1U);
    assert(umi_security_audit_denied_count(&trail) == 1U);
    return 0;
}
