/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_source_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for source distribution metadata, licence and reproducibility flags.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/source_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrSourcePackage value; umi_dr_source_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"src")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.revision,sizeof(value.revision),"abc")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.licence,sizeof(value.licence),"MIT")==UMI_STATUS_OK); CHECK(umi_dr_source_package_valid(&value)); CHECK(umi_dr_source_package_fingerprint(&value) != 0U);
    return 0;
}
