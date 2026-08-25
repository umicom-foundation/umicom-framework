/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_executable_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for executable placement and launch-entry validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/executable_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrExecutableLayout value; umi_dr_executable_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"exec")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.entrypoint,sizeof(value.entrypoint),"bin/app")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.bin_dir,sizeof(value.bin_dir),"bin")==UMI_STATUS_OK); CHECK(umi_dr_executable_layout_valid(&value)); CHECK(umi_dr_executable_layout_fingerprint(&value) != 0U);
    return 0;
}
