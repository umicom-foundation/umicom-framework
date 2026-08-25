/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_environment_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded environment-variable overlay for packaged applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/environment_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrEnvironmentProfile p; umi_dr_environment_profile_init(&p); CHECK(umi_dr_environment_profile_set(&p,"MODE","release")==UMI_STATUS_OK); CHECK(umi_dr_environment_profile_get(&p,"MODE")!=NULL); CHECK(umi_dr_environment_profile_set(&p,"MODE","debug")==UMI_STATUS_OK); CHECK(p.count==1U);
    return 0;
}
