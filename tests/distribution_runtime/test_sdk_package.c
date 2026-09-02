/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_sdk_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for developer SDK package metadata and ABI compatibility range.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/sdk_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrSdkPackage value; umi_dr_sdk_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"sdk")==UMI_STATUS_OK); value.headers=true; value.maximum_abi=(UmiDrVersion){1,0,0}; CHECK(umi_dr_sdk_package_valid(&value)); CHECK(umi_dr_sdk_package_fingerprint(&value) != 0U);
    return 0;
}
