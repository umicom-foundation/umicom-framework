/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_runtime_bundle.c
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
    UmiPackageManifest manifest;
    UmiPortableBundlePlan bundle;
    assert(umi_package_manifest_init(
               &manifest, "studio", "0.23.0", "bin/studio",
               UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
    assert(umi_package_manifest_add(
               &manifest, "build/studio", "bin/studio",
               UMI_PACKAGE_FILE_EXECUTABLE, 1) == UMI_STATUS_OK);
    assert(umi_portable_bundle_init(
               &bundle, "studio-win64", "windows-x64", "stage",
               "dist/studio.zip", &manifest) == UMI_STATUS_OK);
    assert(umi_portable_bundle_add_dependency(
               &bundle, "libgcc_s_seh-1.dll", "bin/libgcc_s_seh-1.dll",
               UMI_DEPENDENCY_BUNDLE, 1) == UMI_STATUS_OK);
    assert(umi_portable_bundle_add_dependency(
               &bundle, "kernel32.dll", "", UMI_DEPENDENCY_SYSTEM, 0) ==
           UMI_STATUS_OK);
    assert(umi_portable_bundle_validate(&bundle) == UMI_STATUS_OK);
    assert(umi_portable_bundle_file_count(&bundle) == 2U);
    return 0;
}
