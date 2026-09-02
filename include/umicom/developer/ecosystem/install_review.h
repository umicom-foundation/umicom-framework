/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/install_review.h
 * PURPOSE: Represent reviewable package install/update actions before mutation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_INSTALL_REVIEW_H
#define UMICOM_DEVELOPER_ECOSYSTEM_INSTALL_REVIEW_H

#include "umicom/developer/ecosystem/package_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the ecosystem install action data shared with callers of this public contract.
 */
typedef struct UmiEcosystemInstallAction {
    UmiEcosystemActionKind kind;
    UmiEcosystemReviewSeverity severity;
    char subject_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char summary[UMI_ECOSYSTEM_TEXT_CAPACITY];
    bool required;
    bool requires_approval;
    bool approved;
    bool completed;
    UmiStatus status;
} UmiEcosystemInstallAction;

/**
 * Represent the ecosystem install review data shared with callers of this public contract.
 */
typedef struct UmiEcosystemInstallReview {
    char package_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char from_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char to_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    UmiEcosystemInstallAction actions[UMI_ECOSYSTEM_MAX_ACTIONS];
    size_t action_count;
    size_t blocker_count;
    size_t warning_count;
    size_t pending_approval_count;
    uint64_t download_bytes;
    bool update;
    bool requires_restart;
    bool requires_migration;
    bool rollback_available;
    uint64_t revision;
} UmiEcosystemInstallReview;

/**
 * Provide the ecosystem install review begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ecosystem_install_review_begin(
    UmiEcosystemInstallReview *review,
    const UmiEcosystemPackageRecord *record);
/**
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
    bool requires_approval);
/**
 * Provide the ecosystem install review set approved operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_install_review_set_approved(
    UmiEcosystemInstallReview *review,
    size_t action_index,
    bool approved);
/**
 * Provide the ecosystem install review complete action operation used by this module and
 * its client applications.
 */
UmiStatus umi_ecosystem_install_review_complete_action(
    UmiEcosystemInstallReview *review,
    size_t action_index,
    UmiStatus status);
/**
 * Provide the ecosystem install review recalculate operation used by this module and its
 * client applications.
 */
void umi_ecosystem_install_review_recalculate(
    UmiEcosystemInstallReview *review);
/**
 * Provide the ecosystem install review ready operation used by this module and its client
 * applications.
 */
bool umi_ecosystem_install_review_ready(
    const UmiEcosystemInstallReview *review);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_INSTALL_REVIEW_H */
