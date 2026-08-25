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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/branch_policy.h"
#include <string.h>
void umi_vcs_advanced_branch_policy_init(UmiVcsAdvancedBranchPolicy *policy)
{
    if (policy == NULL) return;
    (void)memset(policy, 0, sizeof(*policy));
    policy->struct_size = (uint32_t)sizeof(*policy);
    policy->api_version = UMI_VCS_ADVANCED_API_VERSION;
    policy->require_merged_before_delete = 1;
}
UmiStatus umi_vcs_advanced_branch_policy_add_protected(UmiVcsAdvancedBranchPolicy *policy,
                                                        const char *branch)
{
    if (policy == NULL || !umi_vcs_advanced_text_present(branch)) return UMI_STATUS_INVALID_ARGUMENT;
    if (policy->protected_count >= 16U) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (umi_vcs_advanced_branch_policy_is_protected(policy, branch)) return UMI_STATUS_ALREADY_EXISTS;
    if (umi_vcs_advanced_copy_text(policy->protected_names[policy->protected_count],
        sizeof(policy->protected_names[0]), branch) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    policy->protected_count += 1U;
    return UMI_STATUS_OK;
}
int umi_vcs_advanced_branch_policy_is_protected(const UmiVcsAdvancedBranchPolicy *policy,
                                                 const char *branch)
{
    size_t i;
    if (policy == NULL || branch == NULL) return 0;
    for (i = 0U; i < policy->protected_count; ++i)
        if (umi_vcs_advanced_text_equal(policy->protected_names[i], branch)) return 1;
    return 0;
}
UmiStatus umi_vcs_advanced_branch_policy_check_delete(const UmiVcsAdvancedBranchPolicy *policy,
                                                       const char *branch,
                                                       int current_branch,
                                                       int fully_merged,
                                                       int force)
{
    if (policy == NULL || !umi_vcs_advanced_text_present(branch)) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_vcs_advanced_branch_policy_is_protected(policy, branch)) return UMI_STATUS_PERMISSION_DENIED;
    if (current_branch && !policy->allow_delete_current) return UMI_STATUS_INVALID_STATE;
    if (force && !policy->allow_force_delete) return UMI_STATUS_PERMISSION_DENIED;
    if (policy->require_merged_before_delete && !fully_merged && !force) return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
