/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/migration_review.c
 * PURPOSE: Implement approval-aware ecosystem migration previews.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/migration_review.h"

#include <string.h>

UmiStatus umi_ecosystem_migration_review_begin(
    UmiEcosystemMigrationReview *review,
    const char *subject_id,
    const char *from_version,
    const char *to_version,
    bool backup_required)
{
    UmiStatus status;
    if (review == NULL || subject_id == NULL || from_version == NULL ||
        to_version == NULL || subject_id[0] == '\0' || to_version[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(review, 0, sizeof(*review));
    status = umi_ecosystem_copy_text(review->subject_id,
        sizeof(review->subject_id), subject_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(review->from_version,
        sizeof(review->from_version), from_version);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(review->to_version,
        sizeof(review->to_version), to_version);
    if (status != UMI_STATUS_OK) return status;
    review->backup_required = backup_required;
    review->rollback_available = backup_required;
    review->revision = 1U;
    return UMI_STATUS_OK;
}

static UmiEcosystemMigrationStep *find_step(
    UmiEcosystemMigrationReview *review,
    const char *step_id)
{
    size_t index;
    if (review == NULL || step_id == NULL) return NULL;
    for (index = 0U; index < review->step_count; ++index) {
        if (strcmp(review->steps[index].step_id, step_id) == 0) {
            return &review->steps[index];
        }
    }
    return NULL;
}

UmiStatus umi_ecosystem_migration_review_add_step(
    UmiEcosystemMigrationReview *review,
    const char *step_id,
    const char *title,
    const char *detail,
    UmiEcosystemMigrationKind kind,
    UmiEcosystemReviewSeverity severity,
    bool required,
    bool automatic)
{
    UmiEcosystemMigrationStep *step;
    UmiStatus status;
    if (review == NULL || step_id == NULL || title == NULL || detail == NULL ||
        step_id[0] == '\0' || title[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_step(review, step_id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    if (review->step_count >= UMI_ECOSYSTEM_MAX_MIGRATION_STEPS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    step = &review->steps[review->step_count];
    (void)memset(step, 0, sizeof(*step));
    status = umi_ecosystem_copy_text(step->step_id,
        sizeof(step->step_id), step_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(step->title, sizeof(step->title), title);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ecosystem_copy_text(step->detail, sizeof(step->detail), detail);
    if (status != UMI_STATUS_OK) return status;
    step->kind = kind;
    step->severity = severity;
    step->required = required;
    step->automatic = automatic;
    step->accepted = automatic && severity != UMI_ECOSYSTEM_REVIEW_BLOCKER;
    step->status = severity == UMI_ECOSYSTEM_REVIEW_BLOCKER
        ? UMI_STATUS_INVALID_STATE : UMI_STATUS_OK;
    review->step_count++;
    review->revision++;
    umi_ecosystem_migration_review_recalculate(review);
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_migration_review_set_accepted(
    UmiEcosystemMigrationReview *review,
    const char *step_id,
    bool accepted)
{
    UmiEcosystemMigrationStep *step = find_step(review, step_id);
    if (step == NULL) return UMI_STATUS_NOT_FOUND;
    if (step->severity == UMI_ECOSYSTEM_REVIEW_BLOCKER && accepted) {
        return UMI_STATUS_INVALID_STATE;
    }
    step->accepted = accepted;
    review->revision++;
    umi_ecosystem_migration_review_recalculate(review);
    return UMI_STATUS_OK;
}

UmiStatus umi_ecosystem_migration_review_complete_step(
    UmiEcosystemMigrationReview *review,
    const char *step_id,
    UmiStatus status)
{
    UmiEcosystemMigrationStep *step = find_step(review, step_id);
    if (step == NULL) return UMI_STATUS_NOT_FOUND;
    step->completed = status == UMI_STATUS_OK;
    step->status = status;
    if (status != UMI_STATUS_OK) step->severity = UMI_ECOSYSTEM_REVIEW_BLOCKER;
    review->revision++;
    umi_ecosystem_migration_review_recalculate(review);
    return UMI_STATUS_OK;
}

void umi_ecosystem_migration_review_recalculate(
    UmiEcosystemMigrationReview *review)
{
    size_t index;
    if (review == NULL) return;
    review->blocker_count = 0U;
    review->warning_count = 0U;
    review->pending_acceptance_count = 0U;
    for (index = 0U; index < review->step_count; ++index) {
        const UmiEcosystemMigrationStep *step = &review->steps[index];
        if (step->severity == UMI_ECOSYSTEM_REVIEW_BLOCKER) {
            review->blocker_count++;
        } else if (step->severity == UMI_ECOSYSTEM_REVIEW_WARNING) {
            review->warning_count++;
        }
        if (step->required && !step->accepted) {
            review->pending_acceptance_count++;
        }
    }
}

bool umi_ecosystem_migration_review_ready(
    const UmiEcosystemMigrationReview *review)
{
    return review != NULL && review->step_count > 0U &&
        review->blocker_count == 0U && review->pending_acceptance_count == 0U;
}
