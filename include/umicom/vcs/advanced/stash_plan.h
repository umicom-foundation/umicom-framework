/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/stash_plan.h
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
#ifndef UMICOM_VCS_ADVANCED_STASH_PLAN_H
#define UMICOM_VCS_ADVANCED_STASH_PLAN_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named vcs advanced stash action values accepted by this public contract.
 */
typedef enum UmiVcsAdvancedStashAction {
    UMI_VCS_STASH_PUSH = 0, UMI_VCS_STASH_APPLY = 1, UMI_VCS_STASH_POP = 2,
    UMI_VCS_STASH_DROP = 3, UMI_VCS_STASH_BRANCH = 4
} UmiVcsAdvancedStashAction;
/**
 * Represent the vcs advanced stash plan data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedStashPlan {
    uint32_t struct_size; uint32_t api_version;
    UmiVcsAdvancedStashAction action;
    char stash_ref[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char message[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    char branch_name[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    int include_untracked; int keep_index; int reinstate_index;
    UmiVcsSafetyLevel safety;
} UmiVcsAdvancedStashPlan;
/**
 * Initialise vcs advanced stash plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_stash_plan_init(UmiVcsAdvancedStashPlan *plan);
/**
 * Check that vcs advanced stash plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_vcs_advanced_stash_plan_validate(const UmiVcsAdvancedStashPlan *plan);
/**
 * Provide the vcs advanced stash plan push operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_stash_plan_push(UmiVcsAdvancedStashPlan *plan, const char *message,
                                            int include_untracked, int keep_index);
/**
 * Perform vcs advanced stash plan through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_vcs_advanced_stash_plan_apply(UmiVcsAdvancedStashPlan *plan, const char *stash_ref,
                                             int pop, int reinstate_index);
#ifdef __cplusplus
}
#endif
#endif
