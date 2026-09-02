/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/worktree_status.h
 *
 * PURPOSE:
 *   Represent working-tree modification, untracked and conflict counts.
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
#ifndef INCLUDE_UMICOM_REPOSITORY_WORKTREE_STATUS_H
#define INCLUDE_UMICOM_REPOSITORY_WORKTREE_STATUS_H
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the repository worktree status data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryWorktreeStatus {
    size_t modified;
    size_t deleted;
    size_t untracked;
    size_t conflicted;
} UmiRepositoryWorktreeStatus;

/**
 * Initialise repository worktree status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_worktree_status_init(UmiRepositoryWorktreeStatus *status);
/**
 * Return the number of records represented by repository worktree status change without
 * changing their state.
 */
size_t umi_repository_worktree_status_change_count(const UmiRepositoryWorktreeStatus *status);
/**
 * Provide the repository worktree status dirty operation used by this module and its
 * client applications.
 */
int umi_repository_worktree_status_dirty(const UmiRepositoryWorktreeStatus *status);

#ifdef __cplusplus
}
#endif
#endif
