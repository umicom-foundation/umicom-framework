/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/merge_plan.h
 *
 * PURPOSE:
 *   Plan merge strategy, fast-forward policy and safety gates before repository mutation.
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
#ifndef UMICOM_VCS_ADVANCED_MERGE_PLAN_H
#define UMICOM_VCS_ADVANCED_MERGE_PLAN_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named vcs advanced merge strategy values accepted by this public contract.
 */
typedef enum UmiVcsAdvancedMergeStrategy { UMI_VCS_MERGE_ORTHOGONAL=0, UMI_VCS_MERGE_OURS=1, UMI_VCS_MERGE_SUBTREE=2 } UmiVcsAdvancedMergeStrategy;
/**
 * List the named vcs advanced fast forward values accepted by this public contract.
 */
typedef enum UmiVcsAdvancedFastForward { UMI_VCS_FF_ALLOW=0, UMI_VCS_FF_ONLY=1, UMI_VCS_FF_NO=2 } UmiVcsAdvancedFastForward;
/**
 * Represent the vcs advanced merge plan data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedMergePlan {
    uint32_t struct_size; uint32_t api_version;
    char source[UMI_VCS_ADVANCED_LABEL_CAPACITY]; char target[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char merge_base[UMI_VCS_ADVANCED_OID_CAPACITY]; char message[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    UmiVcsAdvancedMergeStrategy strategy; UmiVcsAdvancedFastForward fast_forward;
    int squash; int no_commit; int allow_dirty; UmiVcsSafetyLevel safety;
} UmiVcsAdvancedMergePlan;
/**
 * Initialise vcs advanced merge plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_merge_plan_init(UmiVcsAdvancedMergePlan *plan);
/**
 * Copy vcs advanced merge plan into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_vcs_advanced_merge_plan_set(UmiVcsAdvancedMergePlan *plan,const char *source,const char *target);
/**
 * Provide the vcs advanced merge plan ready operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_merge_plan_ready(const UmiVcsAdvancedMergePlan *plan,int worktree_clean,int conflicts);
#ifdef __cplusplus
}
#endif
#endif
