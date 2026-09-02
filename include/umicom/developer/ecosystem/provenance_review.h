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

/**
 * Represent the ecosystem provenance policy data shared with callers of this public
 * contract.
 */
typedef struct UmiEcosystemProvenancePolicy {
    bool require_verified_publisher;
    bool require_signature;
    bool require_checksum;
    bool require_sbom;
    bool require_licence;
    bool require_trusted_source;
} UmiEcosystemProvenancePolicy;

/**
 * Represent the ecosystem provenance review data shared with callers of this public
 * contract.
 */
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

/**
 * Provide the ecosystem provenance policy default operation used by this module and its
 * client applications.
 */
UmiEcosystemProvenancePolicy umi_ecosystem_provenance_policy_default(void);
/**
 * Initialise ecosystem provenance review from caller-provided values so later operations
 * receive a known state.
 */
void umi_ecosystem_provenance_review_init(
    UmiEcosystemProvenanceReview *review,
    const char *package_id);
/**
 * Provide the ecosystem provenance review evaluate operation used by this module and its
 * client applications.
 */
UmiStatus umi_ecosystem_provenance_review_evaluate(
    UmiEcosystemProvenanceReview *review,
    const UmiEcosystemProvenancePolicy *policy);
/**
 * Provide the ecosystem provenance review acceptable operation used by this module and its
 * client applications.
 */
bool umi_ecosystem_provenance_review_acceptable(
    const UmiEcosystemProvenanceReview *review);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DEVELOPER_ECOSYSTEM_PROVENANCE_REVIEW_H */
