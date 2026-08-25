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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_WORKTREE_SNAPSHOT_H
#define UMICOM_VCS_ADVANCED_WORKTREE_SNAPSHOT_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

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

void umi_vcs_advanced_worktree_snapshot_init(UmiVcsAdvancedWorktreeSnapshot *value);
UmiStatus umi_vcs_advanced_worktree_snapshot_validate(const UmiVcsAdvancedWorktreeSnapshot *value);
int umi_vcs_advanced_worktree_snapshot_clean(const UmiVcsAdvancedWorktreeSnapshot *value);

#ifdef __cplusplus
}
#endif

#endif
