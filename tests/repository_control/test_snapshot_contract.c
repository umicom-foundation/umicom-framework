/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_snapshot_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository snapshot contract semantics.
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
#include <assert.h>
#include "umicom/repository/snapshot.h"

int main(void)
{
    UmiRepositoryLockPlan plan = {0}; UmiRepositoryIssueList issues = {0};
    UmiRepositoryControlSnapshot s;
    plan.count = 1U; plan.stage_count = 1U; plan.revision = 3U;
    assert(umi_repository_snapshot_from_plan(UMI_REPOSITORY_CONTROL_PLANNED,
     UMI_STATUS_OK, &plan, &issues, &s) == UMI_STATUS_OK);
    assert(s.state == UMI_REPOSITORY_CONTROL_PLANNED && s.revision == 3U);
    return 0;
}
