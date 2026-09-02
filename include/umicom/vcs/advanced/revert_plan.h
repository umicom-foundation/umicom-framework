/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/revert_plan.h
 *
 * PURPOSE:
 *   Maintain ordered, reviewable revert plans without rewriting existing history.
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
#ifndef UMICOM_VCS_ADVANCED_REVERT_PLAN_H
#define UMICOM_VCS_ADVANCED_REVERT_PLAN_H
#include "umicom/vcs/advanced/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced revert plan data shared with callers of this public contract.
 */
typedef struct UmiVcsAdvancedRevertPlan {
    uint32_t struct_size; uint32_t api_version;
    char commits[UMI_VCS_ADVANCED_SMALL_CAPACITY][UMI_VCS_ADVANCED_OID_CAPACITY];
    size_t commit_count; uint32_t mainline_parent; int no_commit; int edit_message;
    UmiVcsSafetyLevel safety;
} UmiVcsAdvancedRevertPlan;
/**
 * Initialise vcs advanced revert plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_revert_plan_init(UmiVcsAdvancedRevertPlan *plan);
/**
 * Add vcs advanced revert plan only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_vcs_advanced_revert_plan_add(UmiVcsAdvancedRevertPlan *plan,const char *oid);
/**
 * Provide the vcs advanced revert plan ready operation used by this module and its client
 * applications.
 */
int umi_vcs_advanced_revert_plan_ready(const UmiVcsAdvancedRevertPlan *plan);
#ifdef __cplusplus
}
#endif
#endif
