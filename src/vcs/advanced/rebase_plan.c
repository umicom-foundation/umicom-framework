/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/rebase_plan.c
 *
 * PURPOSE:
 *   Implement interactive rebase todo sequencing and validation.
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

#include "umicom/vcs/advanced/rebase_plan.h"

#include <string.h>

void umi_vcs_advanced_rebase_plan_init(UmiVcsAdvancedRebasePlan *plan)
{
    if (plan == NULL) {
        return;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_VCS_ADVANCED_API_VERSION;
    plan->safety = UMI_VCS_SAFETY_REVIEW;
}

UmiStatus umi_vcs_advanced_rebase_plan_set_base(UmiVcsAdvancedRebasePlan *plan,
                                                 const char *upstream,
                                                 const char *onto)
{
    UmiStatus status;
    if (plan == NULL || !umi_vcs_advanced_text_present(upstream)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_vcs_advanced_copy_text(
        plan->upstream, sizeof(plan->upstream), upstream);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_vcs_advanced_copy_text(plan->onto, sizeof(plan->onto), onto);
}

UmiStatus umi_vcs_advanced_rebase_plan_add(UmiVcsAdvancedRebasePlan *plan,
                                            const UmiVcsAdvancedRebaseStep *step)
{
    if (plan == NULL ||
        umi_vcs_advanced_rebase_step_validate(step) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->step_count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    plan->steps[plan->step_count] = *step;
    plan->steps[plan->step_count].sequence = (uint32_t)plan->step_count;
    plan->step_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_rebase_plan_move(UmiVcsAdvancedRebasePlan *plan,
                                             size_t from,
                                             size_t to)
{
    UmiVcsAdvancedRebaseStep temp;
    size_t i;

    if (plan == NULL || from >= plan->step_count || to >= plan->step_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (from == to) {
        return UMI_STATUS_OK;
    }

    temp = plan->steps[from];
    if (from < to) {
        for (i = from; i < to; ++i) {
            plan->steps[i] = plan->steps[i + 1U];
        }
    } else {
        for (i = from; i > to; --i) {
            plan->steps[i] = plan->steps[i - 1U];
        }
    }
    plan->steps[to] = temp;
    for (i = 0U; i < plan->step_count; ++i) {
        plan->steps[i].sequence = (uint32_t)i;
    }
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_rebase_plan_ready(const UmiVcsAdvancedRebasePlan *plan)
{
    return plan != NULL &&
           umi_vcs_advanced_text_present(plan->upstream) &&
           plan->step_count > 0U;
}
