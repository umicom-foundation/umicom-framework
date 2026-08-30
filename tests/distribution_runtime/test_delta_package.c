/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_delta_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for delta package base/target version and savings validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/delta_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrDeltaPackage value; umi_dr_delta_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"delta")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.digest,sizeof(value.digest),"d")==UMI_STATUS_OK); value.base_version=(UmiDrVersion){1,0,0}; value.target_version=(UmiDrVersion){1,1,0}; value.full_size=100U; value.delta_size=20U; CHECK(umi_dr_delta_package_valid(&value)); CHECK(umi_dr_delta_package_fingerprint(&value) != 0U);
    return 0;
}
