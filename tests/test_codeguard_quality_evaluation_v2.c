/* Umicom Framework Tests | Combined quality evaluation v2 | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/codeguard/quality_evaluation.h"
int main(void)
{
    UmiCodeGuardQualityPolicy policy;
    UmiCodeGuardEvidenceStore evidence = {0};
    UmiCodeGuardApiComparison api = {0};
    UmiCodeGuardDuplicateReview duplicates = {0};
    UmiCodeGuardQualityEvaluation evaluation;
    UmiCodeGuardEvidence item = {0};
    (void)umi_codeguard_quality_copy(item.id,sizeof(item.id),"tests"); item.kind = UMI_CODEGUARD_EVIDENCE_TEST; item.state = UMI_CODEGUARD_EVIDENCE_FAIL;
    assert(umi_codeguard_evidence_add(&evidence,&item) == UMI_STATUS_OK);
    umi_codeguard_quality_policy_default(&policy);
    assert(umi_codeguard_quality_evaluate(&policy,&evidence,&api,&duplicates,&evaluation) == UMI_STATUS_OK);
    assert(evaluation.decision == UMI_CODEGUARD_QUALITY_FAIL && evaluation.failures == 1U);
    return 0;
}
