/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/operation_guard.h
 *
 * PURPOSE:
 *   Describe preconditions that protect mutating source-control operations.
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
#ifndef UMICOM_VCS_ADVANCED_OPERATION_GUARD_H
#define UMICOM_VCS_ADVANCED_OPERATION_GUARD_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced operation guard data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedOperationGuard {
    uint32_t struct_size;
    uint32_t api_version;
    int require_clean_worktree;
    int require_no_conflicts;
    int require_upstream;
    int require_no_unpushed_commits;
    int allow_detached_head;
    UmiVcsSafetyLevel safety;
} UmiVcsAdvancedOperationGuard;

/**
 * Initialise vcs advanced operation guard from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_operation_guard_init(UmiVcsAdvancedOperationGuard *value);
/**
 * Check that vcs advanced operation guard satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_operation_guard_validate(const UmiVcsAdvancedOperationGuard *value);
/**
 * Provide the vcs advanced operation guard allows operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_operation_guard_allows(const UmiVcsAdvancedOperationGuard *guard,
                                               int worktree_clean,
                                               int conflicts,
                                               int has_upstream,
                                               int unpushed_commits,
                                               int detached_head);

#ifdef __cplusplus
}
#endif

#endif
