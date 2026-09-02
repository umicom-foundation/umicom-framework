/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/install_review.c
 * PURPOSE: Implement explicit, approval-aware package operation previews.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/install_review.h"

#include <string.h>

/*
 * Provide the ecosystem install review begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_install_review_begin(
    UmiEcosystemInstallReview *review,
    const UmiEcosystemPackageRecord *record)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_ecosystem_package_record_validate(record);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(review, 0, sizeof(*review));
    status = umi_ecosystem_copy_text(
        review->package_id, sizeof(review->package_id), record->package_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Apply this branch only when its contract condition is satisfied. */
    if (record->installed_version[0] != '\0') {
        status = umi_ecosystem_copy_text(review->from_version,
            sizeof(review->from_version), record->installed_version);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    status = umi_ecosystem_copy_text(review->to_version,
        sizeof(review->to_version), record->available_version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    review->update = record->installed;
    review->download_bytes = record->download_bytes;
    review->requires_restart = record->requires_restart;
    review->requires_migration = record->requires_migration;
    review->rollback_available = record->installed;
    review->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem install review add action operation used by this module and its
 * client applications.
 */
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
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || subject_id == NULL || summary == NULL ||
        subject_id[0] == '\0' || summary[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (review->action_count >= UMI_ECOSYSTEM_MAX_ACTIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    action = &review->actions[review->action_count];
    (void)memset(action, 0, sizeof(*action));
    status = umi_ecosystem_copy_text(
        action->subject_id, sizeof(action->subject_id), subject_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(
        action->summary, sizeof(action->summary), summary);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
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

/*
 * Provide the ecosystem install review set approved operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_install_review_set_approved(
    UmiEcosystemInstallReview *review,
    size_t action_index,
    bool approved)
{
    UmiEcosystemInstallAction *action;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || action_index >= review->action_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    action = &review->actions[action_index];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!action->requires_approval) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (action->severity == UMI_ECOSYSTEM_REVIEW_BLOCKER && approved) {
        return UMI_STATUS_INVALID_STATE;
    }
    action->approved = approved;
    review->revision++;
    umi_ecosystem_install_review_recalculate(review);
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem install review complete action operation used by this module and
 * its client applications.
 */
UmiStatus umi_ecosystem_install_review_complete_action(
    UmiEcosystemInstallReview *review,
    size_t action_index,
    UmiStatus status)
{
    UmiEcosystemInstallAction *action;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL || action_index >= review->action_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    action = &review->actions[action_index];
    action->completed = status == UMI_STATUS_OK;
    action->status = status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) action->severity = UMI_ECOSYSTEM_REVIEW_BLOCKER;
    review->revision++;
    umi_ecosystem_install_review_recalculate(review);
    return UMI_STATUS_OK;
}

/*
 * Provide the ecosystem install review recalculate operation used by this module and its
 * client applications.
 */
void umi_ecosystem_install_review_recalculate(
    UmiEcosystemInstallReview *review)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (review == NULL) return;
    review->blocker_count = 0U;
    review->warning_count = 0U;
    review->pending_approval_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < review->action_count; ++index) {
        const UmiEcosystemInstallAction *action = &review->actions[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (action->severity == UMI_ECOSYSTEM_REVIEW_BLOCKER) {
            review->blocker_count++;
        } else /* Apply this branch only when its contract condition is satisfied. */ if (action->severity == UMI_ECOSYSTEM_REVIEW_WARNING) {
            review->warning_count++;
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (action->required && action->requires_approval && !action->approved) {
            review->pending_approval_count++;
        }
    }
}

/*
 * Provide the ecosystem install review ready operation used by this module and its client
 * applications.
 */
bool umi_ecosystem_install_review_ready(
    const UmiEcosystemInstallReview *review)
{
    return review != NULL && review->package_id[0] != '\0' &&
        review->action_count > 0U && review->blocker_count == 0U &&
        review->pending_approval_count == 0U;
}
