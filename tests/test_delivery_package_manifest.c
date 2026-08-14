/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_package_manifest.c
 *
 * PURPOSE:
 *   Verify product package manifests and entrypoint validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/delivery/delivery.h"

int main(void)
{
    UmiPackageManifest manifest;
    assert(umi_package_manifest_init(
               &manifest, "umicom-studio", "0.23.0", "bin/umicom-studio-console",
               UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
    assert(umi_package_manifest_validate(&manifest) ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_package_manifest_add(
               &manifest, "build/bin/umicom-studio-console",
               "bin/umicom-studio-console", UMI_PACKAGE_FILE_EXECUTABLE,
               1) == UMI_STATUS_OK);
    assert(umi_package_manifest_add(
               &manifest, "LICENSE", "share/licenses/LICENSE",
               UMI_PACKAGE_FILE_LICENCE, 1) == UMI_STATUS_OK);
    assert(umi_package_manifest_validate(&manifest) == UMI_STATUS_OK);
    assert(umi_package_manifest_find(
               &manifest, "share/licenses/LICENSE") != NULL);
    return 0;
}
