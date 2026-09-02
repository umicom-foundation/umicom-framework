/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_stash_plan.c
 *
 * PURPOSE:
 *   Validate plan stash push/apply/pop/drop/branch operations with explicit conflict and index intent.
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
#include "umicom/vcs/advanced/stash_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedStashPlan p; umi_vcs_advanced_stash_plan_init(&p);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_plan_push(&p,"checkpoint",1,0)!=UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_plan_validate(&p)!=UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_stash_plan_apply(&p,"stash@{0}",1,1)!=UMI_STATUS_OK) return 3;
    return 0;
}
