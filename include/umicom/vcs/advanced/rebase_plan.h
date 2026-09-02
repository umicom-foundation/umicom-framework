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
/**
 * Represent the vcs advanced rebase plan data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedRebasePlan {
    uint32_t struct_size; uint32_t api_version;
    char upstream[UMI_VCS_ADVANCED_LABEL_CAPACITY]; char onto[UMI_VCS_ADVANCED_LABEL_CAPACITY];
    UmiVcsAdvancedRebaseStep steps[UMI_VCS_ADVANCED_SMALL_CAPACITY]; size_t step_count;
    int autosquash; int rebase_merges; int update_refs; UmiVcsSafetyLevel safety;
} UmiVcsAdvancedRebasePlan;
/**
 * Initialise vcs advanced rebase plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_rebase_plan_init(UmiVcsAdvancedRebasePlan *plan);
/**
 * Provide the vcs advanced rebase plan set base operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_rebase_plan_set_base(UmiVcsAdvancedRebasePlan *plan,const char *upstream,const char *onto);
/**
 * Add vcs advanced rebase plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_rebase_plan_add(UmiVcsAdvancedRebasePlan *plan,const UmiVcsAdvancedRebaseStep *step);
/**
 * Provide the vcs advanced rebase plan move operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_advanced_rebase_plan_move(UmiVcsAdvancedRebasePlan *plan,size_t from,size_t to);
/**
 * Provide the vcs advanced rebase plan ready operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_rebase_plan_ready(const UmiVcsAdvancedRebasePlan *plan);
#ifdef __cplusplus
}
#endif
#endif
