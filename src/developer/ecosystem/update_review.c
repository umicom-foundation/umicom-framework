/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/update_review.c
 * PURPOSE: Implement safe multi-package update selection and summaries.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/update_review.h"

#include <string.h>

/*
 * Initialise ecosystem update review from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_update_review_init(UmiEcosystemUpdateReview *review)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL) return;
    (void)memset(review, 0, sizeof(*review));
    review->revision = 1U;
}

/*
 * Provide the ecosystem update candidate selectable operation used by this module and its
 * client applications.
 */
bool umi_ecosystem_update_candidate_selectable(
    const UmiEcosystemUpdateCandidate *candidate)
{
    return candidate != NULL && !candidate->pinned &&
        candidate->compatibility != UMI_ECOSYSTEM_INCOMPATIBLE &&
        candidate->compatibility != UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN &&
        candidate->evidence != UMI_ECOSYSTEM_EVIDENCE_REJECTED &&
        candidate->evidence != UMI_ECOSYSTEM_EVIDENCE_UNKNOWN;
}

/*
 * Add ecosystem update review only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ecosystem_update_review_add(
    UmiEcosystemUpdateReview *review,
    const UmiEcosystemPackageRecord *record)
{
    UmiEcosystemUpdateCandidate *candidate;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || record == NULL ||
        !umi_ecosystem_package_has_update(record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (review->candidate_count >= UMI_ECOSYSTEM_MAX_UPDATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    candidate = &review->candidates[review->candidate_count];
    (void)memset(candidate, 0, sizeof(*candidate));
    status = umi_ecosystem_copy_text(candidate->package_id,
        sizeof(candidate->package_id), record->package_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(candidate->from_version,
        sizeof(candidate->from_version), record->installed_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(candidate->to_version,
        sizeof(candidate->to_version), record->available_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(candidate->channel,
        sizeof(candidate->channel), record->channel[0] != '\0'
            ? record->channel : "stable");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    candidate->compatibility = record->compatibility;
    candidate->evidence = record->evidence;
    candidate->pinned = record->pinned;
    candidate->requires_restart = record->requires_restart;
    candidate->requires_migration = record->requires_migration;
    candidate->download_bytes = record->download_bytes;
    review->candidate_count++;
    review->revision++;
    umi_ecosystem_update_review_recalculate(review);
    return UMI_STATUS_OK;
}

/*
 * Find ecosystem update review set while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_ecosystem_update_review_set_selected(
    UmiEcosystemUpdateReview *review,
    const char *package_id,
    bool selected)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || package_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < review->candidate_count; ++index) {
        UmiEcosystemUpdateCandidate *candidate = &review->candidates[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(candidate->package_id, package_id) == 0) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (selected && !umi_ecosystem_update_candidate_selectable(candidate)) {
                return UMI_STATUS_PERMISSION_DENIED;
            }
            candidate->selected = selected;
            review->revision++;
            umi_ecosystem_update_review_recalculate(review);
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the ecosystem update review select safe operation used by this module and its
 * client applications.
 */
size_t umi_ecosystem_update_review_select_safe(
    UmiEcosystemUpdateReview *review)
{
    size_t index;
    size_t selected = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < review->candidate_count; ++index) {
        UmiEcosystemUpdateCandidate *candidate = &review->candidates[index];
        candidate->selected = umi_ecosystem_update_candidate_selectable(candidate) &&
            candidate->evidence == UMI_ECOSYSTEM_EVIDENCE_VERIFIED;
        /* Apply this branch only when its contract condition is satisfied. */
        if (candidate->selected) ++selected;
    }
    review->revision++;
    umi_ecosystem_update_review_recalculate(review);
    return selected;
}

/*
 * Provide the ecosystem update review recalculate operation used by this module and its
 * client applications.
 */
void umi_ecosystem_update_review_recalculate(UmiEcosystemUpdateReview *review)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL) return;
    review->selected_count = 0U;
    review->blocked_count = 0U;
    review->migration_count = 0U;
    review->restart_count = 0U;
    review->selected_download_bytes = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < review->candidate_count; ++index) {
        const UmiEcosystemUpdateCandidate *candidate = &review->candidates[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_ecosystem_update_candidate_selectable(candidate)) {
            review->blocked_count++;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!candidate->selected) continue;
        review->selected_count++;
        review->selected_download_bytes += candidate->download_bytes;
        /* Apply this branch only when its contract condition is satisfied. */
        if (candidate->requires_migration) review->migration_count++;
        /* Apply this branch only when its contract condition is satisfied. */
        if (candidate->requires_restart) review->restart_count++;
    }
}

/*
 * Provide the ecosystem update review ready operation used by this module and its client
 * applications.
 */
bool umi_ecosystem_update_review_ready(const UmiEcosystemUpdateReview *review)
{
    return review != NULL && review->selected_count > 0U;
}
