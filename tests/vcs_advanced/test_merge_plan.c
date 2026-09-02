/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_merge_plan.c
 *
 * PURPOSE:
 *   Validate plan merge strategy, fast-forward policy and safety gates before repository mutation.
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
#include "umicom/vcs/advanced/merge_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedMergePlan p;
    umi_vcs_advanced_merge_plan_init(&p);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_merge_plan_set(&p,"feature","main")!=UMI_STATUS_OK)return 1;
/* Apply this operation only while the related capability or state is available. */
if(!umi_vcs_advanced_merge_plan_ready(&p,1,0))return 2;
    return 0;
}
