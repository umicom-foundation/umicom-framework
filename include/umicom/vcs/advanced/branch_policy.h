/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/branch_policy.h
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
#ifndef UMICOM_VCS_ADVANCED_BRANCH_POLICY_H
#define UMICOM_VCS_ADVANCED_BRANCH_POLICY_H
#include "umicom/vcs/advanced/branch_operation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced branch policy data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedBranchPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    char protected_names[16][UMI_VCS_ADVANCED_LABEL_CAPACITY];
    size_t protected_count;
    int allow_force_delete;
    int allow_delete_current;
    int require_merged_before_delete;
} UmiVcsAdvancedBranchPolicy;
/**
 * Initialise vcs advanced branch policy from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_branch_policy_init(UmiVcsAdvancedBranchPolicy *policy);
/**
 * Provide the vcs advanced branch policy add protected operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_branch_policy_add_protected(UmiVcsAdvancedBranchPolicy *policy,
                                                        const char *branch);
/**
 * Provide the vcs advanced branch policy is protected operation used by this module and
 * its client applications.
 */
int umi_vcs_advanced_branch_policy_is_protected(const UmiVcsAdvancedBranchPolicy *policy,
                                                 const char *branch);
/**
 * Provide the vcs advanced branch policy check delete operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_branch_policy_check_delete(const UmiVcsAdvancedBranchPolicy *policy,
                                                       const char *branch,
                                                       int current_branch,
                                                       int fully_merged,
                                                       int force);
#ifdef __cplusplus
}
#endif
#endif
