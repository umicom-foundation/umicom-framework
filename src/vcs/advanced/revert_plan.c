/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/revert_plan.c
 *
 * PURPOSE:
 *   Implement ordered, reviewable revert plans without rewriting existing history.
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

#include "umicom/vcs/advanced/revert_plan.h"

#include <string.h>

/*
 * Initialise vcs advanced revert plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_revert_plan_init(UmiVcsAdvancedRevertPlan *plan)
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
    plan->safety = UMI_VCS_SAFETY_SAFE;
}

/*
 * Add vcs advanced revert plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_revert_plan_add(UmiVcsAdvancedRevertPlan *plan,
                                            const char *oid)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || !umi_vcs_advanced_text_present(oid)) {
        return UMI_STATUS_INVALID_ARGUMENT;
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
 * Provide the vcs advanced revert plan ready operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_revert_plan_ready(const UmiVcsAdvancedRevertPlan *plan)
{
    return plan != NULL &&
           plan->api_version == UMI_VCS_ADVANCED_API_VERSION &&
           plan->commit_count > 0U;
}
