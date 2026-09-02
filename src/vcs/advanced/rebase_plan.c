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

/*
 * Initialise vcs advanced rebase plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_rebase_plan_init(UmiVcsAdvancedRebasePlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) {
        return;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_VCS_ADVANCED_API_VERSION;
    plan->safety = UMI_VCS_SAFETY_REVIEW;
}

/*
 * Provide the vcs advanced rebase plan set base operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_rebase_plan_set_base(UmiVcsAdvancedRebasePlan *plan,
                                                 const char *upstream,
                                                 const char *onto)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || !umi_vcs_advanced_text_present(upstream)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_vcs_advanced_copy_text(
        plan->upstream, sizeof(plan->upstream), upstream);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }
    return umi_vcs_advanced_copy_text(plan->onto, sizeof(plan->onto), onto);
}

/*
 * Add vcs advanced rebase plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_rebase_plan_add(UmiVcsAdvancedRebasePlan *plan,
                                            const UmiVcsAdvancedRebaseStep *step)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL ||
        umi_vcs_advanced_rebase_step_validate(step) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->step_count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    plan->steps[plan->step_count] = *step;
    plan->steps[plan->step_count].sequence = (uint32_t)plan->step_count;
    plan->step_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced rebase plan move operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_rebase_plan_move(UmiVcsAdvancedRebasePlan *plan,
                                             size_t from,
                                             size_t to)
{
    UmiVcsAdvancedRebaseStep temp;
    size_t i;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || from >= plan->step_count || to >= plan->step_count) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (from == to) {
        return UMI_STATUS_OK;
    }

    temp = plan->steps[from];
    /* Apply this branch only when its contract condition is satisfied. */
    if (from < to) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (i = from; i < to; ++i) {
            plan->steps[i] = plan->steps[i + 1U];
        }
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        /* Visit each bounded item once so every record receives the same rule. */
        for (i = from; i > to; --i) {
            plan->steps[i] = plan->steps[i - 1U];
        }
    }
    plan->steps[to] = temp;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->step_count; ++i) {
        plan->steps[i].sequence = (uint32_t)i;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced rebase plan ready operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_rebase_plan_ready(const UmiVcsAdvancedRebasePlan *plan)
{
    return plan != NULL &&
           umi_vcs_advanced_text_present(plan->upstream) &&
           plan->step_count > 0U;
}
