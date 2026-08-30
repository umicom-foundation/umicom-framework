/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/quality_evaluation.h
 *
 * PURPOSE:
 *   Publish the public quality evaluation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CODEGUARD_QUALITY_EVALUATION_H
#define UMICOM_CODEGUARD_QUALITY_EVALUATION_H
#include "umicom/codeguard/evidence.h"
#include "umicom/codeguard/api_baseline.h"
#include "umicom/codeguard/duplicate_evidence.h"
typedef struct UmiCodeGuardQualityPolicy {
    size_t maximum_build_failures;
    size_t maximum_test_failures;
    size_t maximum_architecture_failures;
    size_t maximum_rule_failures;
    size_t maximum_breaking_api_changes;
    bool duplicate_review_required;
} UmiCodeGuardQualityPolicy;
typedef struct UmiCodeGuardQualityEvaluation {
    UmiCodeGuardQualityDecision decision;
    size_t failures;
    size_t warnings;
    size_t pending_duplicate_reviews;
    size_t breaking_api_changes;
    char summary[UMI_CODEGUARD_QUALITY_TEXT_CAPACITY];
} UmiCodeGuardQualityEvaluation;
void umi_codeguard_quality_policy_default(UmiCodeGuardQualityPolicy *policy);
UmiStatus umi_codeguard_quality_evaluate(const UmiCodeGuardQualityPolicy *policy,const UmiCodeGuardEvidenceStore *evidence,const UmiCodeGuardApiComparison *api,const UmiCodeGuardDuplicateReview *duplicates,UmiCodeGuardQualityEvaluation *out_evaluation);
#endif
