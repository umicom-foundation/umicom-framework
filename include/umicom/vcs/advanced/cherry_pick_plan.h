/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/cherry_pick_plan.h
 *
 * PURPOSE:
 *   Maintain ordered cherry-pick plans with mainline and no-commit controls.
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
#ifndef UMICOM_VCS_ADVANCED_CHERRY_PICK_PLAN_H
#define UMICOM_VCS_ADVANCED_CHERRY_PICK_PLAN_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedCherryPickPlan {
    uint32_t struct_size; uint32_t api_version;
    char commits[UMI_VCS_ADVANCED_SMALL_CAPACITY][UMI_VCS_ADVANCED_OID_CAPACITY];
    size_t commit_count; uint32_t mainline_parent;
    int no_commit; int signoff; int allow_empty;
    UmiVcsSafetyLevel safety;
} UmiVcsAdvancedCherryPickPlan;
void umi_vcs_advanced_cherry_pick_plan_init(UmiVcsAdvancedCherryPickPlan *plan);
UmiStatus umi_vcs_advanced_cherry_pick_plan_add(UmiVcsAdvancedCherryPickPlan *plan,const char *oid);
int umi_vcs_advanced_cherry_pick_plan_ready(const UmiVcsAdvancedCherryPickPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
