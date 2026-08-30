/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs_ui/workspace_views.c
 *
 * PURPOSE:
 *   Project the professional source-control workspace coordinator into
 *   toolkit-neutral panes shared by Studio, future Umicom applications,
 *   GTK4, Qt, Wt and headless automation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs_ui/vcs_ui.h"

#include <stdio.h>
#include <string.h>

#include "view_internal.h"

static UmiStatus coordinator_snapshot(
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiVcsWorkspaceCoordinatorSnapshot *out_snapshot)
{
    if (coordinator == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_vcs_workspace_coordinator_snapshot(coordinator, out_snapshot);
}

static size_t visible_rows(size_t count)
{
    return count < UMI_VCS_UI_VISIBLE_ROWS
        ? count : UMI_VCS_UI_VISIBLE_ROWS;
}

static UmiStatus set_repository_properties(
    UmiUiViewModel *view,
    const UmiVcsWorkspaceCoordinatorSnapshot *snapshot)
{
    UmiStatus status;

    status = umi_vcs_ui_set_boolean(view, "vcs.available",
                                    snapshot->repository.available);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(view, "vcs.root",
                                       snapshot->repository.root);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(view, "vcs.provider",
                                       snapshot->repository.provider_id);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(view, "vcs.branch",
                                       snapshot->repository.branch);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(view, "vcs.upstream",
                                       snapshot->repository.upstream);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(view, "vcs.ahead",
                                        snapshot->repository.ahead);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(view, "vcs.behind",
                                        snapshot->repository.behind);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(view, "vcs.revision",
                                        (int64_t)snapshot->revision);
    }
    return status;
}

UmiStatus umi_vcs_ui_workspace_changes_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-changes", "Source Control",
        "Working tree, staged changes, repository state and safe actions.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(*out_view, "vcs.change-filter",
                                       snapshot.change_filter_label);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(*out_view, "vcs.selected-change",
                                       snapshot.selected_change_path);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.change-count",
            (int64_t)snapshot.repository.changes);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.visible-change-count",
            (int64_t)snapshot.visible_change_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.staged-count",
            (int64_t)snapshot.staged_change_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.unstaged-count",
            (int64_t)snapshot.unstaged_change_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.conflict-count",
            (int64_t)snapshot.conflict_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_boolean(*out_view, "vcs.show-untracked",
                                        snapshot.show_untracked);
    }

    count = visible_rows(snapshot.visible_change_count);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count",
                                        (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiVcsChange change;
        char key[64];
        char text[(UMI_VCS_PATH_CAPACITY * 2U) + 160U];

        status = umi_vcs_workspace_coordinator_change_at(
            coordinator, index, &change);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s%s %s%s%s — index: %s; tree: %s",
                       strcmp(change.path, snapshot.selected_change_path) == 0
                           ? "● " : "○ ",
                       change.staged ? "[staged]" : "[change]",
                       change.path,
                       change.original_path[0] != '\0' ? " ← " : "",
                       change.original_path,
                       umi_vcs_change_state_text(change.index_state),
                       umi_vcs_change_state_text(change.worktree_state));
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }

    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.refresh", "Refresh",
        "Refresh working tree, history and references", snapshot.can_refresh);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 1U, "studio.action.vcs.filter", "Filter…",
        "Show all, staged, unstaged or conflicting changes", 1);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 2U, "studio.action.vcs.select-change", "Select Path…",
        "Select a repository-relative change", count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 3U, "studio.action.vcs.stage-selected", "Stage Selected",
        "Stage the selected working-tree path",
        snapshot.can_stage_selected);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 4U, "studio.action.vcs.unstage-selected",
        "Unstage Selected", "Remove the selected path from the index",
        snapshot.can_unstage_selected);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 5U, "studio.action.vcs.diff-selected", "Open Diff",
        "Open the selected path in the Diff Viewer",
        snapshot.can_open_selected_diff);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 6U, "studio.action.vcs.discard-selected", "Discard…",
        "Discard the selected unstaged change after confirmation",
        snapshot.can_discard_selected);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 7U, "studio.action.vcs.stage-all", "Stage All",
        "Stage every working-tree change",
        snapshot.unstaged_change_count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 8U, "studio.action.vcs.unstage-all", "Unstage All",
        "Remove every indexed change", snapshot.staged_change_count > 0U);
    return status;
}

UmiStatus umi_vcs_ui_workspace_commit_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-commit", "Commit",
        "Compose and validate a commit from the current staged changes.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(*out_view, "vcs.commit-message",
                                       snapshot.commit_message);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.staged-count",
            (int64_t)snapshot.staged_change_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.conflict-count",
            (int64_t)snapshot.conflict_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_boolean(*out_view, "vcs.commit-ready",
                                        snapshot.can_commit);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.set-commit-message",
        "Commit Message…", "Set the commit message", 1);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 1U, "studio.action.vcs.commit-composed", "Commit",
        "Create a commit using the composed message", snapshot.can_commit);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 2U, "studio.action.vcs.stage-all", "Stage All",
        "Stage every working-tree change",
        snapshot.unstaged_change_count > 0U);
    return status;
}

UmiStatus umi_vcs_ui_workspace_history_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-history", "Repository History",
        "Commit history with stable selection for inspection and comparison.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(*out_view, "vcs.selected-commit",
                                       snapshot.selected_commit_id);
    }
    count = visible_rows(snapshot.history_count);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count",
                                        (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiVcsCommit commit;
        char key[64];
        char text[UMI_VCS_MESSAGE_CAPACITY +
                  (UMI_VCS_NAME_CAPACITY * 2U) +
                  UMI_VCS_ID_CAPACITY + 64U];
        status = umi_vcs_workspace_coordinator_commit_at(
            coordinator, index, &commit);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s%.12s  %s — %s <%s>",
                       strcmp(commit.commit_id,
                              snapshot.selected_commit_id) == 0
                           ? "● " : "○ ",
                       commit.commit_id, commit.subject,
                       commit.author, commit.email);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.select-commit", "Select Commit…",
        "Select a commit by its stable identifier", count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 1U, "studio.action.vcs.refresh", "Refresh",
        "Refresh repository history", snapshot.can_refresh);
    return status;
}

UmiStatus umi_vcs_ui_workspace_branches_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-branches", "Branches",
        "Inspect, select, create, switch and safely delete branches.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(*out_view, "vcs.selected-branch",
                                       snapshot.selected_branch_name);
    }
    count = visible_rows(snapshot.branch_count);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count",
                                        (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiVcsBranch branch;
        char key[64];
        char text[(UMI_VCS_NAME_CAPACITY * 2U) + 128U];
        status = umi_vcs_workspace_coordinator_branch_at(
            coordinator, index, &branch);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s%s %s%s%s  ahead %d; behind %d",
                       strcmp(branch.name,
                              snapshot.selected_branch_name) == 0
                           ? "● " : "○ ",
                       branch.current ? "[current]" : "[branch]",
                       branch.name,
                       branch.upstream[0] != '\0' ? " → " : "",
                       branch.upstream, branch.ahead, branch.behind);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.select-branch", "Select Branch…",
        "Select a branch for subsequent actions", count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 1U, "studio.action.vcs.branch-create", "New Branch…",
        "Create and check out a validated branch",
        snapshot.can_create_branch);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 2U, "studio.action.vcs.branch-checkout", "Checkout…",
        "Switch to an existing branch", snapshot.branch_count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 3U, "studio.action.vcs.branch-delete", "Delete…",
        "Safely delete a merged branch", snapshot.branch_count > 1U);
    return status;
}

UmiStatus umi_vcs_ui_workspace_remotes_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-remotes", "Remotes",
        "Remote endpoints and capability-aware synchronization commands.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(*out_view, "vcs.selected-remote",
                                       snapshot.selected_remote_name);
    }
    count = visible_rows(snapshot.remote_count);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count",
                                        (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiVcsRemote remote;
        char key[64];
        char text[(UMI_VCS_URL_CAPACITY * 2U) +
                  UMI_VCS_NAME_CAPACITY + 64U];
        status = umi_vcs_workspace_coordinator_remote_at(
            coordinator, index, &remote);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s%s — fetch: %s; push: %s",
                       strcmp(remote.name,
                              snapshot.selected_remote_name) == 0
                           ? "● " : "○ ",
                       remote.name, remote.fetch_url, remote.push_url);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.select-remote", "Select Remote…",
        "Select a configured remote", count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 1U, "studio.action.vcs.fetch", "Fetch",
        "Fetch and prune remote references", snapshot.can_fetch);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 2U, "studio.action.vcs.pull", "Pull",
        "Fast-forward the active branch", snapshot.can_pull);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 3U, "studio.action.vcs.push", "Push",
        "Push the active branch", snapshot.can_push);
    return status;
}

UmiStatus umi_vcs_ui_workspace_conflicts_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-conflicts", "Merge Conflicts",
        "Conflicting paths and provider-neutral conflict classifications.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    count = visible_rows(snapshot.conflict_count);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count",
                                        (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiVcsConflict conflict;
        char key[64];
        char text[UMI_VCS_PATH_CAPACITY + 128U];
        status = umi_vcs_workspace_coordinator_conflict_at(
            coordinator, index, &conflict);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "%s [%s] — %s",
                       conflict.path, conflict.index_code,
                       umi_vcs_conflict_kind_text(conflict.kind));
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.select-change", "Select Conflict…",
        "Select a conflicting repository path", count > 0U);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 1U, "studio.action.vcs.diff-selected", "Open Diff",
        "Inspect the selected conflict", snapshot.can_open_selected_diff);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 2U, "studio.action.vcs.refresh", "Refresh",
        "Refresh conflict state", snapshot.can_refresh);
    return status;
}

UmiStatus umi_vcs_ui_workspace_diff_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-diff", "Diff Viewer",
        "Unified working-tree or staged diff with classified lines and hunks.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_string(*out_view, "vcs.diff-path",
                                       snapshot.diff_path);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_boolean(*out_view, "vcs.diff-staged",
                                        snapshot.diff_staged);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_boolean(*out_view, "vcs.diff-binary",
                                        snapshot.diff_binary);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.diff-additions",
            (int64_t)snapshot.diff_addition_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.diff-deletions",
            (int64_t)snapshot.diff_deletion_count);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.diff-hunks",
            (int64_t)snapshot.diff_hunk_count);
    }
    count = visible_rows(snapshot.diff_line_count);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count",
                                        (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiVcsDiffLine line;
        char key[64];
        char text[UMI_VCS_DIFF_TEXT_CAPACITY + 96U];
        status = umi_vcs_workspace_coordinator_diff_line_at(
            coordinator, index, &line);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "[%s] %6zu %6zu  %s",
                       umi_vcs_diff_line_kind_text(line.kind),
                       line.old_line, line.new_line, line.text);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.diff-selected", "Working Tree",
        "Load the selected working-tree diff",
        snapshot.can_open_selected_diff);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 1U, "studio.action.vcs.diff-selected-staged", "Staged",
        "Load the selected index diff", snapshot.can_open_selected_diff);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 2U, "studio.action.vcs.stage-selected", "Stage",
        "Stage the selected path", snapshot.can_stage_selected);
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 3U, "studio.action.vcs.unstage-selected", "Unstage",
        "Unstage the selected path", snapshot.can_unstage_selected);
    return status;
}

UmiStatus umi_vcs_ui_workspace_operations_view_create(
    const char *view_id,
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiUiViewModel **out_view)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status = umi_vcs_ui_base_view(
        view_id, "source-control-operations", "Source Control Operations",
        "Auditable repository refresh, staging, commit and synchronization journal.",
        out_view);
    size_t count;
    size_t index;

    if (status != UMI_STATUS_OK) return status;
    status = coordinator_snapshot(coordinator, &snapshot);
    if (status == UMI_STATUS_OK) {
        status = set_repository_properties(*out_view, &snapshot);
    }
    count = visible_rows(snapshot.operation_count);
    if (status == UMI_STATUS_OK) {
        status = umi_vcs_ui_set_integer(*out_view, "vcs.row-count",
                                        (int64_t)count);
    }
    for (index = 0U; status == UMI_STATUS_OK && index < count; ++index) {
        UmiVcsOperation operation;
        char key[64];
        char text[UMI_VCS_PATH_CAPACITY +
                  UMI_VCS_MESSAGE_CAPACITY + 160U];
        status = umi_vcs_workspace_coordinator_operation_at(
            coordinator, index, &operation);
        if (status != UMI_STATUS_OK) break;
        (void)snprintf(key, sizeof(key), "vcs.row.%zu", index);
        (void)snprintf(text, sizeof(text), "#%llu %s [%s] %s — %s",
                       (unsigned long long)operation.operation_id,
                       umi_vcs_operation_kind_text(operation.kind),
                       umi_vcs_operation_state_text(operation.state),
                       operation.subject, operation.summary);
        status = umi_vcs_ui_set_string(*out_view, key, text);
    }
    if (status == UMI_STATUS_OK) status = umi_vcs_ui_set_action(
        *out_view, 0U, "studio.action.vcs.refresh", "Refresh",
        "Refresh repository state and operation journal",
        snapshot.can_refresh);
    return status;
}
