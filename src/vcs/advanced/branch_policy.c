/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/branch_policy.c
 *
 * PURPOSE:
 *   Enforce protected-branch and destructive-operation policy independently of frontend toolkits.
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
#include "umicom/vcs/advanced/branch_policy.h"
#include <string.h>
/*
 * Initialise vcs advanced branch policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_branch_policy_init(UmiVcsAdvancedBranchPolicy *policy)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->api_version = UMI_VCS_ADVANCED_API_VERSION;
    policy->require_merged_before_delete = 1;
}
/*
 * Provide the vcs advanced branch policy add protected operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_branch_policy_add_protected(UmiVcsAdvancedBranchPolicy *policy,
                                                        const char *branch)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !umi_vcs_advanced_text_present(branch)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (policy->protected_count >= 16U) return UMI_STATUS_CAPACITY_EXCEEDED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_branch_policy_is_protected(policy, branch)) return UMI_STATUS_ALREADY_EXISTS;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_copy_text(policy->protected_names[policy->protected_count],
        sizeof(policy->protected_names[0]), branch) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    policy->protected_count += 1U;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs advanced branch policy is protected operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_branch_policy_is_protected(const UmiVcsAdvancedBranchPolicy *policy,
                                                 const char *branch)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || branch == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < policy->protected_count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_vcs_advanced_text_equal(policy->protected_names[i], branch)) return 1;
    return 0;
}
/*
 * Provide the vcs advanced branch policy check delete operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_branch_policy_check_delete(const UmiVcsAdvancedBranchPolicy *policy,
                                                       const char *branch,
                                                       int current_branch,
                                                       int fully_merged,
                                                       int force)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (policy == NULL || !umi_vcs_advanced_text_present(branch)) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_branch_policy_is_protected(policy, branch)) return UMI_STATUS_PERMISSION_DENIED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (current_branch && !policy->allow_delete_current) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (force && !policy->allow_force_delete) return UMI_STATUS_PERMISSION_DENIED;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (policy->require_merged_before_delete && !fully_merged && !force) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
