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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_MERGE_PLAN_H
#define UMICOM_VCS_ADVANCED_MERGE_PLAN_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiVcsAdvancedMergeStrategy { UMI_VCS_MERGE_ORTHOGONAL=0, UMI_VCS_MERGE_OURS=1, UMI_VCS_MERGE_SUBTREE=2 } UmiVcsAdvancedMergeStrategy;
typedef enum UmiVcsAdvancedFastForward { UMI_VCS_FF_ALLOW=0, UMI_VCS_FF_ONLY=1, UMI_VCS_FF_NO=2 } UmiVcsAdvancedFastForward;
typedef struct UmiVcsAdvancedMergePlan {
    uint32_t struct_size; uint32_t api_version;
    char source[UMI_VCS_ADVANCED_LABEL_CAPACITY]; char target[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    char merge_base[UMI_VCS_ADVANCED_OID_CAPACITY]; char message[UMI_VCS_ADVANCED_TEXT_CAPACITY];
    UmiVcsAdvancedMergeStrategy strategy; UmiVcsAdvancedFastForward fast_forward;
    int squash; int no_commit; int allow_dirty; UmiVcsSafetyLevel safety;
} UmiVcsAdvancedMergePlan;
void umi_vcs_advanced_merge_plan_init(UmiVcsAdvancedMergePlan *plan);
UmiStatus umi_vcs_advanced_merge_plan_set(UmiVcsAdvancedMergePlan *plan,const char *source,const char *target);
int umi_vcs_advanced_merge_plan_ready(const UmiVcsAdvancedMergePlan *plan,int worktree_clean,int conflicts);
#ifdef __cplusplus
}
#endif
#endif
