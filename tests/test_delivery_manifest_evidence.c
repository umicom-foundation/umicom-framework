/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_manifest_evidence.c
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
#include "umicom/delivery/manifest.h"
#include "umicom/delivery/build_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDeliveryManifest manifest;
    UmiBuildEvidence evidence;
    assert(umi_delivery_manifest_init(&manifest, "studio", "r1", "0.14.0", UMI_RELEASE_DEVELOPMENT) == UMI_STATUS_OK);
    assert(umi_delivery_manifest_validate(&manifest) == UMI_STATUS_OK);
    umi_build_evidence_init(&evidence);
    evidence.build_succeeded = 1;
    evidence.tests_total = 5U;
    evidence.tests_passed = 5U;
    assert(umi_build_evidence_passed(&evidence));
    return 0;
}
