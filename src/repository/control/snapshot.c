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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/snapshot.h"

#include <string.h>

void umi_repository_snapshot_init(UmiRepositoryControlSnapshot *snapshot)
{
    if (snapshot == NULL) return;
    (void)memset(snapshot, 0, sizeof(*snapshot));
    snapshot->state = UMI_REPOSITORY_CONTROL_IDLE;
    snapshot->last_status = UMI_STATUS_OK;
    snapshot->revision = 1U;
}

UmiStatus umi_repository_snapshot_from_plan(
    UmiRepositoryControlState state,
    UmiStatus last_status,
    const UmiRepositoryLockPlan *plan,
    const UmiRepositoryIssueList *issues,
    UmiRepositoryControlSnapshot *out_snapshot)
{
    UmiStatus status;
    if (plan == NULL || issues == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    umi_repository_snapshot_init(out_snapshot);
    status = umi_repository_health_from_plan(plan, &out_snapshot->health);
    if (status != UMI_STATUS_OK) return status;
    out_snapshot->state = state;
    out_snapshot->last_status = last_status;
    out_snapshot->issue_count = issues->count;
    out_snapshot->dry_run = plan->dry_run;
    out_snapshot->revision = plan->revision;
    return UMI_STATUS_OK;
}
