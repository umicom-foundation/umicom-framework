/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_runtime_dependency.c
 *
 * PURPOSE:
 *   Focused regression coverage for native runtime-library dependency and availability policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/runtime_dependency.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrRuntimeDependency value; umi_dr_runtime_dependency_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"libc")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.soname,sizeof(value.soname),"libc.so.6")==UMI_STATUS_OK); value.system_provided=true; CHECK(umi_dr_runtime_dependency_valid(&value)); CHECK(umi_dr_runtime_dependency_fingerprint(&value) != 0U);
    return 0;
}
