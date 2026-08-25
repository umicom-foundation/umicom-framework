/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/stash_plan.c
 *
 * PURPOSE:
 *   Plan stash push/apply/pop/drop/branch operations with explicit conflict and index intent.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/stash_plan.h"
#include <string.h>
void umi_vcs_advanced_stash_plan_init(UmiVcsAdvancedStashPlan *plan)
{
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size=(uint32_t)sizeof(*plan); plan->api_version=UMI_VCS_ADVANCED_API_VERSION;
    plan->safety=UMI_VCS_SAFETY_REVIEW;
}
UmiStatus umi_vcs_advanced_stash_plan_validate(const UmiVcsAdvancedStashPlan *plan)
{
    if (plan==NULL || plan->struct_size<sizeof(*plan) || plan->api_version!=UMI_VCS_ADVANCED_API_VERSION ||
        plan->action>UMI_VCS_STASH_BRANCH) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->action!=UMI_VCS_STASH_PUSH && !umi_vcs_advanced_text_present(plan->stash_ref))
        return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->action==UMI_VCS_STASH_BRANCH && !umi_vcs_advanced_text_present(plan->branch_name))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus umi_vcs_advanced_stash_plan_push(UmiVcsAdvancedStashPlan *plan, const char *message,
                                            int include_untracked, int keep_index)
{
    if (plan==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->action=UMI_VCS_STASH_PUSH; plan->include_untracked=include_untracked!=0; plan->keep_index=keep_index!=0;
    return umi_vcs_advanced_copy_text(plan->message,sizeof(plan->message),message);
}
UmiStatus umi_vcs_advanced_stash_plan_apply(UmiVcsAdvancedStashPlan *plan, const char *stash_ref,
                                             int pop, int reinstate_index)
{
    UmiStatus s;
    if (plan==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->action=pop?UMI_VCS_STASH_POP:UMI_VCS_STASH_APPLY; plan->reinstate_index=reinstate_index!=0;
    s=umi_vcs_advanced_copy_text(plan->stash_ref,sizeof(plan->stash_ref),stash_ref);
    if (s!=UMI_STATUS_OK) return s;
    return umi_vcs_advanced_stash_plan_validate(plan);
}
