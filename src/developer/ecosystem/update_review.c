/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/update_review.c
 * PURPOSE: Implement safe multi-package update selection and summaries.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/update_review.h"

#include <string.h>

void umi_ecosystem_update_review_init(UmiEcosystemUpdateReview *review)
{
    if (review == NULL) return;
    (void)memset(review, 0, sizeof(*review));
    review->revision = 1U;
}

bool umi_ecosystem_update_candidate_selectable(
    const UmiEcosystemUpdateCandidate *candidate)
{
    return candidate != NULL && !candidate->pinned &&
        candidate->compatibility != UMI_ECOSYSTEM_INCOMPATIBLE &&
        candidate->compatibility != UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN &&
        candidate->evidence != UMI_ECOSYSTEM_EVIDENCE_REJECTED &&
        candidate->evidence != UMI_ECOSYSTEM_EVIDENCE_UNKNOWN;
}

UmiStatus umi_ecosystem_update_review_add(
    UmiEcosystemUpdateReview *review,
    const UmiEcosystemPackageRecord *record)
{
    UmiEcosystemUpdateCandidate *candidate;
    UmiStatus status;
    if (review == NULL || record == NULL ||
        !umi_ecosystem_package_has_update(record)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (review->candidate_count >= UMI_ECOSYSTEM_MAX_UPDATES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    candidate = &review->candidates[review->candidate_count];
    (void)memset(candidate, 0, sizeof(*candidate));
    status = umi_ecosystem_copy_text(candidate->package_id,
        sizeof(candidate->package_id), record->package_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(candidate->from_version,
        sizeof(candidate->from_version), record->installed_version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(candidate->to_version,
        sizeof(candidate->to_version), record->available_version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(candidate->channel,
        sizeof(candidate->channel), record->channel[0] != '\0'
            ? record->channel : "stable");
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

UmiStatus umi_ecosystem_update_review_set_selected(
    UmiEcosystemUpdateReview *review,
    const char *package_id,
    bool selected)
{
    size_t index;
    if (review == NULL || package_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < review->candidate_count; ++index) {
        UmiEcosystemUpdateCandidate *candidate = &review->candidates[index];
        if (strcmp(candidate->package_id, package_id) == 0) {
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

size_t umi_ecosystem_update_review_select_safe(
    UmiEcosystemUpdateReview *review)
{
    size_t index;
    size_t selected = 0U;
    if (review == NULL) return 0U;
    for (index = 0U; index < review->candidate_count; ++index) {
        UmiEcosystemUpdateCandidate *candidate = &review->candidates[index];
        candidate->selected = umi_ecosystem_update_candidate_selectable(candidate) &&
            candidate->evidence == UMI_ECOSYSTEM_EVIDENCE_VERIFIED;
        if (candidate->selected) ++selected;
    }
    review->revision++;
    umi_ecosystem_update_review_recalculate(review);
    return selected;
}

void umi_ecosystem_update_review_recalculate(UmiEcosystemUpdateReview *review)
{
    size_t index;
    if (review == NULL) return;
    review->selected_count = 0U;
    review->blocked_count = 0U;
    review->migration_count = 0U;
    review->restart_count = 0U;
    review->selected_download_bytes = 0U;
    for (index = 0U; index < review->candidate_count; ++index) {
        const UmiEcosystemUpdateCandidate *candidate = &review->candidates[index];
        if (!umi_ecosystem_update_candidate_selectable(candidate)) {
            review->blocked_count++;
        }
        if (!candidate->selected) continue;
        review->selected_count++;
        review->selected_download_bytes += candidate->download_bytes;
        if (candidate->requires_migration) review->migration_count++;
        if (candidate->requires_restart) review->restart_count++;
    }
}

bool umi_ecosystem_update_review_ready(const UmiEcosystemUpdateReview *review)
{
    return review != NULL && review->selected_count > 0U;
}
