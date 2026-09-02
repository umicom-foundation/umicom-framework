/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/status_summary.c
 *
 * PURPOSE:
 *   Implement deterministic repository health-state projection.
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
#include "umicom/repository/status_summary.h"

#include <string.h>

/*
 * Initialise repository status summary from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_status_summary_init(UmiRepositoryStatusSummary *summary)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL) return;
    (void)memset(summary, 0, sizeof(*summary));
    summary->state = UMI_REPOSITORY_MAINTENANCE_CLEAN;
}

/*
 * Provide the repository status summary refresh operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_status_summary_refresh(UmiRepositoryStatusSummary *summary)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (umi_repository_index_status_validate(&summary->index) != UMI_STATUS_OK ||
        umi_repository_submodule_status_validate(&summary->submodules) != UMI_STATUS_OK ||
        umi_repository_remote_status_validate(&summary->remotes) != UMI_STATUS_OK) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_ERROR;
        return UMI_STATUS_INVALID_STATE;
    }
    summary->total_changes = umi_repository_worktree_status_change_count(&summary->worktree) +
        summary->index.staged_paths;
    /* Apply this branch only when its contract condition is satisfied. */
    if (summary->submodules.missing > 0U) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_INCOMPLETE;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (summary->branch.ahead > 0U && summary->branch.behind > 0U) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_DIVERGED;
    } else /* Apply this branch only when its contract condition is satisfied. */ if (summary->total_changes > 0U || summary->submodules.dirty > 0U ||
               summary->submodules.head_mismatch > 0U) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_DIRTY;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        summary->state = UMI_REPOSITORY_MAINTENANCE_CLEAN;
    }
    return UMI_STATUS_OK;
}
