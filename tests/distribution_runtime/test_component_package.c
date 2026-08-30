/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_component_package.c
 *
 * PURPOSE:
 *   Focused regression coverage for optional Framework component package and dependency metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/component_package.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrComponentPackage value; umi_dr_component_package_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"component")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.component_id,sizeof(value.component_id),"ai")==UMI_STATUS_OK); CHECK(umi_dr_component_package_valid(&value)); CHECK(umi_dr_component_package_fingerprint(&value) != 0U);
    return 0;
}
