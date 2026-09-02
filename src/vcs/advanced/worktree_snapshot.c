/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/worktree_snapshot.c
 *
 * PURPOSE:
 *   Capture stable worktree/index state for review, guards and recovery.
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

#include <string.h>

/*
 * Initialise vcs advanced worktree snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_worktree_snapshot_init(UmiVcsAdvancedWorktreeSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;

}

/*
 * Check that vcs advanced worktree snapshot satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_worktree_snapshot_validate(const UmiVcsAdvancedWorktreeSnapshot *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->head_oid))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced worktree snapshot clean operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_worktree_snapshot_clean(const UmiVcsAdvancedWorktreeSnapshot *value)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_worktree_snapshot_validate(value) != UMI_STATUS_OK) return 0;
    return value->staged_count == 0U &&
           value->modified_count == 0U &&
           value->untracked_count == 0U &&
           value->conflict_count == 0U;
}
