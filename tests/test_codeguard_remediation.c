/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_codeguard_remediation.c
 *
 * PURPOSE:
 *   Implement the test codeguard remediation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Remediation plan v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <string.h>
#include "umicom/codeguard/remediation_plan.h"
int main(void)
{
    UmiCodeGuardEvidenceStore evidence = {0};
    UmiCodeGuardEvidence item = {0};
    UmiCodeGuardRemediationPlan plan;
    (void)umi_codeguard_quality_copy(item.id,sizeof(item.id),"unsafe"); item.kind = UMI_CODEGUARD_EVIDENCE_RULE; item.state = UMI_CODEGUARD_EVIDENCE_FAIL; item.line = 7U;
    (void)umi_codeguard_quality_copy(item.path,sizeof(item.path),"src/a.c");
    (void)umi_codeguard_quality_copy(item.remediation,sizeof(item.remediation),"Use bounded copy");
    assert(umi_codeguard_evidence_add(&evidence,&item) == UMI_STATUS_OK);
    assert(umi_codeguard_remediation_plan_build(&evidence,&plan) == UMI_STATUS_OK);
    assert(plan.count == 1U && strcmp(umi_codeguard_remediation_plan_at(&plan,0U)->instruction,"Use bounded copy") == 0);
    return 0;
}
