/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/merge_plan.c
 *
 * PURPOSE:
 *   Implement merge strategy, fast-forward policy and safety gates.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/merge_plan.h"

#include <string.h>

void umi_vcs_advanced_merge_plan_init(UmiVcsAdvancedMergePlan *plan)
{
    if (plan == NULL) {
        return;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_VCS_ADVANCED_API_VERSION;
    plan->strategy = UMI_VCS_MERGE_ORTHOGONAL;
    plan->fast_forward = UMI_VCS_FF_ALLOW;
    plan->safety = UMI_VCS_SAFETY_REVIEW;
}

UmiStatus umi_vcs_advanced_merge_plan_set(UmiVcsAdvancedMergePlan *plan,
                                           const char *source,
                                           const char *target)
{
    UmiStatus status;
    if (plan == NULL ||
        !umi_vcs_advanced_text_present(source) ||
        !umi_vcs_advanced_text_present(target) ||
        umi_vcs_advanced_text_equal(source, target)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_vcs_advanced_copy_text(
        plan->source, sizeof(plan->source), source);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_vcs_advanced_copy_text(
        plan->target, sizeof(plan->target), target);
}

int umi_vcs_advanced_merge_plan_ready(const UmiVcsAdvancedMergePlan *plan,
                                       int worktree_clean,
                                       int conflicts)
{
    return plan != NULL &&
           umi_vcs_advanced_text_present(plan->source) &&
           umi_vcs_advanced_text_present(plan->target) &&
           !conflicts &&
           (worktree_clean || plan->allow_dirty);
}
