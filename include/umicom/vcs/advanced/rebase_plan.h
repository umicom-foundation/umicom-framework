/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/rebase_plan.h
 *
 * PURPOSE:
 *   Maintain an interactive rebase todo list with stable sequencing and validation.
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
#ifndef UMICOM_VCS_ADVANCED_REBASE_PLAN_H
#define UMICOM_VCS_ADVANCED_REBASE_PLAN_H
#include "umicom/vcs/advanced/rebase_step.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiVcsAdvancedRebasePlan {
    uint32_t struct_size; uint32_t api_version;
    char upstream[UMI_VCS_ADVANCED_LABEL_CAPACITY]; char onto[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    UmiVcsAdvancedRebaseStep steps[UMI_VCS_ADVANCED_SMALL_CAPACITY]; size_t step_count;
    int autosquash; int rebase_merges; int update_refs; UmiVcsSafetyLevel safety;
} UmiVcsAdvancedRebasePlan;
void umi_vcs_advanced_rebase_plan_init(UmiVcsAdvancedRebasePlan *plan);
UmiStatus umi_vcs_advanced_rebase_plan_set_base(UmiVcsAdvancedRebasePlan *plan,const char *upstream,const char *onto);
UmiStatus umi_vcs_advanced_rebase_plan_add(UmiVcsAdvancedRebasePlan *plan,const UmiVcsAdvancedRebaseStep *step);
UmiStatus umi_vcs_advanced_rebase_plan_move(UmiVcsAdvancedRebasePlan *plan,size_t from,size_t to);
int umi_vcs_advanced_rebase_plan_ready(const UmiVcsAdvancedRebasePlan *plan);
#ifdef __cplusplus
}
#endif
#endif
