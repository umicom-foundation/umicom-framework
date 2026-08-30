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

/* BEGINNER NOTE:
 * Source modification and release promotion are different authorities; allowing one does not automatically grant the other.
 */

#include "umicom/helix/policy.h"
#include <stddef.h>

UmiHelixPolicy umi_helix_policy_default(void)
{
    UmiHelixPolicy policy = {1U, 1, 0, 0, 1, 0.85};
    return policy;
}

UmiStatus umi_helix_policy_can_modify(const UmiHelixPolicy *policy,
                                      int dependency_change,
                                      int human_approved)
{
    if (policy == NULL || !policy->allow_source_changes) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (dependency_change && !policy->allow_dependency_changes) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (policy->require_human_approval && !human_approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_helix_policy_can_promote(const UmiHelixPolicy *policy,
                                       double fitness,
                                       int human_approved)
{
    if (policy == NULL || !policy->allow_release) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    if (fitness < policy->minimum_fitness) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (policy->require_human_approval && !human_approved) {
        return UMI_STATUS_PERMISSION_DENIED;
    }
    return UMI_STATUS_OK;
}
