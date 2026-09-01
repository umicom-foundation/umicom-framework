/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_release_registry.c
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
#include "umicom/delivery/release_registry.h"
int main(void) {
    UmiReleaseRegistry registry;
    UmiDeliveryManifest m;
    UmiRelease a, b;
    umi_release_registry_init(&registry);
    assert(umi_delivery_manifest_init(&m, "studio", "r1", "x", UMI_RELEASE_BETA) == UMI_STATUS_OK);
    assert(umi_release_init(&a, &m, 1U) == UMI_STATUS_OK);
    assert(umi_release_registry_add(&registry, &a) == UMI_STATUS_OK);
    assert(umi_delivery_manifest_init(&m, "studio", "r2", "x", UMI_RELEASE_BETA) == UMI_STATUS_OK);
    assert(umi_release_init(&b, &m, 2U) == UMI_STATUS_OK);
    assert(umi_release_registry_add(&registry, &b) == UMI_STATUS_OK);
    assert(umi_release_registry_latest(&registry, UMI_RELEASE_BETA)->generation == 2U);
    return 0;
}
