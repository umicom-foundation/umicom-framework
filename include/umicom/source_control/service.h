/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/source_control/service.h
 *
 * PURPOSE:
 *   Define the reusable source-control service aggregating repositories, changes, staging, commits, branches, tags, remotes and operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This module uses a small, explicit C API and bounded storage.  The public
 * contract does not expose toolkit objects, C++ types, or private structures.
 */
#ifndef FRAMEWORK_INCLUDE_UMICOM_SOURCE_CONTROL_SERVICE_H
#define FRAMEWORK_INCLUDE_UMICOM_SOURCE_CONTROL_SERVICE_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/source_control/repository.h"
#include "umicom/source_control/change.h"
#include "umicom/source_control/change_set.h"
#include "umicom/source_control/staging.h"
#include "umicom/source_control/commit.h"
#include "umicom/source_control/branch.h"
#include "umicom/source_control/tag.h"
#include "umicom/source_control/remote.h"
#include "umicom/source_control/diff_session.h"
#include "umicom/source_control/operation.h"
#include "umicom/source_control/history_entry.h"
#include "umicom/vcs/workspace.h"
#include "umicom/vcs/workspace_coordinator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiSourceControlService UmiSourceControlService;
typedef struct UmiSourceControlServiceSnapshot {
    uint32_t struct_size;
    uint32_t api_version;
    uint64_t revision;
    size_t item_count;
    size_t repository_count;
    size_t change_count;
    size_t change_set_count;
    size_t staging_count;
    size_t commit_count;
    size_t branch_count;
    size_t tag_count;
    size_t remote_count;
    size_t diff_session_count;
    size_t operation_count;
    size_t history_entry_count;
    int workspace_open;
    int workspace_coordinator_open;
    UmiVcsWorkspaceSnapshot workspace;
    UmiVcsWorkspaceCoordinatorSnapshot workspace_coordinator;
} UmiSourceControlServiceSnapshot;

UmiStatus umi_source_control_service_create(UmiSourceControlService **out_owner);
void umi_source_control_service_destroy(UmiSourceControlService *owner);
UmiStatus umi_source_control_service_snapshot(const UmiSourceControlService *owner, UmiSourceControlServiceSnapshot *out_snapshot);
UmiSourceControlRepositoryRegistry *umi_source_control_service_repository(UmiSourceControlService *owner);
UmiSourceControlChangeRegistry *umi_source_control_service_change(UmiSourceControlService *owner);
UmiSourceControlChangeSetRegistry *umi_source_control_service_change_set(UmiSourceControlService *owner);
UmiSourceControlStagingRegistry *umi_source_control_service_staging(UmiSourceControlService *owner);
UmiSourceControlCommitRegistry *umi_source_control_service_commit(UmiSourceControlService *owner);
UmiSourceControlBranchRegistry *umi_source_control_service_branch(UmiSourceControlService *owner);
UmiSourceControlTagRegistry *umi_source_control_service_tag(UmiSourceControlService *owner);
UmiSourceControlRemoteRegistry *umi_source_control_service_remote(UmiSourceControlService *owner);
UmiSourceControlDiffSessionRegistry *umi_source_control_service_diff_session(UmiSourceControlService *owner);
UmiSourceControlOperationRegistry *umi_source_control_service_operation(UmiSourceControlService *owner);
UmiSourceControlHistoryEntryRegistry *umi_source_control_service_history_entry(UmiSourceControlService *owner);
UmiStatus umi_source_control_service_open_workspace(UmiSourceControlService *owner, const char *root);
void umi_source_control_service_close_workspace(UmiSourceControlService *owner);
UmiVcsWorkspace *umi_source_control_service_workspace(UmiSourceControlService *owner);
const UmiVcsWorkspace *umi_source_control_service_workspace_const(const UmiSourceControlService *owner);
UmiVcsWorkspaceCoordinator *umi_source_control_service_workspace_coordinator(
    UmiSourceControlService *owner
);
const UmiVcsWorkspaceCoordinator *
umi_source_control_service_workspace_coordinator_const(
    const UmiSourceControlService *owner
);

#ifdef __cplusplus
}
#endif
#endif
