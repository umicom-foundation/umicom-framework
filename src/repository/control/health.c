/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/health.c
 *
 * PURPOSE:
 *   Summarize repository dependency and lock-plan health.
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
#include "umicom/repository/health.h"

#include <string.h>

/*
 * Provide the repository health from plan operation used by this module and its client
 * applications.
 */
UmiStatus umi_repository_health_from_plan(
    const UmiRepositoryLockPlan *plan,
    UmiRepositoryHealth *out_health)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_health == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_health, 0, sizeof(*out_health));
    out_health->dependency_count = plan->count;
    out_health->stage_count = plan->stage_count;
    out_health->missing_count = plan->missing_count;
    out_health->error_count = plan->missing_count;
    out_health->healthy = plan->missing_count == 0U;
    return UMI_STATUS_OK;
}
