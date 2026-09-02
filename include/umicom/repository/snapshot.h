/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/snapshot.h
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
#ifndef UMICOM_REPOSITORY_SNAPSHOT_H
#define UMICOM_REPOSITORY_SNAPSHOT_H
#include "umicom/repository/health.h"
#include "umicom/repository/issue.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository control snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryControlSnapshot {
    UmiRepositoryControlState state;
    UmiStatus last_status;
    UmiRepositoryHealth health;
    size_t issue_count;
    uint64_t revision;
    int dry_run;
} UmiRepositoryControlSnapshot;
/**
 * Initialise repository snapshot from caller-provided values so later operations receive a
 * known state.
 */
void umi_repository_snapshot_init(UmiRepositoryControlSnapshot *snapshot);
/**
 * Provide the repository snapshot from plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_snapshot_from_plan(
    UmiRepositoryControlState state,
    UmiStatus last_status,
    const UmiRepositoryLockPlan *plan,
    const UmiRepositoryIssueList *issues,
    UmiRepositoryControlSnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif
#endif
