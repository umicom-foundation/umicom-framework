/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_installer_plan.c
 *
 * PURPOSE:
 *   Focused regression coverage for installer operations with privilege and reboot evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/installer_plan.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiDrInstallerPlan p; umi_dr_installer_plan_init(&p); p.required_steps=2U; CHECK(umi_dr_installer_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(!umi_dr_installer_plan_ready(&p)); CHECK(umi_dr_installer_plan_complete_step(&p)==UMI_STATUS_OK); CHECK(umi_dr_installer_plan_ready(&p));
    return 0;
}
