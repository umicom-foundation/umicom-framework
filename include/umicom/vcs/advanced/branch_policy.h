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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_BRANCH_POLICY_H
#define UMICOM_VCS_ADVANCED_BRANCH_POLICY_H
#include "umicom/vcs/advanced/branch_operation.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedBranchPolicy {
    uint32_t struct_size;
    uint32_t api_version;
    char protected_names[16][UMI_VCS_ADVANCED_LABEL_CAPACITY];
    size_t protected_count;
    int allow_force_delete;
    int allow_delete_current;
    int require_merged_before_delete;
} UmiVcsAdvancedBranchPolicy;
void umi_vcs_advanced_branch_policy_init(UmiVcsAdvancedBranchPolicy *policy);
UmiStatus umi_vcs_advanced_branch_policy_add_protected(UmiVcsAdvancedBranchPolicy *policy,
                                                        const char *branch);
int umi_vcs_advanced_branch_policy_is_protected(const UmiVcsAdvancedBranchPolicy *policy,
                                                 const char *branch);
UmiStatus umi_vcs_advanced_branch_policy_check_delete(const UmiVcsAdvancedBranchPolicy *policy,
                                                       const char *branch,
                                                       int current_branch,
                                                       int fully_merged,
                                                       int force);
#ifdef __cplusplus
}
#endif
#endif
