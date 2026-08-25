/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/reset_plan.c
 *
 * PURPOSE:
 *   Implement soft, mixed and hard reset intent with destructive classification.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/vcs/advanced/reset_plan.h"

#include <string.h>

void umi_vcs_advanced_reset_plan_init(UmiVcsAdvancedResetPlan *plan)
{
    if (plan == NULL) {
        return;
    }
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size = (uint32_t)sizeof(*plan);
    plan->api_version = UMI_VCS_ADVANCED_API_VERSION;
    plan->mode = UMI_VCS_RESET_MIXED;
    plan->safety = UMI_VCS_SAFETY_REVIEW;
    plan->require_checkpoint = 1;
}

UmiStatus umi_vcs_advanced_reset_plan_set(UmiVcsAdvancedResetPlan *plan,
                                           UmiVcsAdvancedResetMode mode,
                                           const char *target)
{
    UmiStatus status;
    if (plan == NULL || mode > UMI_VCS_RESET_HARD ||
        !umi_vcs_advanced_text_present(target)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    plan->mode = mode;
    status = umi_vcs_advanced_copy_text(
        plan->target, sizeof(plan->target), target);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    plan->safety = mode == UMI_VCS_RESET_HARD
        ? UMI_VCS_SAFETY_DESTRUCTIVE
        : UMI_VCS_SAFETY_REVIEW;
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_reset_plan_destructive(const UmiVcsAdvancedResetPlan *plan)
{
    return plan != NULL &&
           plan->mode == UMI_VCS_RESET_HARD &&
           !plan->path_limited;
}
