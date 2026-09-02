/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/operation_guard.c
 *
 * PURPOSE:
 *   Implement preconditions that protect mutating source-control operations.
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
#include "umicom/vcs/advanced/operation_guard.h"

#include <string.h>

/*
 * Initialise vcs advanced operation guard from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_operation_guard_init(UmiVcsAdvancedOperationGuard *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->require_no_conflicts = 1;
    value->safety = UMI_VCS_SAFETY_REVIEW;
}

/*
 * Check that vcs advanced operation guard satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_operation_guard_validate(const UmiVcsAdvancedOperationGuard *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->safety > UMI_VCS_SAFETY_DESTRUCTIVE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced operation guard allows operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_operation_guard_allows(const UmiVcsAdvancedOperationGuard *guard,
                                               int worktree_clean,
                                               int conflicts,
                                               int has_upstream,
                                               int unpushed_commits,
                                               int detached_head)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_operation_guard_validate(guard) != UMI_STATUS_OK) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_clean_worktree && !worktree_clean) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_no_conflicts && conflicts) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_upstream && !has_upstream) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (guard->require_no_unpushed_commits && unpushed_commits) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!guard->allow_detached_head && detached_head) return 0;
    return 1;
}
