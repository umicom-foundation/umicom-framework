/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_reset_plan.c
 *
 * PURPOSE:
 *   Validate model soft/mixed/hard reset intent with destructive-operation classification.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/reset_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedResetPlan p;
    umi_vcs_advanced_reset_plan_init(&p);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_reset_plan_set(&p,UMI_VCS_RESET_HARD,"HEAD~1")!=UMI_STATUS_OK)return 1;
/* Apply this branch only when its contract condition is satisfied. */
if(!umi_vcs_advanced_reset_plan_destructive(&p))return 2;
    return 0;
}
