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

typedef enum UmiVcsChangeFilter {
    UMI_VCS_CHANGE_FILTER_ALL = 0,
    UMI_VCS_CHANGE_FILTER_STAGED = 1,
    UMI_VCS_CHANGE_FILTER_UNSTAGED = 2,
    UMI_VCS_CHANGE_FILTER_CONFLICTS = 3
} UmiVcsChangeFilter;

typedef struct UmiVcsWorkspaceCoordinator UmiVcsWorkspaceCoordinator;

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

UmiStatus umi_vcs_workspace_coordinator_create(
    UmiVcsWorkspace *workspace,
    UmiVcsWorkspaceCoordinator **out_coordinator
);
void umi_vcs_workspace_coordinator_destroy(
    UmiVcsWorkspaceCoordinator *coordinator
);

UmiStatus umi_vcs_workspace_coordinator_refresh(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t history_limit
);
UmiStatus umi_vcs_workspace_coordinator_synchronise(
    UmiVcsWorkspaceCoordinator *coordinator
);
UmiStatus umi_vcs_workspace_coordinator_snapshot(
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiVcsWorkspaceCoordinatorSnapshot *out_snapshot
);

UmiStatus umi_vcs_workspace_coordinator_set_change_filter(
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiVcsChangeFilter filter
);
UmiStatus umi_vcs_workspace_coordinator_set_show_untracked(
    UmiVcsWorkspaceCoordinator *coordinator,
    int enabled
);
UmiStatus umi_vcs_workspace_coordinator_select_change(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *path
);
UmiStatus umi_vcs_workspace_coordinator_select_commit(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *commit_id
);
UmiStatus umi_vcs_workspace_coordinator_select_branch(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *branch_name
);
UmiStatus umi_vcs_workspace_coordinator_select_remote(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *remote_name
);
UmiStatus umi_vcs_workspace_coordinator_set_commit_message(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *message
);
void umi_vcs_workspace_coordinator_clear_selection(
    UmiVcsWorkspaceCoordinator *coordinator
);

UmiStatus umi_vcs_workspace_coordinator_stage_selected(
    UmiVcsWorkspaceCoordinator *coordinator
);
UmiStatus umi_vcs_workspace_coordinator_unstage_selected(
    UmiVcsWorkspaceCoordinator *coordinator
);
UmiStatus umi_vcs_workspace_coordinator_discard_selected(
    UmiVcsWorkspaceCoordinator *coordinator
);
UmiStatus umi_vcs_workspace_coordinator_commit(
    UmiVcsWorkspaceCoordinator *coordinator,
    char *out_commit_id,
    size_t capacity
);
UmiStatus umi_vcs_workspace_coordinator_open_selected_diff(
    UmiVcsWorkspaceCoordinator *coordinator,
    int staged
);

UmiStatus umi_vcs_workspace_coordinator_change_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t visible_index,
    UmiVcsChange *out_change
);
UmiStatus umi_vcs_workspace_coordinator_conflict_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsConflict *out_conflict
);
UmiStatus umi_vcs_workspace_coordinator_commit_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsCommit *out_commit
);
UmiStatus umi_vcs_workspace_coordinator_branch_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsBranch *out_branch
);
UmiStatus umi_vcs_workspace_coordinator_remote_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsRemote *out_remote
);
UmiStatus umi_vcs_workspace_coordinator_tag_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsTag *out_tag
);
UmiStatus umi_vcs_workspace_coordinator_diff_line_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsDiffLine *out_line
);
UmiStatus umi_vcs_workspace_coordinator_operation_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsOperation *out_operation
);

const char *umi_vcs_change_filter_text(UmiVcsChangeFilter filter);

#ifdef __cplusplus
}
#endif

#endif
