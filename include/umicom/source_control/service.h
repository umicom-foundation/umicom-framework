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

/**
 * Represent the source control service data shared with callers of this public contract.
 */
typedef struct UmiSourceControlService UmiSourceControlService;
/**
 * Represent the source control service snapshot data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise source control service from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_source_control_service_create(UmiSourceControlService **out_owner);
/**
 * Release or reset state held by source control service so the same storage can be reused
 * safely.
 */
void umi_source_control_service_destroy(UmiSourceControlService *owner);
/**
 * Provide the source control service snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_source_control_service_snapshot(const UmiSourceControlService *owner, UmiSourceControlServiceSnapshot *out_snapshot);
/**
 * Provide the source control service repository operation used by this module and its
 * client applications.
 */
UmiSourceControlRepositoryRegistry *umi_source_control_service_repository(UmiSourceControlService *owner);
/**
 * Provide the source control service change operation used by this module and its client
 * applications.
 */
UmiSourceControlChangeRegistry *umi_source_control_service_change(UmiSourceControlService *owner);
/**
 * Copy source control service change into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiSourceControlChangeSetRegistry *umi_source_control_service_change_set(UmiSourceControlService *owner);
/**
 * Provide the source control service staging operation used by this module and its client
 * applications.
 */
UmiSourceControlStagingRegistry *umi_source_control_service_staging(UmiSourceControlService *owner);
/**
 * Provide the source control service commit operation used by this module and its client
 * applications.
 */
UmiSourceControlCommitRegistry *umi_source_control_service_commit(UmiSourceControlService *owner);
/**
 * Provide the source control service branch operation used by this module and its client
 * applications.
 */
UmiSourceControlBranchRegistry *umi_source_control_service_branch(UmiSourceControlService *owner);
/**
 * Provide the source control service tag operation used by this module and its client
 * applications.
 */
UmiSourceControlTagRegistry *umi_source_control_service_tag(UmiSourceControlService *owner);
/**
 * Provide the source control service remote operation used by this module and its client
 * applications.
 */
UmiSourceControlRemoteRegistry *umi_source_control_service_remote(UmiSourceControlService *owner);
/**
 * Provide the source control service diff session operation used by this module and its
 * client applications.
 */
UmiSourceControlDiffSessionRegistry *umi_source_control_service_diff_session(UmiSourceControlService *owner);
/**
 * Provide the source control service operation operation used by this module and its
 * client applications.
 */
UmiSourceControlOperationRegistry *umi_source_control_service_operation(UmiSourceControlService *owner);
/**
 * Provide the source control service history entry operation used by this module and its
 * client applications.
 */
UmiSourceControlHistoryEntryRegistry *umi_source_control_service_history_entry(UmiSourceControlService *owner);
/**
 * Provide the source control service open workspace operation used by this module and its
 * client applications.
 */
UmiStatus umi_source_control_service_open_workspace(UmiSourceControlService *owner, const char *root);
/**
 * Provide the source control service close workspace operation used by this module and its
 * client applications.
 */
void umi_source_control_service_close_workspace(UmiSourceControlService *owner);
/**
 * Provide the source control service workspace operation used by this module and its
 * client applications.
 */
UmiVcsWorkspace *umi_source_control_service_workspace(UmiSourceControlService *owner);
/**
 * Provide the source control service workspace const operation used by this module and its
 * client applications.
 */
const UmiVcsWorkspace *umi_source_control_service_workspace_const(const UmiSourceControlService *owner);
/**
 * Provide the source control service workspace coordinator operation used by this module
 * and its client applications.
 */
UmiVcsWorkspaceCoordinator *umi_source_control_service_workspace_coordinator(
    UmiSourceControlService *owner
);
/**
 * Provide the source control service workspace coordinator const operation used by this
 * module and its client applications.
 */
const UmiVcsWorkspaceCoordinator *
umi_source_control_service_workspace_coordinator_const(
    const UmiSourceControlService *owner
);

#ifdef __cplusplus
}
#endif
#endif
