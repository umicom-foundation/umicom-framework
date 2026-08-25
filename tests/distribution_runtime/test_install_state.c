/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_install_state.c
 *
 * PURPOSE:
 *   Focused regression coverage for installed application version, channel and health state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/install_state.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrInstallState value; umi_dr_install_state_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"inst")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.application_id,sizeof(value.application_id),"app")==UMI_STATUS_OK); value.channel=UMI_DR_CHANNEL_STABLE; value.scope=UMI_DR_SCOPE_USER; CHECK(umi_dr_install_state_valid(&value)); CHECK(umi_dr_install_state_fingerprint(&value) != 0U);
    return 0;
}
