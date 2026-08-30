/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/ecosystem/provenance_review.h
 * PURPOSE: Summarise signed package provenance, SBOM and licence evidence.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_ECOSYSTEM_PROVENANCE_REVIEW_H
#define UMICOM_DEVELOPER_ECOSYSTEM_PROVENANCE_REVIEW_H

#include "umicom/developer/ecosystem/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEcosystemProvenancePolicy {
    bool require_verified_publisher;
    bool require_signature;
    bool require_checksum;
    bool require_sbom;
    bool require_licence;
    bool require_trusted_source;
} UmiEcosystemProvenancePolicy;

typedef struct UmiEcosystemProvenanceReview {
    char package_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char publisher[UMI_ECOSYSTEM_NAME_CAPACITY];
    char source_id[UMI_ECOSYSTEM_ID_CAPACITY];
    char signer[UMI_ECOSYSTEM_NAME_CAPACITY];
    char fingerprint[UMI_ECOSYSTEM_TEXT_CAPACITY];
    char checksum[UMI_ECOSYSTEM_TEXT_CAPACITY];
    char sbom_reference[UMI_ECOSYSTEM_PATH_CAPACITY];
    char licence_expression[UMI_ECOSYSTEM_NAME_CAPACITY];
    bool verified_publisher;
    bool signature_valid;
    bool checksum_valid;
    bool sbom_present;
    bool licence_present;
    bool source_trusted;
    size_t blocker_count;
    size_t warning_count;
    uint32_t risk_score;
    UmiEcosystemEvidenceState decision;
} UmiEcosystemProvenanceReview;

UmiEcosystemProvenancePolicy umi_ecosystem_provenance_policy_default(void);
void umi_ecosystem_provenance_review_init(
    UmiEcosystemProvenanceReview *review,
    const char *package_id);
UmiStatus umi_ecosystem_provenance_review_evaluate(
    UmiEcosystemProvenanceReview *review,
    const UmiEcosystemProvenancePolicy *policy);
bool umi_ecosystem_provenance_review_acceptable(
    const UmiEcosystemProvenanceReview *review);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_PROVENANCE_REVIEW_H */
