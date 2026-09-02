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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/stash_plan.h"
#include <string.h>
/*
 * Initialise vcs advanced stash plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_stash_plan_init(UmiVcsAdvancedStashPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL) return;
    (void)memset(plan, 0, sizeof(*plan));
    plan->struct_size=(uint32_t)sizeof(*plan); plan->api_version=UMI_VCS_ADVANCED_API_VERSION;
    plan->safety=UMI_VCS_SAFETY_REVIEW;
}
/*
 * Check that vcs advanced stash plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_stash_plan_validate(const UmiVcsAdvancedStashPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan==NULL || plan->struct_size<sizeof(*plan) || plan->api_version!=UMI_VCS_ADVANCED_API_VERSION ||
        plan->action>UMI_VCS_STASH_BRANCH) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->action!=UMI_VCS_STASH_PUSH && !umi_vcs_advanced_text_present(plan->stash_ref))
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->action==UMI_VCS_STASH_BRANCH && !umi_vcs_advanced_text_present(plan->branch_name))
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs advanced stash plan push operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_stash_plan_push(UmiVcsAdvancedStashPlan *plan, const char *message,
                                            int include_untracked, int keep_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->action=UMI_VCS_STASH_PUSH; plan->include_untracked=include_untracked!=0; plan->keep_index=keep_index!=0;
    return umi_vcs_advanced_copy_text(plan->message,sizeof(plan->message),message);
}
/*
 * Perform vcs advanced stash plan through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_vcs_advanced_stash_plan_apply(UmiVcsAdvancedStashPlan *plan, const char *stash_ref,
                                             int pop, int reinstate_index)
{
    UmiStatus s;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    plan->action=pop?UMI_VCS_STASH_POP:UMI_VCS_STASH_APPLY; plan->reinstate_index=reinstate_index!=0;
    s=umi_vcs_advanced_copy_text(plan->stash_ref,sizeof(plan->stash_ref),stash_ref);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (s!=UMI_STATUS_OK) return s;
    return umi_vcs_advanced_stash_plan_validate(plan);
}
