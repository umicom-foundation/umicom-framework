/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_control/test_health_contract.c
 *
 * PURPOSE:
 *   Regression coverage for repository health contract semantics.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "umicom/repository/health.h"

int main(void)
{
    UmiRepositoryLockPlan plan = {0}; UmiRepositoryHealth h;
    plan.count = 2U; plan.stage_count = 2U;
    assert(umi_repository_health_from_plan(&plan, &h) == UMI_STATUS_OK);
    assert(h.healthy && h.dependency_count == 2U);
    return 0;
}
