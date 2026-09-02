/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workspace_coordinator.c
 *
 * PURPOSE:
 *   Implement professional source-control workspace state over the canonical
 *   provider-neutral UmiVcsWorkspace model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/workspace_coordinator.h"

#include <stdlib.h>
#include <string.h>

struct UmiVcsWorkspaceCoordinator {
    UmiVcsWorkspace *workspace;
    UmiVcsChangeFilter change_filter;
    char selected_change_path[UMI_VCS_PATH_CAPACITY];
    char selected_commit_id[UMI_VCS_ID_CAPACITY];
    char selected_branch_name[UMI_VCS_NAME_CAPACITY];
    char selected_remote_name[UMI_VCS_NAME_CAPACITY];
    char commit_message[UMI_VCS_COMMIT_MESSAGE_CAPACITY];
    uint64_t revision;
    int show_untracked;
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Provide the change has unstaged content operation used by this module and its client
 * applications.
 */
static int change_has_unstaged_content(const UmiVcsChange *change)
{
    return change != NULL &&
           (!change->staged ||
            change->worktree_state != UMI_VCS_CHANGE_UNMODIFIED);
}

/*
 * Provide the change is conflicted operation used by this module and its client
 * applications.
 */
static int change_is_conflicted(UmiVcsWorkspaceCoordinator *coordinator,
                                const UmiVcsChange *change)
{
    const UmiVcsConflictList *conflicts;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || change == NULL) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (change->index_state == UMI_VCS_CHANGE_CONFLICTED ||
        change->worktree_state == UMI_VCS_CHANGE_CONFLICTED) {
        return 1;
    }
    conflicts = umi_vcs_workspace_conflicts(coordinator->workspace);
    return umi_vcs_conflict_list_find(conflicts, change->path) != NULL;
}

/* Provide the change matches operation used by this module and its client applications. */
static int change_matches(UmiVcsWorkspaceCoordinator *coordinator,
                          const UmiVcsChange *change)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || change == NULL) return 0;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!coordinator->show_untracked &&
        change->index_state == UMI_VCS_CHANGE_UNTRACKED &&
        change->worktree_state == UMI_VCS_CHANGE_UNTRACKED) {
        return 0;
    }
    /* Select the behaviour associated with the requested command or state value. */
    switch (coordinator->change_filter) {
        case UMI_VCS_CHANGE_FILTER_STAGED:
            return change->staged;
        case UMI_VCS_CHANGE_FILTER_UNSTAGED:
            return change_has_unstaged_content(change);
        case UMI_VCS_CHANGE_FILTER_CONFLICTS:
            return change_is_conflicted(coordinator, change);
        case UMI_VCS_CHANGE_FILTER_ALL:
        default:
            return 1;
    }
}

/* Return the number of records represented by visible change without changing their state. */
static size_t visible_change_count(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    const UmiVcsChangeList *changes =
        umi_vcs_workspace_changes(coordinator->workspace);
    size_t count = umi_vcs_change_list_count(changes);
    size_t index;
    size_t visible = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (change_matches(coordinator,
                           umi_vcs_change_list_at(changes, index))) {
            visible += 1U;
        }
    }
    return visible;
}

/*
 * Return the number of records represented by unstaged change without changing their
 * state.
 */
static size_t unstaged_change_count(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    const UmiVcsChangeList *changes =
        umi_vcs_workspace_changes(coordinator->workspace);
    size_t count = umi_vcs_change_list_count(changes);
    size_t index;
    size_t unstaged = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (change_has_unstaged_content(
                umi_vcs_change_list_at(changes, index))) {
            unstaged += 1U;
        }
    }
    return unstaged;
}

/* Provide the capability operation used by this module and its client applications. */
static int capability(const UmiVcsWorkspaceSnapshot *snapshot,
                      UmiVcsCapability requested)
{
    return snapshot != NULL &&
           (snapshot->capabilities & (uint64_t)requested) != 0U;
}

/*
 * Provide the reconcile selection operation used by this module and its client
 * applications.
 */
static void reconcile_selection(UmiVcsWorkspaceCoordinator *coordinator)
{
    const UmiVcsChangeList *changes;
    const UmiVcsHistory *history;
    const UmiVcsBranchList *branches;
    const UmiVcsRemoteList *remotes;
    size_t index;
    int found;

    changes = umi_vcs_workspace_changes(coordinator->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_change_path[0] != '\0' &&
        umi_vcs_change_list_find(changes,
                                 coordinator->selected_change_path) == NULL) {
        coordinator->selected_change_path[0] = '\0';
    }

    history = umi_vcs_workspace_history(coordinator->workspace);
    found = coordinator->selected_commit_id[0] == '\0';
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; !found && index < umi_vcs_history_count(history); ++index) {
        const UmiVcsCommit *commit = umi_vcs_history_at(history, index);
        found = commit != NULL &&
                strcmp(commit->commit_id,
                       coordinator->selected_commit_id) == 0;
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!found) coordinator->selected_commit_id[0] = '\0';

    branches = umi_vcs_workspace_branches(coordinator->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_branch_name[0] != '\0' &&
        umi_vcs_branch_list_find(branches,
                                 coordinator->selected_branch_name) == NULL) {
        coordinator->selected_branch_name[0] = '\0';
    }
    remotes = umi_vcs_workspace_remotes(coordinator->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_remote_name[0] != '\0' &&
        umi_vcs_remote_list_find(remotes,
                                 coordinator->selected_remote_name) == NULL) {
        coordinator->selected_remote_name[0] = '\0';
    }
}

/* Provide the choose defaults operation used by this module and its client applications. */
static void choose_defaults(UmiVcsWorkspaceCoordinator *coordinator)
{
    const UmiVcsChangeList *changes;
    const UmiVcsHistory *history;
    const UmiVcsBranchList *branches;
    const UmiVcsRemoteList *remotes;
    size_t index;

    changes = umi_vcs_workspace_changes(coordinator->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_change_path[0] == '\0') {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < umi_vcs_change_list_count(changes); ++index) {
            const UmiVcsChange *change =
                umi_vcs_change_list_at(changes, index);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (change_matches(coordinator, change)) {
                (void)copy_text(coordinator->selected_change_path,
                                sizeof(coordinator->selected_change_path),
                                change->path);
                break;
            }
        }
    }

    history = umi_vcs_workspace_history(coordinator->workspace);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (coordinator->selected_commit_id[0] == '\0' &&
        umi_vcs_history_count(history) > 0U) {
        const UmiVcsCommit *commit = umi_vcs_history_at(history, 0U);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (commit != NULL) {
            (void)copy_text(coordinator->selected_commit_id,
                            sizeof(coordinator->selected_commit_id),
                            commit->commit_id);
        }
    }

    branches = umi_vcs_workspace_branches(coordinator->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_branch_name[0] == '\0') {
        const UmiVcsBranch *branch = umi_vcs_branch_list_current(branches);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (branch == NULL && umi_vcs_branch_list_count(branches) > 0U) {
            branch = umi_vcs_branch_list_at(branches, 0U);
        }
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (branch != NULL) {
            (void)copy_text(coordinator->selected_branch_name,
                            sizeof(coordinator->selected_branch_name),
                            branch->name);
        }
    }

    remotes = umi_vcs_workspace_remotes(coordinator->workspace);
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_remote_name[0] == '\0' &&
        umi_vcs_remote_list_count(remotes) > 0U) {
        const UmiVcsRemote *remote = umi_vcs_remote_list_at(remotes, 0U);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (remote != NULL) {
            (void)copy_text(coordinator->selected_remote_name,
                            sizeof(coordinator->selected_remote_name),
                            remote->name);
        }
    }
}

/*
 * Initialise vcs workspace coordinator from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_vcs_workspace_coordinator_create(
    UmiVcsWorkspace *workspace,
    UmiVcsWorkspaceCoordinator **out_coordinator)
{
    UmiVcsWorkspaceCoordinator *coordinator;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_coordinator == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    *out_coordinator = NULL;
    coordinator = (UmiVcsWorkspaceCoordinator *)calloc(
        1U, sizeof(*coordinator));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    coordinator->workspace = workspace;
    coordinator->change_filter = UMI_VCS_CHANGE_FILTER_ALL;
    coordinator->show_untracked = 1;
    coordinator->revision = 1U;
    reconcile_selection(coordinator);
    choose_defaults(coordinator);
    *out_coordinator = coordinator;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by vcs workspace coordinator so the same storage can be
 * reused safely.
 */
void umi_vcs_workspace_coordinator_destroy(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    free(coordinator);
}

/*
 * Provide the vcs workspace coordinator refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_refresh(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t history_limit)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_workspace_refresh(coordinator->workspace, history_limit);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        reconcile_selection(coordinator);
        choose_defaults(coordinator);
        coordinator->revision += 1U;
    }
    return status;
}

/*
 * Provide the vcs workspace coordinator synchronise operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_synchronise(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    reconcile_selection(coordinator);
    choose_defaults(coordinator);
    coordinator->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workspace coordinator snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_snapshot(
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiVcsWorkspaceCoordinatorSnapshot *out_snapshot)
{
    const UmiVcsChange *selected = NULL;
    UmiVcsDiffDocumentSnapshot diff;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    reconcile_selection(coordinator);
    choose_defaults(coordinator);
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    out_snapshot->struct_size = (uint32_t)sizeof(*out_snapshot);
    status = umi_vcs_workspace_snapshot(coordinator->workspace,
                                        &out_snapshot->repository);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    out_snapshot->revision = coordinator->revision;
    out_snapshot->change_filter = coordinator->change_filter;
    (void)copy_text(out_snapshot->change_filter_label,
                    sizeof(out_snapshot->change_filter_label),
                    umi_vcs_change_filter_text(coordinator->change_filter));
    (void)copy_text(out_snapshot->selected_change_path,
                    sizeof(out_snapshot->selected_change_path),
                    coordinator->selected_change_path);
    (void)copy_text(out_snapshot->selected_commit_id,
                    sizeof(out_snapshot->selected_commit_id),
                    coordinator->selected_commit_id);
    (void)copy_text(out_snapshot->selected_branch_name,
                    sizeof(out_snapshot->selected_branch_name),
                    coordinator->selected_branch_name);
    (void)copy_text(out_snapshot->selected_remote_name,
                    sizeof(out_snapshot->selected_remote_name),
                    coordinator->selected_remote_name);
    (void)copy_text(out_snapshot->commit_message,
                    sizeof(out_snapshot->commit_message),
                    coordinator->commit_message);
    out_snapshot->show_untracked = coordinator->show_untracked;
    out_snapshot->visible_change_count = visible_change_count(coordinator);
    out_snapshot->staged_change_count = out_snapshot->repository.staged;
    out_snapshot->unstaged_change_count = unstaged_change_count(coordinator);
    out_snapshot->conflict_count = out_snapshot->repository.conflicts;
    out_snapshot->history_count = out_snapshot->repository.commits;
    out_snapshot->branch_count = out_snapshot->repository.branches;
    out_snapshot->remote_count = out_snapshot->repository.remotes;
    out_snapshot->tag_count = out_snapshot->repository.tags;
    out_snapshot->diff_line_count = out_snapshot->repository.diff_lines;
    out_snapshot->operation_count = out_snapshot->repository.operations;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_diff_document_snapshot(
            umi_vcs_workspace_diff(coordinator->workspace), &diff) ==
        UMI_STATUS_OK) {
        (void)copy_text(out_snapshot->diff_path,
                        sizeof(out_snapshot->diff_path), diff.path);
        out_snapshot->diff_staged = diff.staged;
        out_snapshot->diff_binary = diff.binary;
        out_snapshot->diff_addition_count = diff.additions;
        out_snapshot->diff_deletion_count = diff.deletions;
        out_snapshot->diff_hunk_count = diff.hunks;
    }

    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_change_path[0] != '\0') {
        selected = umi_vcs_change_list_find(
            umi_vcs_workspace_changes(coordinator->workspace),
            coordinator->selected_change_path);
    }
    out_snapshot->can_refresh =
        out_snapshot->repository.available &&
        capability(&out_snapshot->repository, UMI_VCS_CAPABILITY_STATUS);
    out_snapshot->can_stage_selected = selected != NULL &&
        !selected->staged && capability(&out_snapshot->repository,
                                        UMI_VCS_CAPABILITY_STAGE);
    out_snapshot->can_unstage_selected = selected != NULL &&
        selected->staged && capability(&out_snapshot->repository,
                                       UMI_VCS_CAPABILITY_STAGE);
    out_snapshot->can_discard_selected = selected != NULL &&
        change_has_unstaged_content(selected) &&
        !change_is_conflicted(coordinator, selected) &&
        capability(&out_snapshot->repository, UMI_VCS_CAPABILITY_DISCARD);
    out_snapshot->can_commit = coordinator->commit_message[0] != '\0' &&
        out_snapshot->staged_change_count > 0U &&
        out_snapshot->conflict_count == 0U &&
        capability(&out_snapshot->repository, UMI_VCS_CAPABILITY_COMMIT);
    out_snapshot->can_fetch = out_snapshot->remote_count > 0U &&
        capability(&out_snapshot->repository, UMI_VCS_CAPABILITY_FETCH);
    out_snapshot->can_pull = out_snapshot->repository.upstream[0] != '\0' &&
        out_snapshot->conflict_count == 0U &&
        capability(&out_snapshot->repository, UMI_VCS_CAPABILITY_PULL);
    out_snapshot->can_push = out_snapshot->remote_count > 0U &&
        capability(&out_snapshot->repository, UMI_VCS_CAPABILITY_PUSH);
    out_snapshot->can_create_branch = capability(
        &out_snapshot->repository, UMI_VCS_CAPABILITY_BRANCH_CREATE);
    out_snapshot->can_checkout_selected_branch =
        coordinator->selected_branch_name[0] != '\0' &&
        strcmp(coordinator->selected_branch_name,
               out_snapshot->repository.branch) != 0 &&
        capability(&out_snapshot->repository,
                   UMI_VCS_CAPABILITY_BRANCH_CHECKOUT);
    out_snapshot->can_delete_selected_branch =
        out_snapshot->can_checkout_selected_branch &&
        capability(&out_snapshot->repository,
                   UMI_VCS_CAPABILITY_BRANCH_DELETE);
    out_snapshot->can_open_selected_diff = selected != NULL &&
        capability(&out_snapshot->repository, UMI_VCS_CAPABILITY_DIFF);
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workspace coordinator set change filter operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_set_change_filter(
    UmiVcsWorkspaceCoordinator *coordinator,
    UmiVcsChangeFilter filter)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || filter < UMI_VCS_CHANGE_FILTER_ALL ||
        filter > UMI_VCS_CHANGE_FILTER_CONFLICTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->change_filter != filter) {
        coordinator->change_filter = filter;
        coordinator->selected_change_path[0] = '\0';
        choose_defaults(coordinator);
        coordinator->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workspace coordinator set show untracked operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_set_show_untracked(
    UmiVcsWorkspaceCoordinator *coordinator,
    int enabled)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    enabled = enabled != 0;
    /* Apply this operation only while the related capability or state is available. */
    if (coordinator->show_untracked != enabled) {
        coordinator->show_untracked = enabled;
        /* Apply this branch only when its contract condition is satisfied. */
        if (coordinator->selected_change_path[0] != '\0') {
            const UmiVcsChange *selected = umi_vcs_change_list_find(
                umi_vcs_workspace_changes(coordinator->workspace),
                coordinator->selected_change_path);
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (!change_matches(coordinator, selected)) {
                coordinator->selected_change_path[0] = '\0';
            }
        }
        choose_defaults(coordinator);
        coordinator->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workspace coordinator select change operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_change(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *path)
{
    const UmiVcsChange *change;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || path == NULL || path[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    change = umi_vcs_change_list_find(
        umi_vcs_workspace_changes(coordinator->workspace), path);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (change == NULL) return UMI_STATUS_NOT_FOUND;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!change_matches(coordinator, change)) return UMI_STATUS_UNAVAILABLE;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(coordinator->selected_change_path, path) != 0) {
        UmiStatus status = copy_text(coordinator->selected_change_path,
                                     sizeof(coordinator->selected_change_path),
                                     path);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        coordinator->revision += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs workspace coordinator select commit operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_commit(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *commit_id)
{
    const UmiVcsHistory *history;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || commit_id == NULL || commit_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    history = umi_vcs_workspace_history(coordinator->workspace);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_vcs_history_count(history); ++index) {
        const UmiVcsCommit *commit = umi_vcs_history_at(history, index);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (commit != NULL && strcmp(commit->commit_id, commit_id) == 0) {
            UmiStatus status = copy_text(
                coordinator->selected_commit_id,
                sizeof(coordinator->selected_commit_id), commit_id);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status == UMI_STATUS_OK) coordinator->revision += 1U;
            return status;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Provide the vcs workspace coordinator select branch operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_branch(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *branch_name)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || branch_name == NULL ||
        branch_name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_branch_list_find(
            umi_vcs_workspace_branches(coordinator->workspace),
            branch_name) == NULL) return UMI_STATUS_NOT_FOUND;
    status = copy_text(coordinator->selected_branch_name,
                       sizeof(coordinator->selected_branch_name), branch_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) coordinator->revision += 1U;
    return status;
}

/*
 * Provide the vcs workspace coordinator select remote operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_select_remote(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *remote_name)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || remote_name == NULL ||
        remote_name[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_remote_list_find(
            umi_vcs_workspace_remotes(coordinator->workspace),
            remote_name) == NULL) return UMI_STATUS_NOT_FOUND;
    status = copy_text(coordinator->selected_remote_name,
                       sizeof(coordinator->selected_remote_name), remote_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) coordinator->revision += 1U;
    return status;
}

/*
 * Provide the vcs workspace coordinator set commit message operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_set_commit_message(
    UmiVcsWorkspaceCoordinator *coordinator,
    const char *message)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = copy_text(coordinator->commit_message,
                       sizeof(coordinator->commit_message), message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) coordinator->revision += 1U;
    return status;
}

/*
 * Provide the vcs workspace coordinator clear selection operation used by this module and
 * its client applications.
 */
void umi_vcs_workspace_coordinator_clear_selection(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return;
    coordinator->selected_change_path[0] = '\0';
    coordinator->selected_commit_id[0] = '\0';
    coordinator->selected_branch_name[0] = '\0';
    coordinator->selected_remote_name[0] = '\0';
    choose_defaults(coordinator);
    coordinator->revision += 1U;
}

/*
 * Find vcs workspace coordinator stage while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_stage_selected(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_change_path[0] == '\0') {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_vcs_workspace_stage(coordinator->workspace,
                                     coordinator->selected_change_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        reconcile_selection(coordinator);
        choose_defaults(coordinator);
        coordinator->revision += 1U;
    }
    return status;
}

/*
 * Find vcs workspace coordinator unstage while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_unstage_selected(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_change_path[0] == '\0') {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_vcs_workspace_unstage(coordinator->workspace,
                                       coordinator->selected_change_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        reconcile_selection(coordinator);
        choose_defaults(coordinator);
        coordinator->revision += 1U;
    }
    return status;
}

/*
 * Find vcs workspace coordinator discard while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_discard_selected(
    UmiVcsWorkspaceCoordinator *coordinator)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_change_path[0] == '\0') {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_vcs_workspace_discard(coordinator->workspace,
                                       coordinator->selected_change_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        reconcile_selection(coordinator);
        choose_defaults(coordinator);
        coordinator->revision += 1U;
    }
    return status;
}

/*
 * Provide the vcs workspace coordinator commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_commit(
    UmiVcsWorkspaceCoordinator *coordinator,
    char *out_commit_id,
    size_t capacity)
{
    UmiVcsWorkspaceCoordinatorSnapshot snapshot;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_workspace_coordinator_snapshot(coordinator, &snapshot);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!snapshot.can_commit) return UMI_STATUS_UNAVAILABLE;
    status = umi_vcs_workspace_commit(coordinator->workspace,
                                      coordinator->commit_message,
                                      out_commit_id, capacity);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        coordinator->commit_message[0] = '\0';
        reconcile_selection(coordinator);
        choose_defaults(coordinator);
        coordinator->revision += 1U;
    }
    return status;
}

/*
 * Provide the vcs workspace coordinator open selected diff operation used by this module
 * and its client applications.
 */
UmiStatus umi_vcs_workspace_coordinator_open_selected_diff(
    UmiVcsWorkspaceCoordinator *coordinator,
    int staged)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (coordinator->selected_change_path[0] == '\0') {
        return UMI_STATUS_UNAVAILABLE;
    }
    status = umi_vcs_workspace_open_diff(coordinator->workspace,
                                         coordinator->selected_change_path,
                                         staged);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) coordinator->revision += 1U;
    return status;
}

/*
 * Find vcs workspace coordinator change while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_change_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t visible_index,
    UmiVcsChange *out_change)
{
    const UmiVcsChangeList *changes;
    size_t count;
    size_t index;
    size_t visible = 0U;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_change == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    changes = umi_vcs_workspace_changes(coordinator->workspace);
    count = umi_vcs_change_list_count(changes);
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const UmiVcsChange *change = umi_vcs_change_list_at(changes, index);
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!change_matches(coordinator, change)) continue;
        /* Apply this operation only while the related capability or state is available. */
        if (visible == visible_index) {
            *out_change = *change;
            return UMI_STATUS_OK;
        }
        visible += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find vcs workspace coordinator conflict while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_conflict_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsConflict *out_conflict)
{
    const UmiVcsConflict *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_conflict == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_vcs_conflict_list_at(
        umi_vcs_workspace_conflicts(coordinator->workspace), index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_conflict = *item;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workspace coordinator commit while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_commit_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsCommit *out_commit)
{
    const UmiVcsCommit *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_commit == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_vcs_history_at(
        umi_vcs_workspace_history(coordinator->workspace), index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_commit = *item;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workspace coordinator branch while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_branch_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsBranch *out_branch)
{
    const UmiVcsBranch *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_branch == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_vcs_branch_list_at(
        umi_vcs_workspace_branches(coordinator->workspace), index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_branch = *item;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workspace coordinator remote while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_remote_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsRemote *out_remote)
{
    const UmiVcsRemote *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_remote == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_vcs_remote_list_at(
        umi_vcs_workspace_remotes(coordinator->workspace), index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_remote = *item;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workspace coordinator tag while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_tag_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsTag *out_tag)
{
    const UmiVcsTag *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_tag == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_vcs_tag_list_at(
        umi_vcs_workspace_tags(coordinator->workspace), index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_tag = *item;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workspace coordinator diff line while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_diff_line_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsDiffLine *out_line)
{
    const UmiVcsDiffLine *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_line == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_vcs_diff_document_line_at(
        umi_vcs_workspace_diff(coordinator->workspace), index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_line = *item;
    return UMI_STATUS_OK;
}

/*
 * Find vcs workspace coordinator operation while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiStatus umi_vcs_workspace_coordinator_operation_at(
    UmiVcsWorkspaceCoordinator *coordinator,
    size_t index,
    UmiVcsOperation *out_operation)
{
    const UmiVcsOperation *item;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (coordinator == NULL || out_operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = umi_vcs_operation_log_at(
        umi_vcs_workspace_operations(coordinator->workspace), index);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_NOT_FOUND;
    *out_operation = *item;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs change filter text operation used by this module and its client
 * applications.
 */
const char *umi_vcs_change_filter_text(UmiVcsChangeFilter filter)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (filter) {
        case UMI_VCS_CHANGE_FILTER_STAGED: return "staged";
        case UMI_VCS_CHANGE_FILTER_UNSTAGED: return "unstaged";
        case UMI_VCS_CHANGE_FILTER_CONFLICTS: return "conflicts";
        case UMI_VCS_CHANGE_FILTER_ALL: return "all changes";
        default: return "unknown";
    }
}
