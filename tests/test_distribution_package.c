/*-----------------------------------------------------------------------------
 * Umicom Framework Tests
 * File: tests/test_distribution_package.c
 *
 * PURPOSE:
 *   Verify Distribution API v2 package initialisation, validation and version
 *   ordering.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "distribution_fixture.h"

int main(void)
{
    UmiDistributionPackage package = distribution_test_package(
        "studio-0.17.0", "org.umicom.studio", (UmiVersion){0U, 17U, 0U},
        UMI_RELEASE_STABLE);
    char reason[256];
    assert(umi_distribution_package_validate(
        &package, reason, sizeof(reason)) == UMI_STATUS_OK);
    assert(strcmp(reason, "valid") == 0);
    assert(umi_distribution_version_compare(
        (UmiVersion){0U, 17U, 0U}, (UmiVersion){0U, 16U, 9U}) > 0);
    assert(strcmp(umi_distribution_package_kind_text(package.kind),
                  "application") == 0);
    package.required_framework_abi = 0U;
    assert(umi_distribution_package_validate(
        &package, reason, sizeof(reason)) == UMI_STATUS_INVALID_STATE);
    return 0;
}
