/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_branch_policy.c
 *
 * PURPOSE:
 *   Validate enforce protected-branch and destructive-operation policy independently of frontend toolkits.
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
#include "umicom/vcs/advanced/branch_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedBranchPolicy p;
    umi_vcs_advanced_branch_policy_init(&p);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_branch_policy_add_protected(&p, "main") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_branch_policy_check_delete(&p, "main", 0, 1, 0) != UMI_STATUS_PERMISSION_DENIED) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_branch_policy_check_delete(&p, "feature", 0, 1, 0) != UMI_STATUS_OK) return 3;
    return 0;
}
