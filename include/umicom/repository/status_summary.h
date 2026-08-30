/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/status_summary.h
 *
 * PURPOSE:
 *   Compose branch, worktree, index, submodule and remote state into one Framework snapshot.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef INCLUDE_UMICOM_REPOSITORY_STATUS_SUMMARY_H
#define INCLUDE_UMICOM_REPOSITORY_STATUS_SUMMARY_H
#include "umicom/repository/maintenance_types.h"
#include "umicom/repository/branch_status.h"
#include "umicom/repository/worktree_status.h"
#include "umicom/repository/index_status.h"
#include "umicom/repository/submodule_status.h"
#include "umicom/repository/remote_status.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiRepositoryStatusSummary {
    UmiRepositoryBranchStatus branch;
    UmiRepositoryWorktreeStatus worktree;
    UmiRepositoryIndexStatus index;
    UmiRepositorySubmoduleStatus submodules;
    UmiRepositoryRemoteStatus remotes;
    UmiRepositoryMaintenanceState state;
    size_t total_changes;
} UmiRepositoryStatusSummary;

void umi_repository_status_summary_init(UmiRepositoryStatusSummary *summary);
UmiStatus umi_repository_status_summary_refresh(UmiRepositoryStatusSummary *summary);

#ifdef __cplusplus
}
#endif
#endif
