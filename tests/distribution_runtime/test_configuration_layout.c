/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_configuration_layout.c
 *
 * PURPOSE:
 *   Focused regression coverage for system, user and portable configuration-root policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/configuration_layout.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrConfigurationLayout value; umi_dr_configuration_layout_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"config")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.user_root,sizeof(value.user_root),"config")==UMI_STATUS_OK); value.portable=true; CHECK(umi_dr_configuration_layout_valid(&value)); CHECK(umi_dr_configuration_layout_fingerprint(&value) != 0U);
    return 0;
}
