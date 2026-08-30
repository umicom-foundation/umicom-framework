/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/update_review.h
 * PURPOSE: Model selectable package updates, channels and rollout consequences.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_UPDATE_REVIEW_H
#define UMICOM_DEVELOPER_ECOSYSTEM_UPDATE_REVIEW_H

#include "umicom/developer/ecosystem/package_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEcosystemUpdateCandidate {
    char package_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char from_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char to_version[UMI_ECOSYSTEM_VERSION_CAPACITY];
    char channel[UMI_ECOSYSTEM_ID_CAPACITY];
    UmiEcosystemCompatibility compatibility;
    UmiEcosystemEvidenceState evidence;
    bool selected;
    bool pinned;
    bool requires_restart;
    bool requires_migration;
    uint64_t download_bytes;
} UmiEcosystemUpdateCandidate;

typedef struct UmiEcosystemUpdateReview {
    UmiEcosystemUpdateCandidate candidates[UMI_ECOSYSTEM_MAX_UPDATES];
    size_t candidate_count;
    size_t selected_count;
    size_t blocked_count;
    size_t migration_count;
    size_t restart_count;
    uint64_t selected_download_bytes;
    uint64_t revision;
} UmiEcosystemUpdateReview;

void umi_ecosystem_update_review_init(UmiEcosystemUpdateReview *review);
UmiStatus umi_ecosystem_update_review_add(
    UmiEcosystemUpdateReview *review,
    const UmiEcosystemPackageRecord *record);
UmiStatus umi_ecosystem_update_review_set_selected(
    UmiEcosystemUpdateReview *review,
    const char *package_id,
    bool selected);
size_t umi_ecosystem_update_review_select_safe(
    UmiEcosystemUpdateReview *review);
void umi_ecosystem_update_review_recalculate(UmiEcosystemUpdateReview *review);
bool umi_ecosystem_update_candidate_selectable(
    const UmiEcosystemUpdateCandidate *candidate);
bool umi_ecosystem_update_review_ready(const UmiEcosystemUpdateReview *review);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_UPDATE_REVIEW_H */
