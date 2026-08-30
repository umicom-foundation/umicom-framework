/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_runtime_bundle_v2.c
 *
 * PURPOSE:
 *   Verify portable bundle plans and required runtime dependency evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/delivery/delivery.h"

int main(void)
{
    UmiPackageManifestV2 manifest;
    UmiPortableBundlePlanV2 bundle;
    assert(umi_package_manifest_v2_init(
               &manifest, "studio", "0.23.0", "bin/studio",
               UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
    assert(umi_package_manifest_v2_add(
               &manifest, "build/studio", "bin/studio",
               UMI_PACKAGE_FILE_EXECUTABLE_V2, 1) == UMI_STATUS_OK);
    assert(umi_portable_bundle_v2_init(
               &bundle, "studio-win64", "windows-x64", "stage",
               "dist/studio.zip", &manifest) == UMI_STATUS_OK);
    assert(umi_portable_bundle_v2_add_dependency(
               &bundle, "libgcc_s_seh-1.dll", "bin/libgcc_s_seh-1.dll",
               UMI_DEPENDENCY_BUNDLE_V2, 1) == UMI_STATUS_OK);
    assert(umi_portable_bundle_v2_add_dependency(
               &bundle, "kernel32.dll", "", UMI_DEPENDENCY_SYSTEM_V2, 0) ==
           UMI_STATUS_OK);
    assert(umi_portable_bundle_v2_validate(&bundle) == UMI_STATUS_OK);
    assert(umi_portable_bundle_v2_file_count(&bundle) == 2U);
    return 0;
}
