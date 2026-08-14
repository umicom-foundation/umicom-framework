/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_product_release.c
 *
 * PURPOSE:
 *   Verify complete package, installer, evidence and update release readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/delivery/delivery.h"

int main(void)
{
    UmiPackageManifest package;
    UmiUpdateChannel channel;
    UmiInstallerPlan installer;
    UmiProductRelease release;
    const char *checks[] = {
        "tests", "checksums", "signatures", "licences", "sbom", "provenance"
    };
    size_t index;
    assert(umi_package_manifest_init(
               &package, "umicom-studio", "0.23.0", "bin/studio.exe",
               UMI_PACKAGE_WINDOWS_SETUP) == UMI_STATUS_OK);
    assert(umi_package_manifest_add(
               &package, "build/studio.exe", "bin/studio.exe",
               UMI_PACKAGE_FILE_EXECUTABLE, 1) == UMI_STATUS_OK);
    assert(umi_update_channel_init(
               &channel, "stable", "https://updates.umicom.org/studio/stable.json",
               UMI_RELEASE_STABLE, 100U) == UMI_STATUS_OK);
    assert(umi_product_release_init(
               &release, "studio-0.23.0", UMI_RELEASE_STABLE, &package,
               &channel) == UMI_STATUS_OK);
    assert(umi_installer_plan_init(
               &installer, "Umicom Studio", "Umicom Foundation", "0.23.0",
               "Umicom Studio", "bin/studio.exe", UMI_INSTALLER_WINDOWS,
               UMI_INSTALL_SCOPE_USER) == UMI_STATUS_OK);
    assert(umi_product_release_add_installer(&release, &installer) ==
           UMI_STATUS_OK);
    for (index = 0U; index < sizeof(checks) / sizeof(checks[0]); ++index) {
        assert(umi_release_evidence_set(
                   &release.evidence, checks[index], UMI_EVIDENCE_PASS) ==
               UMI_STATUS_OK);
    }
    assert(umi_product_release_ready(&release));
    assert(umi_product_release_blocker_count(&release) == 0U);
    return 0;
}
