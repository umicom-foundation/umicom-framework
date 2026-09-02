/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/policy.c
 *
 * PURPOSE:
 *   Apply bounded Helix autonomy and promotion rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Source modification and release promotion are different authorities; allowing one does not automatically grant the other.
 */

#include "umicom/helix/policy.h"
#include <stddef.h>

/*
 * Provide the helix policy default operation used by this module and its client
 * applications.
 */
UmiHelixPolicy umi_helix_policy_default(void)
{
    UmiHelixPolicy policy = {1U, 1, 0, 0, 1, 0.85};
    return policy;
}

/*
 * Provide the helix policy can modify operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_policy_can_modify(const UmiHelixPolicy *policy,
                                      int dependency_change,
                                      int human_approved)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !policy->allow_source_changes) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (dependency_change && !policy->allow_dependency_changes) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_human_approval && !human_approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the helix policy can promote operation used by this module and its client
 * applications.
 */
UmiStatus umi_helix_policy_can_promote(const UmiHelixPolicy *policy,
                                       double fitness,
                                       int human_approved)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !policy->allow_release) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (fitness < policy->minimum_fitness) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->require_human_approval && !human_approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}
