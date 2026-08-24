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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/status_summary.h"

#include <string.h>

void umi_repository_status_summary_init(UmiRepositoryStatusSummary *summary)
{
    if (summary == NULL) return;
    (void)memset(summary, 0, sizeof(*summary));
    summary->state = UMI_REPOSITORY_MAINTENANCE_CLEAN;
}

UmiStatus umi_repository_status_summary_refresh(UmiRepositoryStatusSummary *summary)
{
    if (summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_repository_index_status_validate(&summary->index) != UMI_STATUS_OK ||
        umi_repository_submodule_status_validate(&summary->submodules) != UMI_STATUS_OK ||
        umi_repository_remote_status_validate(&summary->remotes) != UMI_STATUS_OK) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_ERROR;
        return UMI_STATUS_INVALID_STATE;
    }
    summary->total_changes = umi_repository_worktree_status_change_count(&summary->worktree) +
        summary->index.staged_paths;
    if (summary->submodules.missing > 0U) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_INCOMPLETE;
    } else if (summary->branch.ahead > 0U && summary->branch.behind > 0U) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_DIVERGED;
    } else if (summary->total_changes > 0U || summary->submodules.dirty > 0U ||
               summary->submodules.head_mismatch > 0U) {
        summary->state = UMI_REPOSITORY_MAINTENANCE_DIRTY;
    } else {
        summary->state = UMI_REPOSITORY_MAINTENANCE_CLEAN;
    }
    return UMI_STATUS_OK;
}
