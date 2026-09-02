/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/doctor_policy.c
 *
 * PURPOSE:
 *   Implement conservative default repository doctor policy.
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
#include "umicom/repository/doctor_policy.h"

#include <string.h>

/*
 * Provide the repository doctor policy default operation used by this module and its
 * client applications.
 */
void umi_repository_doctor_policy_default(UmiRepositoryDoctorPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->require_initialised_submodules = 1;
    policy->require_matching_submodule_heads = 1;
}

/*
 * Check that repository doctor policy satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_repository_doctor_policy_validate(const UmiRepositoryDoctorPolicy *policy)
{
    return policy != NULL ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
