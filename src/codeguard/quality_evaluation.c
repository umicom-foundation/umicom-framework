/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/quality_evaluation.c
 *
 * PURPOSE:
 *   Implement the quality evaluation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Combined quality evaluation | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/quality_evaluation.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the codeguard quality policy default operation used by this module and its
 * client applications.
 */
void umi_codeguard_quality_policy_default(UmiCodeGuardQualityPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    policy->maximum_build_failures = 0U;
    policy->maximum_test_failures = 0U;
    policy->maximum_architecture_failures = 0U;
    policy->maximum_rule_failures = 0U;
    policy->maximum_breaking_api_changes = 0U;
    policy->duplicate_review_required = true;
}
/* Provide the failed kind operation used by this module and its client applications. */
static size_t failed_kind(const UmiCodeGuardEvidenceStore *store,UmiCodeGuardEvidenceKind kind)
{
    size_t index;
    size_t count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < store->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (store->items[index].kind == kind && store->items[index].state == UMI_CODEGUARD_EVIDENCE_FAIL) count += 1U;
    return count;
}
/* Return the number of records represented by breaking without changing their state. */
static size_t breaking_count(const UmiCodeGuardApiComparison *api)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (api == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < api->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (api->changes[index].level == UMI_CODEGUARD_BREAKING) count += 1U;
    return count;
}
/*
 * Provide the codeguard quality evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_quality_evaluate(const UmiCodeGuardQualityPolicy *policy,const UmiCodeGuardEvidenceStore *evidence,const UmiCodeGuardApiComparison *api,const UmiCodeGuardDuplicateReview *duplicates,UmiCodeGuardQualityEvaluation *out_evaluation)
{
    size_t index;
    size_t build;
    size_t tests;
    size_t architecture;
    size_t rules;
    int length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || evidence == NULL || api == NULL || duplicates == NULL || out_evaluation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_evaluation,0,sizeof(*out_evaluation));
    build = failed_kind(evidence,UMI_CODEGUARD_EVIDENCE_BUILD);
    tests = failed_kind(evidence,UMI_CODEGUARD_EVIDENCE_TEST);
    architecture = failed_kind(evidence,UMI_CODEGUARD_EVIDENCE_ARCHITECTURE);
    rules = failed_kind(evidence,UMI_CODEGUARD_EVIDENCE_RULE);
    out_evaluation->breaking_api_changes = breaking_count(api);
    out_evaluation->warnings = umi_codeguard_evidence_count_state(evidence,UMI_CODEGUARD_EVIDENCE_WARNING);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < duplicates->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (duplicates->items[index].disposition == UMI_CODEGUARD_CANDIDATE_REVIEW) out_evaluation->pending_duplicate_reviews += 1U;
    out_evaluation->failures = build + tests + architecture + rules + out_evaluation->breaking_api_changes;
    /* Apply this branch only when its contract condition is satisfied. */
    if (build > policy->maximum_build_failures || tests > policy->maximum_test_failures || architecture > policy->maximum_architecture_failures || rules > policy->maximum_rule_failures || out_evaluation->breaking_api_changes > policy->maximum_breaking_api_changes) out_evaluation->decision = UMI_CODEGUARD_QUALITY_FAIL;
    else /* Apply this branch only when its contract condition is satisfied. */ if (out_evaluation->warnings > 0U || (policy->duplicate_review_required && out_evaluation->pending_duplicate_reviews > 0U)) out_evaluation->decision = UMI_CODEGUARD_QUALITY_WARN;
    /* Use this fallback path when the earlier condition does not apply. */
    else out_evaluation->decision = UMI_CODEGUARD_QUALITY_PASS;
    length = snprintf(out_evaluation->summary,sizeof(out_evaluation->summary),"decision=%s failures=%zu warnings=%zu breaking-api=%zu duplicate-reviews=%zu",umi_codeguard_quality_decision_text(out_evaluation->decision),out_evaluation->failures,out_evaluation->warnings,out_evaluation->breaking_api_changes,out_evaluation->pending_duplicate_reviews);
    return length < 0 || (size_t)length >= sizeof(out_evaluation->summary) ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
}
