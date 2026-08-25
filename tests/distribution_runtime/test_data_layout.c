/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_data_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for read-only packaged data and writable application-data separation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/data_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrDataLayout value; umi_dr_data_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"data")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.read_only_dir,sizeof(value.read_only_dir),"share")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.writable_dir,sizeof(value.writable_dir),"state")==UMI_STATUS_OK); CHECK(umi_dr_data_layout_valid(&value)); CHECK(umi_dr_data_layout_fingerprint(&value) != 0U);
    return 0;
}
