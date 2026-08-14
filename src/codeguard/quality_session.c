/* Umicom Framework | Architecture-quality session | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/quality_session.h"
#include <string.h>
void umi_codeguard_quality_session_init(UmiCodeGuardQualitySession *session)
{
    if (session == NULL) return;
    (void)memset(session,0,sizeof(*session));
    umi_codeguard_architecture_policy_default(&session->architecture);
    umi_codeguard_quality_policy_default(&session->policy);
    session->revision = 1U;
}
UmiStatus umi_codeguard_quality_session_record(UmiCodeGuardQualitySession *session,const UmiCodeGuardEvidence *evidence)
{
    UmiStatus status;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_evidence_add(&session->evidence,evidence);
    if (status == UMI_STATUS_OK) session->revision += 1U;
    return status;
}
UmiStatus umi_codeguard_quality_session_evaluate(UmiCodeGuardQualitySession *session)
{
    UmiStatus status;
    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_codeguard_quality_evaluate(&session->policy,&session->evidence,&session->api,&session->duplicates,&session->evaluation);
    if (status == UMI_STATUS_OK) status = umi_codeguard_remediation_plan_build(&session->evidence,&session->remediation);
    if (status == UMI_STATUS_OK) session->revision += 1U;
    return status;
}
