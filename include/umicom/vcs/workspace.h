/*-----------------------------------------------------------------------------
 * Umicom Framework operational source-control workspace.
 * Framework owns repository state, operations and presentation-ready models;
 * products such as Umicom Studio only orchestrate and render them.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_WORKSPACE_H
#define UMICOM_VCS_WORKSPACE_H
#include "umicom/base/status.h"
#include "umicom/vcs/branch_list.h"
#include "umicom/vcs/change.h"
#include "umicom/vcs/conflict.h"
#include "umicom/vcs/diff_document.h"
#include "umicom/vcs/history.h"
#include "umicom/vcs/operation_log.h"
#include "umicom/vcs/provider.h"
#include "umicom/vcs/remote.h"
#include "umicom/vcs/tag.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsWorkspace UmiVcsWorkspace;
UmiStatus umi_vcs_workspace_create(const char *root, const UmiVcsProvider *provider, int available, UmiVcsWorkspace **out_workspace);
UmiStatus umi_vcs_workspace_create_git(const char *root, UmiVcsWorkspace **out_workspace);
void umi_vcs_workspace_destroy(UmiVcsWorkspace *workspace);
UmiStatus umi_vcs_workspace_refresh(UmiVcsWorkspace *workspace, size_t history_limit);
UmiStatus umi_vcs_workspace_snapshot(const UmiVcsWorkspace *workspace, UmiVcsWorkspaceSnapshot *out_snapshot);
UmiStatus umi_vcs_workspace_stage(UmiVcsWorkspace *workspace, const char *path);
UmiStatus umi_vcs_workspace_unstage(UmiVcsWorkspace *workspace, const char *path);
UmiStatus umi_vcs_workspace_stage_all(UmiVcsWorkspace *workspace);
UmiStatus umi_vcs_workspace_unstage_all(UmiVcsWorkspace *workspace);
UmiStatus umi_vcs_workspace_discard(UmiVcsWorkspace *workspace, const char *path);
UmiStatus umi_vcs_workspace_commit(UmiVcsWorkspace *workspace, const char *message, char *out_commit_id, size_t capacity);
UmiStatus umi_vcs_workspace_fetch(UmiVcsWorkspace *workspace);
UmiStatus umi_vcs_workspace_pull(UmiVcsWorkspace *workspace);
UmiStatus umi_vcs_workspace_push(UmiVcsWorkspace *workspace);
UmiStatus umi_vcs_workspace_branch_create(UmiVcsWorkspace *workspace, const char *name, int checkout);
UmiStatus umi_vcs_workspace_branch_checkout(UmiVcsWorkspace *workspace, const char *name);
UmiStatus umi_vcs_workspace_branch_delete(UmiVcsWorkspace *workspace, const char *name, int force);
UmiStatus umi_vcs_workspace_open_diff(UmiVcsWorkspace *workspace, const char *path, int staged);
const UmiVcsChangeList *umi_vcs_workspace_changes(const UmiVcsWorkspace *workspace);
const UmiVcsConflictList *umi_vcs_workspace_conflicts(const UmiVcsWorkspace *workspace);
const UmiVcsHistory *umi_vcs_workspace_history(const UmiVcsWorkspace *workspace);
const UmiVcsBranchList *umi_vcs_workspace_branches(const UmiVcsWorkspace *workspace);
const UmiVcsRemoteList *umi_vcs_workspace_remotes(const UmiVcsWorkspace *workspace);
const UmiVcsTagList *umi_vcs_workspace_tags(const UmiVcsWorkspace *workspace);
const UmiVcsDiffDocument *umi_vcs_workspace_diff(const UmiVcsWorkspace *workspace);
const UmiVcsOperationLog *umi_vcs_workspace_operations(const UmiVcsWorkspace *workspace);
#ifdef __cplusplus
}
#endif
#endif
