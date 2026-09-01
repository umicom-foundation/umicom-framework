/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_verification_candidate.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/release_candidate.h"
int main(void) {
    UmiReleaseCandidate c;
    UmiDeliveryArtifact a;
    umi_release_candidate_init(&c);
    assert(umi_delivery_manifest_init(&c.manifest, "studio", "r", "0.14.0", UMI_RELEASE_DEVELOPMENT) == UMI_STATUS_OK);
    c.build.build_succeeded = 1; c.build.tests_total = 1U; c.build.tests_passed = 1U;
    assert(umi_delivery_artifact_init(&a, "app", UMI_ARTIFACT_EXECUTABLE, "bin/app") == UMI_STATUS_OK);
    assert(umi_artifact_set_add(&c.artifacts, &a) == UMI_STATUS_OK);
    assert(umi_verification_report_add(&c.verification, "integrity", UMI_EVIDENCE_PASS) == UMI_STATUS_OK);
    assert(umi_release_candidate_ready(&c));
    return 0;
}
