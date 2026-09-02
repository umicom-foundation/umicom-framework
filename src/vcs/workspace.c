/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/workspace.c
 *
 * PURPOSE:
 *   Implement the workspace behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework operational source-control workspace implementation.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/workspace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/vcs/git_cli.h"
#include "umicom/vcs/repository.h"
#include "umicom/platform/process.h"
struct UmiVcsWorkspace {
    char root[UMI_VCS_PATH_CAPACITY];
    char provider_id[UMI_VCS_ID_CAPACITY];
    UmiVcsProvider direct_provider;
    UmiVcsRepository *repository;
    UmiVcsChangeList *changes;
    UmiVcsConflictList *conflicts;
    UmiVcsHistory *history;
    UmiVcsBranchList *branches;
    UmiVcsRemoteList *remotes;
    UmiVcsTagList *tags;
    UmiVcsDiffDocument *diff;
    UmiVcsOperationLog *operations;
    UmiVcsBranch branch;
    size_t history_limit;
    uint64_t revision;
    int available;
    int owns_direct_provider;
};
/*
 * Initialise workspace models from caller-provided values so later operations receive a
 * known state.
 */
static UmiStatus workspace_models_create(UmiVcsWorkspace *workspace)
{
    UmiStatus status = umi_vcs_change_list_create(&workspace->changes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_conflict_list_create(&workspace->conflicts);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_history_create(&workspace->history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_branch_list_create(&workspace->branches);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_remote_list_create(&workspace->remotes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_tag_list_create(&workspace->tags);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_diff_document_create(&workspace->diff);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_operation_log_create(&workspace->operations);
    return status;
}
/*
 * Initialise vcs workspace from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_vcs_workspace_create(const char *root, const UmiVcsProvider *provider, int available, UmiVcsWorkspace **out_workspace)
{
    UmiVcsWorkspace *workspace;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || root[0] == '\0' || provider == NULL || out_workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_workspace = NULL;
    status = umi_vcs_provider_validate(provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (strlen(root) + 1U > UMI_VCS_PATH_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    workspace = calloc(1U, sizeof(*workspace));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)snprintf(workspace->root, sizeof(workspace->root), "%s", root);
    (void)snprintf(workspace->provider_id, sizeof(workspace->provider_id), "%s", provider->provider_id);
    workspace->available = available != 0; workspace->history_limit = 50U; workspace->revision = 1U;
    status = workspace_models_create(workspace);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && workspace->available) {
        status = umi_vcs_repository_create(root, provider, &workspace->repository);
    } else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status == UMI_STATUS_OK) {
        workspace->direct_provider = *provider; workspace->owns_direct_provider = 1;
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) { umi_vcs_workspace_destroy(workspace); return status; }
    *out_workspace = workspace; return UMI_STATUS_OK;
}
/*
 * Provide the vcs workspace create git operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_create_git(const char *root, UmiVcsWorkspace **out_workspace)
{
    UmiVcsProvider provider;
    UmiStatus status;
    int is_repository = 0;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (root == NULL || out_workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_git_cli_provider(&provider);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_git_cli_is_repository(root, &is_repository);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) is_repository = 0;
    status = umi_vcs_workspace_create(root, &provider, is_repository, out_workspace);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != UMI_STATUS_OK && provider.destroy != NULL) provider.destroy(provider.instance);
    return status;
}
/* Release or reset state held by vcs workspace so the same storage can be reused safely. */
void umi_vcs_workspace_destroy(UmiVcsWorkspace *workspace)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace->repository != NULL) umi_vcs_repository_destroy(workspace->repository);
    else /* Protect caller-owned memory by checking that required state is available before it is used. */ if (workspace->owns_direct_provider && workspace->direct_provider.destroy != NULL)
        workspace->direct_provider.destroy(workspace->direct_provider.instance);
    umi_vcs_operation_log_destroy(workspace->operations);
    umi_vcs_diff_document_destroy(workspace->diff);
    umi_vcs_tag_list_destroy(workspace->tags);
    umi_vcs_remote_list_destroy(workspace->remotes);
    umi_vcs_branch_list_destroy(workspace->branches);
    umi_vcs_history_destroy(workspace->history);
    umi_vcs_conflict_list_destroy(workspace->conflicts);
    umi_vcs_change_list_destroy(workspace->changes);
    free(workspace);
}
/* Provide the refresh optional operation used by this module and its client applications. */
static UmiStatus refresh_optional(UmiStatus status)
{ return status == UMI_STATUS_NOT_IMPLEMENTED ? UMI_STATUS_OK : status; }
/*
 * Provide the workspace refresh internal operation used by this module and its client
 * applications.
 */
static UmiStatus workspace_refresh_internal(UmiVcsWorkspace *workspace, size_t history_limit, int record)
{
    UmiStatus status;
    uint64_t operation_id = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!workspace->available || workspace->repository == NULL) return UMI_STATUS_UNAVAILABLE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (history_limit > 0U) workspace->history_limit = history_limit;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (record) (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_REFRESH, workspace->root, &operation_id);
    status = umi_vcs_repository_status(workspace->repository, workspace->changes, &workspace->branch);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_conflict_list_build(workspace->conflicts, workspace->changes);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_repository_history(workspace->repository, workspace->history_limit, workspace->history);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = refresh_optional(umi_vcs_repository_branches(workspace->repository, workspace->branches));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = refresh_optional(umi_vcs_repository_remotes(workspace->repository, workspace->remotes));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = refresh_optional(umi_vcs_repository_tags(workspace->repository, workspace->tags));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (record) (void)umi_vcs_operation_log_finish(workspace->operations, operation_id, status,
        status == UMI_STATUS_OK ? "Repository models refreshed" : "Repository refresh failed");
    return status;
}
/*
 * Provide the vcs workspace refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_refresh(UmiVcsWorkspace *workspace, size_t history_limit)
{ return workspace_refresh_internal(workspace, history_limit, 1); }
/*
 * Provide the vcs workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_snapshot(const UmiVcsWorkspace *workspace, UmiVcsWorkspaceSnapshot *out_snapshot)
{
    UmiVcsDiffDocumentSnapshot diff_snapshot;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workspace == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(out_snapshot->root, sizeof(out_snapshot->root), "%s", workspace->root);
    (void)snprintf(out_snapshot->provider_id, sizeof(out_snapshot->provider_id), "%s", workspace->provider_id);
    (void)snprintf(out_snapshot->branch, sizeof(out_snapshot->branch), "%s", workspace->branch.name);
    (void)snprintf(out_snapshot->upstream, sizeof(out_snapshot->upstream), "%s", workspace->branch.upstream);
    out_snapshot->available = workspace->available; out_snapshot->detached = workspace->branch.detached;
    out_snapshot->ahead = workspace->branch.ahead; out_snapshot->behind = workspace->branch.behind;
    out_snapshot->changes = umi_vcs_change_list_count(workspace->changes);
    out_snapshot->staged = umi_vcs_change_list_staged_count(workspace->changes);
    out_snapshot->conflicts = umi_vcs_conflict_list_count(workspace->conflicts);
    out_snapshot->commits = umi_vcs_history_count(workspace->history);
    out_snapshot->branches = umi_vcs_branch_list_count(workspace->branches);
    out_snapshot->remotes = umi_vcs_remote_list_count(workspace->remotes);
    out_snapshot->tags = umi_vcs_tag_list_count(workspace->tags);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_diff_document_snapshot(workspace->diff, &diff_snapshot) == UMI_STATUS_OK)
        out_snapshot->diff_lines = diff_snapshot.lines;
    out_snapshot->operations = umi_vcs_operation_log_count(workspace->operations);
    out_snapshot->revision = workspace->revision;
    out_snapshot->capabilities = workspace->repository != NULL
        ? umi_vcs_repository_capabilities(workspace->repository)
        : umi_vcs_provider_capabilities(&workspace->direct_provider);
    return UMI_STATUS_OK;
}
/* Provide the finish mutation operation used by this module and its client applications. */
static UmiStatus finish_mutation(UmiVcsWorkspace *workspace, uint64_t operation_id, UmiStatus status, const char *success)
{
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        UmiStatus refresh_status = workspace_refresh_internal(workspace, workspace->history_limit, 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (refresh_status != UMI_STATUS_OK) status = refresh_status;
    }
    (void)umi_vcs_operation_log_finish(workspace->operations, operation_id, status,
        status == UMI_STATUS_OK ? success : "Source-control operation failed");
    return status;
}
#define UMI_VCS_WORKSPACE_READY(workspace_) \
    ((workspace_) != NULL && (workspace_)->available && (workspace_)->repository != NULL)
/*
 * Provide the vcs workspace stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_stage(UmiVcsWorkspace *workspace, const char *path)
{
    uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || path == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_STAGE, path, &id);
    status = umi_vcs_repository_stage(workspace->repository, path);
    return finish_mutation(workspace, id, status, "Path staged");
}
/*
 * Provide the vcs workspace unstage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_unstage(UmiVcsWorkspace *workspace, const char *path)
{
    uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || path == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_UNSTAGE, path, &id);
    status = umi_vcs_repository_unstage(workspace->repository, path);
    return finish_mutation(workspace, id, status, "Path unstaged");
}
/*
 * Provide the vcs workspace stage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_stage_all(UmiVcsWorkspace *workspace)
{
    uint64_t id = 0U; UmiStatus status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_WORKSPACE_READY(workspace)) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_STAGE, "all changes", &id);
    status = umi_vcs_repository_stage_all(workspace->repository);
    return finish_mutation(workspace, id, status, "All changes staged");
}
/*
 * Provide the vcs workspace unstage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_unstage_all(UmiVcsWorkspace *workspace)
{
    uint64_t id = 0U; UmiStatus status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_WORKSPACE_READY(workspace)) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_UNSTAGE, "all changes", &id);
    status = umi_vcs_repository_unstage_all(workspace->repository);
    return finish_mutation(workspace, id, status, "All changes unstaged");
}
/*
 * Provide the vcs workspace discard operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_discard(UmiVcsWorkspace *workspace, const char *path)
{
    uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || path == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_DISCARD, path, &id);
    status = umi_vcs_repository_discard(workspace->repository, path);
    return finish_mutation(workspace, id, status, "Working-tree change discarded");
}
/*
 * Provide the vcs workspace commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_commit(UmiVcsWorkspace *workspace, const char *message, char *out_commit_id, size_t capacity)
{
    uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || message == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_COMMIT, message, &id);
    status = umi_vcs_repository_commit(workspace->repository, message, out_commit_id, capacity);
    return finish_mutation(workspace, id, status, "Commit created");
}
/* Provide the sync operation operation used by this module and its client applications. */
static UmiStatus sync_operation(UmiVcsWorkspace *workspace, UmiVcsOperationKind kind)
{
    uint64_t id = 0U; UmiStatus status;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!UMI_VCS_WORKSPACE_READY(workspace)) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, kind, workspace->branch.upstream, &id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (kind == UMI_VCS_OPERATION_FETCH) status = umi_vcs_repository_fetch(workspace->repository);
    else /* Preserve the original failure result so the caller can respond to the correct cause. */ if (kind == UMI_VCS_OPERATION_PULL) status = umi_vcs_repository_pull(workspace->repository);
    /* Use this fallback path when the earlier condition does not apply. */
    else status = umi_vcs_repository_push(workspace->repository);
    return finish_mutation(workspace, id, status, "Remote synchronization completed");
}
/*
 * Provide the vcs workspace fetch operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_fetch(UmiVcsWorkspace *workspace)
{ return sync_operation(workspace, UMI_VCS_OPERATION_FETCH); }
/*
 * Provide the vcs workspace pull operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_pull(UmiVcsWorkspace *workspace)
{ return sync_operation(workspace, UMI_VCS_OPERATION_PULL); }
/*
 * Provide the vcs workspace push operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_push(UmiVcsWorkspace *workspace)
{ return sync_operation(workspace, UMI_VCS_OPERATION_PUSH); }
/*
 * Initialise vcs workspace branch from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_vcs_workspace_branch_create(UmiVcsWorkspace *workspace, const char *name, int checkout)
{
    uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || name == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_BRANCH_CREATE, name, &id);
    status = umi_vcs_repository_branch_create(workspace->repository, name, checkout);
    return finish_mutation(workspace, id, status, "Branch created");
}
/*
 * Provide the vcs workspace branch checkout operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_branch_checkout(UmiVcsWorkspace *workspace, const char *name)
{
    uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || name == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_BRANCH_CHECKOUT, name, &id);
    status = umi_vcs_repository_branch_checkout(workspace->repository, name);
    return finish_mutation(workspace, id, status, "Branch checked out");
}
/*
 * Provide the vcs workspace branch delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_branch_delete(UmiVcsWorkspace *workspace, const char *name, int force)
{
    uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || name == NULL) return UMI_STATUS_UNAVAILABLE;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_BRANCH_DELETE, name, &id);
    status = umi_vcs_repository_branch_delete(workspace->repository, name, force);
    return finish_mutation(workspace, id, status, "Branch deleted");
}
/*
 * Provide the vcs workspace open diff operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_open_diff(UmiVcsWorkspace *workspace, const char *path, int staged)
{
    char *output; uint64_t id = 0U; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!UMI_VCS_WORKSPACE_READY(workspace) || path == NULL) return UMI_STATUS_UNAVAILABLE;
    output = calloc(UMI_PROCESS_OUTPUT_CAPACITY, sizeof(char));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (output == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (void)umi_vcs_operation_log_begin(workspace->operations, UMI_VCS_OPERATION_DIFF, path, &id);
    status = umi_vcs_repository_diff(workspace->repository, path, staged, output, UMI_PROCESS_OUTPUT_CAPACITY);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_vcs_diff_document_parse(workspace->diff, path, staged, output);
    free(output);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) workspace->revision += 1U;
    (void)umi_vcs_operation_log_finish(workspace->operations, id, status,
        status == UMI_STATUS_OK ? "Diff loaded" : "Diff load failed");
    return status;
}
/*
 * Provide the vcs workspace changes operation used by this module and its client
 * applications.
 */
const UmiVcsChangeList *umi_vcs_workspace_changes(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->changes : NULL; }
/*
 * Provide the vcs workspace conflicts operation used by this module and its client
 * applications.
 */
const UmiVcsConflictList *umi_vcs_workspace_conflicts(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->conflicts : NULL; }
/*
 * Provide the vcs workspace history operation used by this module and its client
 * applications.
 */
const UmiVcsHistory *umi_vcs_workspace_history(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->history : NULL; }
/*
 * Provide the vcs workspace branches operation used by this module and its client
 * applications.
 */
const UmiVcsBranchList *umi_vcs_workspace_branches(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->branches : NULL; }
/*
 * Provide the vcs workspace remotes operation used by this module and its client
 * applications.
 */
const UmiVcsRemoteList *umi_vcs_workspace_remotes(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->remotes : NULL; }
/*
 * Provide the vcs workspace tags operation used by this module and its client
 * applications.
 */
const UmiVcsTagList *umi_vcs_workspace_tags(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->tags : NULL; }
/*
 * Provide the vcs workspace diff operation used by this module and its client
 * applications.
 */
const UmiVcsDiffDocument *umi_vcs_workspace_diff(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->diff : NULL; }
/*
 * Provide the vcs workspace operations operation used by this module and its client
 * applications.
 */
const UmiVcsOperationLog *umi_vcs_workspace_operations(const UmiVcsWorkspace *workspace)
{ return workspace != NULL ? workspace->operations : NULL; }
