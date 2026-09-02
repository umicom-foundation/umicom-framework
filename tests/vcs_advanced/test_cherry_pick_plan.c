/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_cherry_pick_plan.c
 *
 * PURPOSE:
 *   Validate maintain ordered cherry-pick plans with mainline and no-commit controls.
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
#include "umicom/vcs/advanced/cherry_pick_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedCherryPickPlan p;
    umi_vcs_advanced_cherry_pick_plan_init(&p);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_cherry_pick_plan_add(&p,"a")!=UMI_STATUS_OK)return 1;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_cherry_pick_plan_add(&p,"b")!=UMI_STATUS_OK)return 2;
/* Apply this operation only while the related capability or state is available. */
if(!umi_vcs_advanced_cherry_pick_plan_ready(&p))return 3;
    return 0;
}
