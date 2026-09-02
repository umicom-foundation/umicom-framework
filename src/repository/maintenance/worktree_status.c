/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/worktree_status.c
 *
 * PURPOSE:
 *   Implement worktree change counting and dirtiness predicates.
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
#include "umicom/repository/worktree_status.h"

#include <string.h>

/*
 * Initialise repository worktree status from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_worktree_status_init(UmiRepositoryWorktreeStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status != NULL) (void)memset(status, 0, sizeof(*status));
}

/*
 * Return the number of records represented by repository worktree status change without
 * changing their state.
 */
size_t umi_repository_worktree_status_change_count(const UmiRepositoryWorktreeStatus *status)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == NULL) return 0U;
    return status->modified + status->deleted + status->untracked + status->conflicted;
}

/*
 * Provide the repository worktree status dirty operation used by this module and its
 * client applications.
 */
int umi_repository_worktree_status_dirty(const UmiRepositoryWorktreeStatus *status)
{
    return umi_repository_worktree_status_change_count(status) > 0U;
}
