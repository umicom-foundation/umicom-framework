/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_product_release_v2.c
 *
 * PURPOSE:
 *   Verify complete package, installer, evidence and update release readiness.
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
    UmiPackageManifestV2 package;
    UmiUpdateChannelV2 channel;
    UmiInstallerPlanV2 installer;
    UmiProductReleaseV2 release;
    const char *checks[] = {
        "tests", "checksums", "signatures", "licences", "sbom", "provenance"
    };
    size_t index;
    assert(umi_package_manifest_v2_init(
               &package, "umicom-studio", "0.23.0", "bin/studio.exe",
               UMI_PACKAGE_WINDOWS_SETUP) == UMI_STATUS_OK);
    assert(umi_package_manifest_v2_add(
               &package, "build/studio.exe", "bin/studio.exe",
               UMI_PACKAGE_FILE_EXECUTABLE_V2, 1) == UMI_STATUS_OK);
    assert(umi_update_channel_v2_init(
               &channel, "stable", "https://updates.umicom.org/studio/stable.json",
               UMI_RELEASE_STABLE, 100U) == UMI_STATUS_OK);
    assert(umi_product_release_v2_init(
               &release, "studio-0.23.0", UMI_RELEASE_STABLE, &package,
               &channel) == UMI_STATUS_OK);
    assert(umi_installer_plan_v2_init(
               &installer, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "Umicom Studio", "bin/studio.exe", UMI_INSTALLER_WINDOWS_V2,
               UMI_INSTALL_SCOPE_USER_V2) == UMI_STATUS_OK);
    assert(umi_product_release_v2_add_installer(&release, &installer) ==
           UMI_STATUS_OK);
    for (index = 0U; index < sizeof(checks) / sizeof(checks[0]); ++index) {
        assert(umi_release_evidence_v2_set(
                   &release.evidence, checks[index], UMI_EVIDENCE_PASS) ==
               UMI_STATUS_OK);
    }
    assert(umi_product_release_v2_ready(&release));
    assert(umi_product_release_v2_blocker_count(&release) == 0U);
    return 0;
}
