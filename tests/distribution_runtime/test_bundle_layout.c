/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_bundle_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for portable application bundle directory layout validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/bundle_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrBundleLayout value; umi_dr_bundle_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"standard")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.bin_dir,sizeof(value.bin_dir),"bin")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.lib_dir,sizeof(value.lib_dir),"lib")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.share_dir,sizeof(value.share_dir),"share")==UMI_STATUS_OK); CHECK(umi_dr_bundle_layout_valid(&value)); CHECK(umi_dr_bundle_layout_fingerprint(&value) != 0U);
    return 0;
}
