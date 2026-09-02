/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/cherry_pick_plan.c
 *
 * PURPOSE:
 *   Implement ordered cherry-pick plans with mainline and no-commit controls.
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

#include "umicom/vcs/advanced/cherry_pick_plan.h"

#include <string.h>

/*
 * Initialise vcs advanced cherry pick plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_cherry_pick_plan_init(UmiVcsAdvancedCherryPickPlan *plan)
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
 * Add vcs advanced cherry pick plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_cherry_pick_plan_add(UmiVcsAdvancedCherryPickPlan *plan,
                                                 const char *oid)
{
    size_t i;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || !umi_vcs_advanced_text_present(oid)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->commit_count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_vcs_advanced_text_equal(plan->commits[i], oid)) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->commit_count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_vcs_advanced_copy_text(
        plan->commits[plan->commit_count], sizeof(plan->commits[0]), oid);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        plan->commit_count += 1U;
    }
    return status;
}

/*
 * Provide the vcs advanced cherry pick plan ready operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_cherry_pick_plan_ready(
    const UmiVcsAdvancedCherryPickPlan *plan)
{
    return plan != NULL &&
           plan->struct_size >= sizeof(*plan) &&
           plan->api_version == UMI_VCS_ADVANCED_API_VERSION &&
           plan->commit_count > 0U;
}
