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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/operation_guard.h"

#include <string.h>

void umi_vcs_advanced_operation_guard_init(UmiVcsAdvancedOperationGuard *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->require_no_conflicts = 1;
    value->safety = UMI_VCS_SAFETY_REVIEW;
}

UmiStatus umi_vcs_advanced_operation_guard_validate(const UmiVcsAdvancedOperationGuard *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (value->safety > UMI_VCS_SAFETY_DESTRUCTIVE)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_operation_guard_allows(const UmiVcsAdvancedOperationGuard *guard,
                                               int worktree_clean,
                                               int conflicts,
                                               int has_upstream,
                                               int unpushed_commits,
                                               int detached_head)
{
    if (umi_vcs_advanced_operation_guard_validate(guard) != UMI_STATUS_OK) return 0;
    if (guard->require_clean_worktree && !worktree_clean) return 0;
    if (guard->require_no_conflicts && conflicts) return 0;
    if (guard->require_upstream && !has_upstream) return 0;
    if (guard->require_no_unpushed_commits && unpushed_commits) return 0;
    if (!guard->allow_detached_head && detached_head) return 0;
    return 1;
}
