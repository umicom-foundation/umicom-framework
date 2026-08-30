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

void umi_vcs_advanced_revert_plan_init(UmiVcsAdvancedRevertPlan *plan)
{
    if (plan == NULL) {
        return;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_VCS_ADVANCED_API_VERSION;
    plan->safety = UMI_VCS_SAFETY_SAFE;
}

UmiStatus umi_vcs_advanced_revert_plan_add(UmiVcsAdvancedRevertPlan *plan,
                                            const char *oid)
{
    UmiStatus status;
    if (plan == NULL || !umi_vcs_advanced_text_present(oid)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->commit_count >= UMI_VCS_ADVANCED_SMALL_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    status = umi_vcs_advanced_copy_text(
        plan->commits[plan->commit_count], sizeof(plan->commits[0]), oid);
    if (status == UMI_STATUS_OK) {
        plan->commit_count += 1U;
    }
    return status;
}

int umi_vcs_advanced_revert_plan_ready(const UmiVcsAdvancedRevertPlan *plan)
{
    return plan != NULL &&
           plan->api_version == UMI_VCS_ADVANCED_API_VERSION &&
           plan->commit_count > 0U;
}
