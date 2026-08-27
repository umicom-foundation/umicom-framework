/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/install_review.c
 * PURPOSE: Implement explicit, approval-aware package operation previews.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/install_review.h"

#include <string.h>

UmiStatus umi_ecosystem_install_review_begin(
    UmiEcosystemInstallReview *review,
    const UmiEcosystemPackageRecord *record)
{
    UmiStatus status;
    if (review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ecosystem_package_record_validate(record);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(review, 0, sizeof(*review));
    status = umi_ecosystem_copy_text(
        review->package_id, sizeof(review->package_id), record->package_id);
    if (status != UMI_STATUS_OK) return status;
    if (record->installed_version[0] != '\0') {
        status = umi_ecosystem_copy_text(review->from_version,
            sizeof(review->from_version), record->installed_version);
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_ecosystem_copy_text(review->to_version,
        sizeof(review->to_version), record->available_version);
    if (status != UMI_STATUS_OK) return status;
    review->update = record->installed;
    review->download_bytes = record->download_bytes;
    review->requires_restart = record->requires_restart;
    review->requires_migration = record->requires_migration;
    review->rollback_available = record->installed;
    review->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_install_review_add_action(
    UmiEcosystemInstallReview *review,
    UmiEcosystemActionKind kind,
    const char *subject_id,
    const char *summary,
    UmiEcosystemReviewSeverity severity,
    bool required,
    bool requires_approval)
{
    UmiEcosystemInstallAction *action;
    UmiStatus status;
    if (review == NULL || subject_id == NULL || summary == NULL ||
        subject_id[0] == '\0' || summary[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (review->action_count >= UMI_ECOSYSTEM_MAX_ACTIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    action = &review->actions[review->action_count];
    (void)memset(action, 0, sizeof(*action));
    status = umi_ecosystem_copy_text(
        action->subject_id, sizeof(action->subject_id), subject_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(
        action->summary, sizeof(action->summary), summary);
    if (status != UMI_STATUS_OK) return status;
    action->kind = kind;
    action->severity = severity;
    action->required = required;
    action->requires_approval = requires_approval;
    action->approved = !requires_approval;
    action->status = severity == UMI_ECOSYSTEM_REVIEW_BLOCKER
        ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    review->action_count++;
    review->revision++;
    umi_ecosystem_install_review_recalculate(review);
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_install_review_set_approved(
    UmiEcosystemInstallReview *review,
    size_t action_index,
    bool approved)
{
    UmiEcosystemInstallAction *action;
    if (review == NULL || action_index >= review->action_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    action = &review->actions[action_index];
    if (!action->requires_approval) return UMI_STATUS_INVALID_STATE;
    if (action->severity == UMI_ECOSYSTEM_REVIEW_BLOCKER && approved) {
        return UMI_STATUS_INVALID_STATE;
    }
    action->approved = approved;
    review->revision++;
    umi_ecosystem_install_review_recalculate(review);
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_install_review_complete_action(
    UmiEcosystemInstallReview *review,
    size_t action_index,
    UmiStatus status)
{
    UmiEcosystemInstallAction *action;
    if (review == NULL || action_index >= review->action_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    action = &review->actions[action_index];
    action->completed = status == UMI_STATUS_OK;
    action->status = status;
    if (status != UMI_STATUS_OK) action->severity = UMI_ECOSYSTEM_REVIEW_BLOCKER;
    review->revision++;
    umi_ecosystem_install_review_recalculate(review);
    return UMI_STATUS_OK;
}

void umi_ecosystem_install_review_recalculate(
    UmiEcosystemInstallReview *review)
{
    size_t index;
    if (review == NULL) return;
    review->blocker_count = 0U;
    review->warning_count = 0U;
    review->pending_approval_count = 0U;
    for (index = 0U; index < review->action_count; ++index) {
        const UmiEcosystemInstallAction *action = &review->actions[index];
        if (action->severity == UMI_ECOSYSTEM_REVIEW_BLOCKER) {
            review->blocker_count++;
        } else if (action->severity == UMI_ECOSYSTEM_REVIEW_WARNING) {
            review->warning_count++;
        }
        if (action->required && action->requires_approval && !action->approved) {
            review->pending_approval_count++;
        }
    }
}

bool umi_ecosystem_install_review_ready(
    const UmiEcosystemInstallReview *review)
{
    return review != NULL && review->package_id[0] != '\0' &&
        review->action_count > 0U && review->blocker_count == 0U &&
        review->pending_approval_count == 0U;
}
