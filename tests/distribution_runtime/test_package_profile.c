/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_package_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for named package profile selecting format, scope, compression and symbols policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/package_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrPackageProfile value; umi_dr_package_profile_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"release")==UMI_STATUS_OK); value.format=UMI_DR_PACKAGE_ZIP; value.scope=UMI_DR_SCOPE_PORTABLE; CHECK(umi_dr_package_profile_valid(&value)); CHECK(umi_dr_package_profile_fingerprint(&value) != 0U);
    return 0;
}
