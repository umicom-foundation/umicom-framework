/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_bundle_dependencies.c
 *
 * PURPOSE:
 *   Verify the delivery-platform behaviour exercised by this focused test.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This small executable uses assertions so a failure points directly at one delivery contract.
 */

#include <assert.h>
#include "umicom/delivery/runtime_bundle.h"
#include "umicom/delivery/dependency_scan.h"
int main(void) {
    UmiRuntimeBundle bundle;
    UmiDependencyScan scan;
    umi_runtime_bundle_init(&bundle);
    umi_dependency_scan_init(&scan);
    assert(umi_runtime_bundle_add(&bundle, "bin/studio") == UMI_STATUS_OK);
    assert(umi_dependency_scan_add(&scan, "gtk4") == UMI_STATUS_OK);
    assert(umi_runtime_bundle_contains(&bundle, "bin/studio"));
    assert(umi_dependency_scan_contains(&scan, "gtk4"));
    return 0;
}
