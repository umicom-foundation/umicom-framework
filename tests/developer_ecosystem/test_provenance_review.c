/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_provenance_review.c
 * PURPOSE: Verify strict signed-provenance and supply-chain policy evaluation.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/provenance_review.h"

int main(void)
{
    UmiEcosystemProvenancePolicy policy =
        umi_ecosystem_provenance_policy_default();
    UmiEcosystemProvenanceReview review;
    umi_ecosystem_provenance_review_init(&review, "org.umicom.component");
    review.verified_publisher = true;
    review.signature_valid = true;
    review.checksum_valid = true;
    review.sbom_present = true;
    review.licence_present = true;
    review.source_trusted = true;
    assert(umi_ecosystem_provenance_review_evaluate(&review, &policy) ==
        UMI_STATUS_OK);
    assert(review.decision == UMI_ECOSYSTEM_EVIDENCE_VERIFIED);
    assert(umi_ecosystem_provenance_review_acceptable(&review));
    review.signature_valid = false;
    assert(umi_ecosystem_provenance_review_evaluate(&review, &policy) ==
        UMI_STATUS_OK);
    assert(review.decision == UMI_ECOSYSTEM_EVIDENCE_REJECTED);
    assert(review.risk_score == 30U);
    assert(!umi_ecosystem_provenance_review_acceptable(&review));
    return 0;
}
