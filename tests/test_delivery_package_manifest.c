/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_delivery_package_manifest.c
 *
 * PURPOSE:
 *   Verify product package manifests and entrypoint validation.
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

/* A rejected replacement must preserve the previously accepted package manifest value. */
static int UmiPackageManifestConstructionCases(void)
{
    UmiPackageManifest *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_package_manifest_init(value, "umicom-studio", "0.23.0", "bin/umicom-studio-console", UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->product_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_package_manifest_init(value, oversized, "0.23.0", "bin/umicom-studio-console", UMI_PACKAGE_ZIP) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_package_manifest_init(value, value->product_id, "0.23.0", "bin/umicom-studio-console", UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->product_id, "umicom-studio") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->version) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_package_manifest_init(value, "umicom-studio", oversized, "bin/umicom-studio-console", UMI_PACKAGE_ZIP) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_package_manifest_init(value, "umicom-studio", value->version, "bin/umicom-studio-console", UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->version, "0.23.0") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->entrypoint) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_package_manifest_init(value, "umicom-studio", "0.23.0", oversized, UMI_PACKAGE_ZIP) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_package_manifest_init(value, "umicom-studio", "0.23.0", value->entrypoint, UMI_PACKAGE_ZIP) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->entrypoint, "bin/umicom-studio-console") == 0);
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
    if (UmiPackageManifestConstructionCases() != 0) return 1;

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
