/* Umicom Framework | Architecture-quality session | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_CODEGUARD_QUALITY_SESSION_H
#define UMICOM_CODEGUARD_QUALITY_SESSION_H
#include "umicom/codeguard/architecture_policy.h"
#include "umicom/codeguard/quality_evaluation.h"
#include "umicom/codeguard/remediation_plan.h"
typedef struct UmiCodeGuardQualitySession {
    UmiCodeGuardEvidenceStore evidence;
    UmiCodeGuardArchitecturePolicy architecture;
    UmiCodeGuardDuplicateReview duplicates;
    UmiCodeGuardApiComparison api;
    UmiCodeGuardQualityPolicy policy;
    UmiCodeGuardQualityEvaluation evaluation;
    UmiCodeGuardRemediationPlan remediation;
    uint64_t revision;
} UmiCodeGuardQualitySession;
void umi_codeguard_quality_session_init(UmiCodeGuardQualitySession *session);
UmiStatus umi_codeguard_quality_session_record(UmiCodeGuardQualitySession *session,const UmiCodeGuardEvidence *evidence);
UmiStatus umi_codeguard_quality_session_evaluate(UmiCodeGuardQualitySession *session);
#endif
