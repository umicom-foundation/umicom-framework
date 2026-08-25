/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_launch_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for named launch profile with environment, frontend and safe-mode controls.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/launch_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrLaunchProfile value; umi_dr_launch_profile_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"normal")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.launcher_id,sizeof(value.launcher_id),"default")==UMI_STATUS_OK); CHECK(umi_dr_launch_profile_valid(&value)); CHECK(umi_dr_launch_profile_fingerprint(&value) != 0U);
    return 0;
}
