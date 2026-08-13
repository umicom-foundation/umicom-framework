/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/source_control/service.c
 *
 * PURPOSE:
 *   Implement the reusable source-control service aggregating repositories, changes, staging, commits, branches, tags, remotes and operations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#include "umicom/source_control/service.h"

#include <stdlib.h>
#include <string.h>

struct UmiSourceControlService {
    UmiSourceControlRepositoryRegistry *repository;
    UmiSourceControlChangeRegistry *change;
    UmiSourceControlChangeSetRegistry *change_set;
    UmiSourceControlStagingRegistry *staging;
    UmiSourceControlCommitRegistry *commit;
    UmiSourceControlBranchRegistry *branch;
    UmiSourceControlTagRegistry *tag;
    UmiSourceControlRemoteRegistry *remote;
    UmiSourceControlDiffSessionRegistry *diff_session;
    UmiSourceControlOperationRegistry *operation;
    UmiSourceControlHistoryEntryRegistry *history_entry;
    UmiVcsWorkspace *workspace;
    uint64_t revision;
};

UmiStatus umi_source_control_service_create(UmiSourceControlService **out_owner)
{
    UmiSourceControlService *owner; UmiStatus status = UMI_STATUS_OK;
    if (out_owner == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_owner = NULL; owner = (UmiSourceControlService *)calloc(1U,sizeof(*owner));
    if (owner == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    owner->revision = 1U;
    if (status == UMI_STATUS_OK) status = umi_source_control_repository_registry_create(&owner->repository);
    if (status == UMI_STATUS_OK) status = umi_source_control_change_registry_create(&owner->change);
    if (status == UMI_STATUS_OK) status = umi_source_control_change_set_registry_create(&owner->change_set);
    if (status == UMI_STATUS_OK) status = umi_source_control_staging_registry_create(&owner->staging);
    if (status == UMI_STATUS_OK) status = umi_source_control_commit_registry_create(&owner->commit);
    if (status == UMI_STATUS_OK) status = umi_source_control_branch_registry_create(&owner->branch);
    if (status == UMI_STATUS_OK) status = umi_source_control_tag_registry_create(&owner->tag);
    if (status == UMI_STATUS_OK) status = umi_source_control_remote_registry_create(&owner->remote);
    if (status == UMI_STATUS_OK) status = umi_source_control_diff_session_registry_create(&owner->diff_session);
    if (status == UMI_STATUS_OK) status = umi_source_control_operation_registry_create(&owner->operation);
    if (status == UMI_STATUS_OK) status = umi_source_control_history_entry_registry_create(&owner->history_entry);
    if (status != UMI_STATUS_OK) { umi_source_control_service_destroy(owner); return status; }
    *out_owner = owner; return UMI_STATUS_OK;
}

void umi_source_control_service_destroy(UmiSourceControlService *owner)
{
    if (owner == NULL) return;
    umi_vcs_workspace_destroy(owner->workspace);
    umi_source_control_history_entry_registry_destroy(owner->history_entry);
    umi_source_control_operation_registry_destroy(owner->operation);
    umi_source_control_diff_session_registry_destroy(owner->diff_session);
    umi_source_control_remote_registry_destroy(owner->remote);
    umi_source_control_tag_registry_destroy(owner->tag);
    umi_source_control_branch_registry_destroy(owner->branch);
    umi_source_control_commit_registry_destroy(owner->commit);
    umi_source_control_staging_registry_destroy(owner->staging);
    umi_source_control_change_set_registry_destroy(owner->change_set);
    umi_source_control_change_registry_destroy(owner->change);
    umi_source_control_repository_registry_destroy(owner->repository);
    free(owner);
}

UmiStatus umi_source_control_service_snapshot(const UmiSourceControlService *owner, UmiSourceControlServiceSnapshot *out_snapshot)
{
    if (owner == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_snapshot,0,sizeof(*out_snapshot));
    out_snapshot->struct_size=(uint32_t)sizeof(*out_snapshot); out_snapshot->api_version=2U;
    out_snapshot->revision=owner->revision;
    out_snapshot->repository_count = umi_source_control_repository_registry_count(owner->repository);
    out_snapshot->change_count = umi_source_control_change_registry_count(owner->change);
    out_snapshot->change_set_count = umi_source_control_change_set_registry_count(owner->change_set);
    out_snapshot->staging_count = umi_source_control_staging_registry_count(owner->staging);
    out_snapshot->commit_count = umi_source_control_commit_registry_count(owner->commit);
    out_snapshot->branch_count = umi_source_control_branch_registry_count(owner->branch);
    out_snapshot->tag_count = umi_source_control_tag_registry_count(owner->tag);
    out_snapshot->remote_count = umi_source_control_remote_registry_count(owner->remote);
    out_snapshot->diff_session_count = umi_source_control_diff_session_registry_count(owner->diff_session);
    out_snapshot->operation_count = umi_source_control_operation_registry_count(owner->operation);
    out_snapshot->history_entry_count = umi_source_control_history_entry_registry_count(owner->history_entry);
    out_snapshot->item_count = out_snapshot->repository_count + out_snapshot->change_count + out_snapshot->change_set_count + out_snapshot->staging_count + out_snapshot->commit_count + out_snapshot->branch_count + out_snapshot->tag_count + out_snapshot->remote_count + out_snapshot->diff_session_count + out_snapshot->operation_count + out_snapshot->history_entry_count;
    out_snapshot->workspace_open = owner->workspace != NULL;
    if (owner->workspace != NULL) {
        (void)umi_vcs_workspace_snapshot(owner->workspace, &out_snapshot->workspace);
    }
    return UMI_STATUS_OK;
}

UmiSourceControlRepositoryRegistry *umi_source_control_service_repository(UmiSourceControlService *owner) { return owner != NULL ? owner->repository : NULL; }
UmiSourceControlChangeRegistry *umi_source_control_service_change(UmiSourceControlService *owner) { return owner != NULL ? owner->change : NULL; }
UmiSourceControlChangeSetRegistry *umi_source_control_service_change_set(UmiSourceControlService *owner) { return owner != NULL ? owner->change_set : NULL; }
UmiSourceControlStagingRegistry *umi_source_control_service_staging(UmiSourceControlService *owner) { return owner != NULL ? owner->staging : NULL; }
UmiSourceControlCommitRegistry *umi_source_control_service_commit(UmiSourceControlService *owner) { return owner != NULL ? owner->commit : NULL; }
UmiSourceControlBranchRegistry *umi_source_control_service_branch(UmiSourceControlService *owner) { return owner != NULL ? owner->branch : NULL; }
UmiSourceControlTagRegistry *umi_source_control_service_tag(UmiSourceControlService *owner) { return owner != NULL ? owner->tag : NULL; }
UmiSourceControlRemoteRegistry *umi_source_control_service_remote(UmiSourceControlService *owner) { return owner != NULL ? owner->remote : NULL; }
UmiSourceControlDiffSessionRegistry *umi_source_control_service_diff_session(UmiSourceControlService *owner) { return owner != NULL ? owner->diff_session : NULL; }
UmiSourceControlOperationRegistry *umi_source_control_service_operation(UmiSourceControlService *owner) { return owner != NULL ? owner->operation : NULL; }
UmiSourceControlHistoryEntryRegistry *umi_source_control_service_history_entry(UmiSourceControlService *owner) { return owner != NULL ? owner->history_entry : NULL; }

UmiStatus umi_source_control_service_open_workspace(UmiSourceControlService *owner, const char *root)
{
    UmiVcsWorkspace *workspace = NULL;
    UmiStatus status;
    if (owner == NULL || root == NULL || root[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_workspace_create_git(root, &workspace);
    if (status != UMI_STATUS_OK) return status;
    {
        UmiVcsWorkspaceSnapshot snapshot;
        if (umi_vcs_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK &&
            snapshot.available) {
            /* Populate the Centre immediately; an unborn/partially configured
             * repository may still be opened even if its first refresh fails. */
            (void)umi_vcs_workspace_refresh(workspace, 50U);
        }
    }
    umi_vcs_workspace_destroy(owner->workspace);
    owner->workspace = workspace;
    owner->revision += 1U;
    return UMI_STATUS_OK;
}

void umi_source_control_service_close_workspace(UmiSourceControlService *owner)
{
    if (owner == NULL || owner->workspace == NULL) return;
    umi_vcs_workspace_destroy(owner->workspace);
    owner->workspace = NULL;
    owner->revision += 1U;
}

UmiVcsWorkspace *umi_source_control_service_workspace(UmiSourceControlService *owner)
{ return owner != NULL ? owner->workspace : NULL; }

const UmiVcsWorkspace *umi_source_control_service_workspace_const(const UmiSourceControlService *owner)
{ return owner != NULL ? owner->workspace : NULL; }
