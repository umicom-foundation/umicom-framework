/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_product_release.c
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
#include "value_archive/construction_cases.h"
#include "umicom/delivery/delivery.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted update channel value. */
static int UmiUpdateChannelConstructionCases(void)
{
    UmiUpdateChannel *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_update_channel_init(value, "stable", "https://updates.umicom.org/studio/stable.json", UMI_RELEASE_STABLE, 100U) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->channel_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_update_channel_init(value, oversized, "https://updates.umicom.org/studio/stable.json", UMI_RELEASE_STABLE, 100U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_update_channel_init(value, value->channel_id, "https://updates.umicom.org/studio/stable.json", UMI_RELEASE_STABLE, 100U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->channel_id, "stable") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->feed_url) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_update_channel_init(value, "stable", oversized, UMI_RELEASE_STABLE, 100U) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_update_channel_init(value, "stable", value->feed_url, UMI_RELEASE_STABLE, 100U) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->feed_url, "https://updates.umicom.org/studio/stable.json") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiUpdateChannelConstructionCases() != 0) return 1;

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
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < sizeof(checks) / sizeof(checks[0]); ++index) {
        assert(umi_release_evidence_set(
                   &release.evidence, checks[index], UMI_EVIDENCE_PASS) ==
               UMI_STATUS_OK);
    }
    assert(umi_product_release_ready(&release));
    assert(umi_product_release_blocker_count(&release) == 0U);
    return 0;
}
