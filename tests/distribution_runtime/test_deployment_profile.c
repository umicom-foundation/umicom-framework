/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/distribution_runtime/test_deployment_profile.c
 *
 * PURPOSE:
 *   Focused regression coverage for deployment target, scope, rollout and update-channel profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/distribution/runtime/deployment_profile.h"


#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDrDeploymentProfile value; umi_dr_deployment_profile_init(&value); CHECK(umi_dr_copy_text(value.id,sizeof(value.id),"desktop")==UMI_STATUS_OK); CHECK(umi_dr_copy_text(value.target,sizeof(value.target),"local")==UMI_STATUS_OK); value.scope=UMI_DR_SCOPE_USER; value.channel=UMI_DR_CHANNEL_STABLE; value.rollout_percent=100U; CHECK(umi_dr_deployment_profile_valid(&value)); CHECK(umi_dr_deployment_profile_fingerprint(&value) != 0U);
    return 0;
}
