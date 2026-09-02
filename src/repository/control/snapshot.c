/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/snapshot.c
 *
 * PURPOSE:
 *   Publish immutable repository-control operational snapshots.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/snapshot.h"

#include <string.h>

/*
 * Initialise repository snapshot from caller-provided values so later operations receive a
 * known state.
 */
void umi_repository_snapshot_init(UmiRepositoryControlSnapshot *snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->state = UMI_REPOSITORY_CONTROL_IDLE;
    snapshot->last_status = UMI_STATUS_OK;
    snapshot->revision = 1U;
}

/*
 * Provide the repository snapshot from plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_snapshot_from_plan(
    UmiRepositoryControlState state,
    UmiStatus last_status,
    const UmiRepositoryLockPlan *plan,
    const UmiRepositoryIssueList *issues,
    UmiRepositoryControlSnapshot *out_snapshot)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || issues == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_repository_snapshot_init(out_snapshot);
    status = umi_repository_health_from_plan(plan, &out_snapshot->health);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->state = state;
    out_snapshot->last_status = last_status;
    out_snapshot->issue_count = issues->count;
    out_snapshot->dry_run = plan->dry_run;
    out_snapshot->revision = plan->revision;
    return UMI_STATUS_OK;
}
