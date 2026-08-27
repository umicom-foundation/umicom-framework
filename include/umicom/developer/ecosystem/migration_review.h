/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/migration_review.h
 * PURPOSE: Model versioned manifest, project and component migration guidance.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_MIGRATION_REVIEW_H
#define UMICOM_DEVELOPER_ECOSYSTEM_MIGRATION_REVIEW_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEcosystemMigrationStep {
    char step_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char title[UMI_ECOSYSTEM_NAME_CAPACITY];
    char detail[UMI_ECOSYSTEM_TEXT_CAPACITY];
    UmiEcosystemMigrationKind kind;
    UmiEcosystemReviewSeverity severity;
    bool required;
    bool automatic;
    bool accepted;
    bool completed;
    UmiStatus status;
} UmiEcosystemMigrationStep;

typedef struct UmiEcosystemMigrationReview {
    char subject_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char from_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char to_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    UmiEcosystemMigrationStep steps[UMI_ECOSYSTEM_MAX_MIGRATION_STEPS];
    size_t step_count;
    size_t blocker_count;
    size_t warning_count;
    size_t pending_acceptance_count;
    bool backup_required;
    bool rollback_available;
    uint64_t revision;
} UmiEcosystemMigrationReview;

UmiStatus umi_ecosystem_migration_review_begin(
    UmiEcosystemMigrationReview *review,
    const char *subject_id,
    const char *from_version,
    const char *to_version,
    bool backup_required);
UmiStatus umi_ecosystem_migration_review_add_step(
    UmiEcosystemMigrationReview *review,
    const char *step_id,
    const char *title,
    const char *detail,
    UmiEcosystemMigrationKind kind,
    UmiEcosystemReviewSeverity severity,
    bool required,
    bool automatic);
UmiStatus umi_ecosystem_migration_review_set_accepted(
    UmiEcosystemMigrationReview *review,
    const char *step_id,
    bool accepted);
UmiStatus umi_ecosystem_migration_review_complete_step(
    UmiEcosystemMigrationReview *review,
    const char *step_id,
    UmiStatus status);
void umi_ecosystem_migration_review_recalculate(
    UmiEcosystemMigrationReview *review);
bool umi_ecosystem_migration_review_ready(
    const UmiEcosystemMigrationReview *review);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_MIGRATION_REVIEW_H */
