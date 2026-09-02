/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/provenance_review.c
 * PURPOSE: Evaluate package evidence against explicit supply-chain policy.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/provenance_review.h"

#include <string.h>

/*
 * Provide the ecosystem provenance policy default operation used by this module and its
 * client applications.
 */
UmiEcosystemProvenancePolicy umi_ecosystem_provenance_policy_default(void)
{
    UmiEcosystemProvenancePolicy policy;
    policy.require_verified_publisher = true;
    policy.require_signature = true;
    policy.require_checksum = true;
    policy.require_sbom = true;
    policy.require_licence = true;
    policy.require_trusted_source = true;
    return policy;
}

/*
 * Initialise ecosystem provenance review from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_provenance_review_init(
    UmiEcosystemProvenanceReview *review,
    const char *package_id)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL) return;
    (void)memset(review, 0, sizeof(*review));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (package_id != NULL) {
        (void)umi_ecosystem_copy_text(
            review->package_id, sizeof(review->package_id), package_id);
    }
    review->decision = UMI_ECOSYSTEM_EVIDENCE_UNKNOWN;
}

/* Provide the evaluate item operation used by this module and its client applications. */
static void evaluate_item(
    bool present,
    bool required,
    size_t *blockers,
    size_t *warnings,
    uint32_t *risk_score,
    uint32_t weight)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (present) return;
    /* Apply this branch only when its contract condition is satisfied. */
    if (required) {
        (*blockers)++;
        *risk_score += weight;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (*warnings)++;
        *risk_score += weight / 2U;
    }
}

/*
 * Provide the ecosystem provenance review evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_provenance_review_evaluate(
    UmiEcosystemProvenanceReview *review,
    const UmiEcosystemProvenancePolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || policy == NULL || review->package_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    review->blocker_count = 0U;
    review->warning_count = 0U;
    review->risk_score = 0U;
    evaluate_item(review->verified_publisher, policy->require_verified_publisher,
        &review->blocker_count, &review->warning_count, &review->risk_score, 20U);
    evaluate_item(review->signature_valid, policy->require_signature,
        &review->blocker_count, &review->warning_count, &review->risk_score, 30U);
    evaluate_item(review->checksum_valid, policy->require_checksum,
        &review->blocker_count, &review->warning_count, &review->risk_score, 20U);
    evaluate_item(review->sbom_present, policy->require_sbom,
        &review->blocker_count, &review->warning_count, &review->risk_score, 10U);
    evaluate_item(review->licence_present, policy->require_licence,
        &review->blocker_count, &review->warning_count, &review->risk_score, 5U);
    evaluate_item(review->source_trusted, policy->require_trusted_source,
        &review->blocker_count, &review->warning_count, &review->risk_score, 15U);
    /* Apply this branch only when its contract condition is satisfied. */
    if (review->risk_score > 100U) review->risk_score = 100U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (review->blocker_count > 0U) {
        review->decision = UMI_ECOSYSTEM_EVIDENCE_REJECTED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (review->warning_count > 0U) {
        review->decision = UMI_ECOSYSTEM_EVIDENCE_WARNING;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        review->decision = UMI_ECOSYSTEM_EVIDENCE_VERIFIED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem provenance review acceptable operation used by this module and its
 * client applications.
 */
bool umi_ecosystem_provenance_review_acceptable(
    const UmiEcosystemProvenanceReview *review)
{
    return review != NULL && review->package_id[0] != '\0' &&
        review->blocker_count == 0U &&
        (review->decision == UMI_ECOSYSTEM_EVIDENCE_VERIFIED ||
         review->decision == UMI_ECOSYSTEM_EVIDENCE_WARNING);
}
