/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_debug_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for diagnostic/debug companion package metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/debug_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrDebugPackage value; umi_dr_debug_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"dbg")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); value.symbols=true; CHECK(umi_dr_debug_package_valid(&value)); CHECK(umi_dr_debug_package_fingerprint(&value) != 0U);
    return 0;
}
