/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_worktree_snapshot.c
 *
 * PURPOSE:
 *   Validate capture stable worktree/index state for review, guards and recovery.
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
#include "umicom/vcs/advanced/worktree_snapshot.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedWorktreeSnapshot value;
    umi_vcs_advanced_worktree_snapshot_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_worktree_snapshot_validate(&value) == UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_copy_text(value.head_oid, sizeof(value.head_oid), "abc") != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_worktree_snapshot_clean(&value)) return 3;
    value.modified_count = 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_worktree_snapshot_clean(&value)) return 4;
    return 0;
}
