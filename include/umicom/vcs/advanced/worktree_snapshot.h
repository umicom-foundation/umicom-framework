/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/worktree_snapshot.h
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
#ifndef UMICOM_VCS_ADVANCED_WORKTREE_SNAPSHOT_H
#define UMICOM_VCS_ADVANCED_WORKTREE_SNAPSHOT_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced worktree snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedWorktreeSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    char branch[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char head_oid[UMI_VCS_ADVANCED_OID_CAPACITY];
    size_t staged_count;
    size_t modified_count;
    size_t untracked_count;
    size_t conflict_count;
    uint64_t fingerprint;
    int detached_head;
} UmiVcsAdvancedWorktreeSnapshot;

/**
 * Initialise vcs advanced worktree snapshot from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_worktree_snapshot_init(UmiVcsAdvancedWorktreeSnapshot *value);
/**
 * Check that vcs advanced worktree snapshot satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_worktree_snapshot_validate(const UmiVcsAdvancedWorktreeSnapshot *value);
/**
 * Provide the vcs advanced worktree snapshot clean operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_worktree_snapshot_clean(const UmiVcsAdvancedWorktreeSnapshot *value);

#ifdef __cplusplus
}
#endif

#endif
