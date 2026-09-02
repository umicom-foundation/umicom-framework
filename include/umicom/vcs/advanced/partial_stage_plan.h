/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/partial_stage_plan.h
 *
 * PURPOSE:
 *   Build reviewable partial-stage plans from selected diff hunks.
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
#ifndef UMICOM_VCS_ADVANCED_PARTIAL_STAGE_PLAN_H
#define UMICOM_VCS_ADVANCED_PARTIAL_STAGE_PLAN_H
#include "umicom/vcs/advanced/staging_hunk.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the vcs advanced partial stage plan data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedPartialStagePlan {
    uint32_t struct_size;
    uint32_t api_version;
    char repository_root[UMI_VCS_ADVANCED_PATH_CAPACITY];
    UmiVcsAdvancedStagingHunk hunks[UMI_VCS_ADVANCED_LIST_CAPACITY];
    size_t hunk_count;
    size_t selected_count;
    int reverse;
    int check_only;
    uint64_t fingerprint;
} UmiVcsAdvancedPartialStagePlan;
/**
 * Initialise vcs advanced partial stage plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_vcs_advanced_partial_stage_plan_init(UmiVcsAdvancedPartialStagePlan *plan);
/**
 * Provide the vcs advanced partial stage plan set root operation used by this module and
 * its client applications.
 */
UmiStatus umi_vcs_advanced_partial_stage_plan_set_root(UmiVcsAdvancedPartialStagePlan *plan,
                                                        const char *root);
/**
 * Add vcs advanced partial stage plan only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_vcs_advanced_partial_stage_plan_add(UmiVcsAdvancedPartialStagePlan *plan,
                                                   const UmiVcsAdvancedStagingHunk *hunk);
/**
 * Provide the vcs advanced partial stage plan select operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_partial_stage_plan_select(UmiVcsAdvancedPartialStagePlan *plan,
                                                      size_t index,
                                                      int selected);
/**
 * Provide the vcs advanced partial stage plan ready operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_partial_stage_plan_ready(const UmiVcsAdvancedPartialStagePlan *plan);
#ifdef __cplusplus
}
#endif
#endif
