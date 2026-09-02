/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workspace_coordinator.h
 *
 * PURPOSE:
 *   Coordinate the selection, filtering, commit-composition and command state
 *   required by a professional source-control workspace.  Repository records
 *   remain owned by UmiVcsWorkspace; this coordinator adds presentation state
 *   without copying or replacing the canonical VCS collections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VCS_WORKSPACE_COORDINATOR_H
#define UMICOM_VCS_WORKSPACE_COORDINATOR_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/vcs/workspace.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_VCS_COMMIT_MESSAGE_CAPACITY 4096U

/**
 * List the named vcs change filter values accepted by this public contract.
 */
typedef enum UmiVcsChangeFilter {
    UMI_VCS_CHANGE_FILTER_ALL = 0,
    UMI_VCS_CHANGE_FILTER_STAGED = 1,
    UMI_VCS_CHANGE_FILTER_UNSTAGED = 2,
    UMI_VCS_CHANGE_FILTER_CONFLICTS = 3
} UmiVcsChangeFilter;

/**
 * Represent the vcs workspace coordinator data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkspaceCoordinator UmiVcsWorkspaceCoordinator;

/**
 * Represent the vcs workspace coordinator snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsWorkspaceCoordinatorSnapshot {
    uint32_t struct_size;
    uint64_t revision;
    UmiVcsWorkspaceSnapshot repository;
    UmiVcsChangeFilter change_filter;
    char change_filter_label[UMI_VCS_NAME_CAPACITY];
    char selected_change_path[UMI_VCS_PATH_CAPACITY];
    char selected_commit_id[UMI_VCS_ID_CAPACITY];
    char selected_branch_name[UMI_VCS_NAME_CAPACITY];
    char selected_remote_name[UMI_VCS_NAME_CAPACITY];
    char commit_message[UMI_VCS_COMMIT_MESSAGE_CAPACITY];
    char diff_path[UMI_VCS_PATH_CAPACITY];
    size_t visible_change_count;
    size_t staged_change_count;
    size_t unstaged_change_count;
    size_t conflict_count;
    size_t history_count;
    size_t branch_count;
    size_t remote_count;
    size_t tag_count;
    size_t diff_line_count;
    size_t diff_addition_count;
    size_t diff_deletion_count;
    size_t diff_hunk_count;
    size_t operation_count;
    int show_untracked;
    int diff_staged;
    int diff_binary;
    int can_refresh;
    int can_stage_selected;
    int can_unstage_selected;
    int can_discard_selected;
    int can_commit;
    int can_fetch;
    int can_pull;
    int can_push;
    int can_create_branch;
    int can_checkout_selected_branch;
    int can_delete_selected_branch;
    int can_open_selected_diff;
} UmiVcsWorkspaceCoordinatorSnapshot;

/**
 * Initialise vcs workspace coordinator from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_vcs_workspace_coordinator_create(
    UmiVcsWorkspace *workspace,
    UmiVcsWorkspaceCoordinator **out_coordinator
);
/**
 * Release or reset state held by vcs workspace coordinator so the same storage can be
 * reused safely.
 */
void umi_vcs_workspace_coordinator_destroy(
    UmiVcsWorkspaceCoordinator *coordinator
);

/**
 * Provide the vcs workspace coordinator refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_refresh(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t history_limit
);
/**
 * Provide the vcs workspace coordinator synchronise operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_synchronise(
    UmiVcsWorkspaceCoordinator *coordinator
);
/**
 * Provide the vcs workspace coordinator snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_snapshot(
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiVcsWorkspaceCoordinatorSnapshot *out_snapshot
);

/**
 * Provide the vcs workspace coordinator set change filter operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_set_change_filter(
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiVcsChangeFilter filter
);
/**
 * Provide the vcs workspace coordinator set show untracked operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_set_show_untracked(
    UmiVcsWorkspaceCoordinator *coordinator,
    int enabled
);
/**
 * Provide the vcs workspace coordinator select change operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_change(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *path
);
/**
 * Provide the vcs workspace coordinator select commit operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_commit(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *commit_id
);
/**
 * Provide the vcs workspace coordinator select branch operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_branch(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *branch_name
);
/**
 * Provide the vcs workspace coordinator select remote operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_remote(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *remote_name
);
/**
 * Provide the vcs workspace coordinator set commit message operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_set_commit_message(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *message
);
/**
 * Provide the vcs workspace coordinator clear selection operation used by this module and
 * its client applications.
 */
void umi_vcs_workspace_coordinator_clear_selection(
    UmiVcsWorkspaceCoordinator *coordinator
);

/**
 * Find vcs workspace coordinator stage while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_stage_selected(
    UmiVcsWorkspaceCoordinator *coordinator
);
/**
 * Find vcs workspace coordinator unstage while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_unstage_selected(
    UmiVcsWorkspaceCoordinator *coordinator
);
/**
 * Find vcs workspace coordinator discard while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_discard_selected(
    UmiVcsWorkspaceCoordinator *coordinator
);
/**
 * Provide the vcs workspace coordinator commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_commit(
    UmiVcsWorkspaceCoordinator *coordinator,
    char *out_commit_id,
    size_t capacity
);
/**
 * Provide the vcs workspace coordinator open selected diff operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_open_selected_diff(
    UmiVcsWorkspaceCoordinator *coordinator,
    int staged
);

/**
 * Find vcs workspace coordinator change while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_change_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t visible_index,
    UmiVcsChange *out_change
);
/**
 * Find vcs workspace coordinator conflict while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_conflict_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsConflict *out_conflict
);
/**
 * Find vcs workspace coordinator commit while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_commit_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsCommit *out_commit
);
/**
 * Find vcs workspace coordinator branch while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_branch_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsBranch *out_branch
);
/**
 * Find vcs workspace coordinator remote while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_remote_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsRemote *out_remote
);
/**
 * Find vcs workspace coordinator tag while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_tag_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsTag *out_tag
);
/**
 * Find vcs workspace coordinator diff line while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_diff_line_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsDiffLine *out_line
);
/**
 * Find vcs workspace coordinator operation while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_operation_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsOperation *out_operation
);

/**
 * Provide the vcs change filter text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_change_filter_text(UmiVcsChangeFilter filter);

#ifdef __cplusplus
}
#endif

#endif
