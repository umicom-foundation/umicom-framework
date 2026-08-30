/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_health_boundary.c
 *
 * PURPOSE:
 *   Regression coverage for repository health boundary semantics.
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
#include "umicom/repository/health.h"

int main(void)
{
    UmiRepositoryLockPlan plan = {0}; UmiRepositoryHealth h;
    plan.count = 2U; plan.missing_count = 1U;
    assert(umi_repository_health_from_plan(&plan, &h) == UMI_STATUS_OK);
    assert(!h.healthy && h.error_count == 1U);
    return 0;
}
