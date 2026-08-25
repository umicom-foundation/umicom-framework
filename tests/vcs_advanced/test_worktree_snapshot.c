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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/worktree_snapshot.h"

int main(void)
{
    UmiVcsAdvancedWorktreeSnapshot value;
    umi_vcs_advanced_worktree_snapshot_init(&value);
    if (umi_vcs_advanced_worktree_snapshot_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_copy_text(value.head_oid, sizeof(value.head_oid), "abc") != UMI_STATUS_OK) return 2;
    if (!umi_vcs_advanced_worktree_snapshot_clean(&value)) return 3;
    value.modified_count = 1U;
    if (umi_vcs_advanced_worktree_snapshot_clean(&value)) return 4;
    return 0;
}
