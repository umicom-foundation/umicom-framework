/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_provenance_release.c
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
#include "umicom/delivery/provenance.h"
#include "umicom/delivery/release.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProvenance p;
    UmiDeliveryManifest m;
    UmiRelease r;
    assert(umi_provenance_init(&p, "abc", "builder", "debug") == UMI_STATUS_OK);
    assert(umi_provenance_validate(&p) == UMI_STATUS_OK);
    assert(umi_delivery_manifest_init(&m, "studio", "rel", "0.14.0", UMI_RELEASE_BETA) == UMI_STATUS_OK);
    assert(umi_release_init(&r, &m, 7U) == UMI_STATUS_OK);
    r.verification = UMI_EVIDENCE_PASS;
    assert(umi_release_ready_to_publish(&r));
    return 0;
}
