/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_package_format.c
 *
 * PURPOSE:
 *   Focused regression coverage for package format descriptors and platform suitability checks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_format.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    CHECK(umi_dr_package_format_supported(UMI_DR_PACKAGE_MSI,UMI_DR_PLATFORM_WINDOWS)); CHECK(!umi_dr_package_format_supported(UMI_DR_PACKAGE_DEB,UMI_DR_PLATFORM_WINDOWS)); CHECK(umi_dr_package_format_installer(UMI_DR_PACKAGE_RPM));
    return 0;
}
