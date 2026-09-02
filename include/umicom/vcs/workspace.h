/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/workspace.h
 *
 * PURPOSE:
 *   Publish the public workspace contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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
/**
 * Represent the vcs workspace data shared with callers of this public contract.
 */
typedef struct UmiVcsWorkspace UmiVcsWorkspace;
/**
 * Initialise vcs workspace from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_vcs_workspace_create(const char *root, const UmiVcsProvider *provider, int available, UmiVcsWorkspace **out_workspace);
/**
 * Provide the vcs workspace create git operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_create_git(const char *root, UmiVcsWorkspace **out_workspace);
/**
 * Release or reset state held by vcs workspace so the same storage can be reused safely.
 */
void umi_vcs_workspace_destroy(UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace refresh operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_refresh(UmiVcsWorkspace *workspace, size_t history_limit);
/**
 * Provide the vcs workspace snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_snapshot(const UmiVcsWorkspace *workspace, UmiVcsWorkspaceSnapshot *out_snapshot);
/**
 * Provide the vcs workspace stage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_stage(UmiVcsWorkspace *workspace, const char *path);
/**
 * Provide the vcs workspace unstage operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_unstage(UmiVcsWorkspace *workspace, const char *path);
/**
 * Provide the vcs workspace stage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_stage_all(UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace unstage all operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_unstage_all(UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace discard operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_discard(UmiVcsWorkspace *workspace, const char *path);
/**
 * Provide the vcs workspace commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_commit(UmiVcsWorkspace *workspace, const char *message, char *out_commit_id, size_t capacity);
/**
 * Provide the vcs workspace fetch operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_fetch(UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace pull operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_pull(UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace push operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_push(UmiVcsWorkspace *workspace);
/**
 * Initialise vcs workspace branch from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_vcs_workspace_branch_create(UmiVcsWorkspace *workspace, const char *name, int checkout);
/**
 * Provide the vcs workspace branch checkout operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_branch_checkout(UmiVcsWorkspace *workspace, const char *name);
/**
 * Provide the vcs workspace branch delete operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_branch_delete(UmiVcsWorkspace *workspace, const char *name, int force);
/**
 * Provide the vcs workspace open diff operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_workspace_open_diff(UmiVcsWorkspace *workspace, const char *path, int staged);
/**
 * Provide the vcs workspace changes operation used by this module and its client
 * applications.
 */
const UmiVcsChangeList *umi_vcs_workspace_changes(const UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace conflicts operation used by this module and its client
 * applications.
 */
const UmiVcsConflictList *umi_vcs_workspace_conflicts(const UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace history operation used by this module and its client
 * applications.
 */
const UmiVcsHistory *umi_vcs_workspace_history(const UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace branches operation used by this module and its client
 * applications.
 */
const UmiVcsBranchList *umi_vcs_workspace_branches(const UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace remotes operation used by this module and its client
 * applications.
 */
const UmiVcsRemoteList *umi_vcs_workspace_remotes(const UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace tags operation used by this module and its client
 * applications.
 */
const UmiVcsTagList *umi_vcs_workspace_tags(const UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace diff operation used by this module and its client
 * applications.
 */
const UmiVcsDiffDocument *umi_vcs_workspace_diff(const UmiVcsWorkspace *workspace);
/**
 * Provide the vcs workspace operations operation used by this module and its client
 * applications.
 */
const UmiVcsOperationLog *umi_vcs_workspace_operations(const UmiVcsWorkspace *workspace);
#ifdef __cplusplus
}
#endif
#endif
