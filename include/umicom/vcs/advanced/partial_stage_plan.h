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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_PARTIAL_STAGE_PLAN_H
#define UMICOM_VCS_ADVANCED_PARTIAL_STAGE_PLAN_H
#include "umicom/vcs/advanced/staging_hunk.h"
#ifdef __cplusplus
extern "C" {
#endif
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
void umi_vcs_advanced_partial_stage_plan_init(UmiVcsAdvancedPartialStagePlan *plan);
UmiStatus umi_vcs_advanced_partial_stage_plan_set_root(UmiVcsAdvancedPartialStagePlan *plan,
                                                        const char *root);
UmiStatus umi_vcs_advanced_partial_stage_plan_add(UmiVcsAdvancedPartialStagePlan *plan,
                                                   const UmiVcsAdvancedStagingHunk *hunk);
UmiStatus umi_vcs_advanced_partial_stage_plan_select(UmiVcsAdvancedPartialStagePlan *plan,
                                                      size_t index,
                                                      int selected);
int umi_vcs_advanced_partial_stage_plan_ready(const UmiVcsAdvancedPartialStagePlan *plan);
#ifdef __cplusplus
}
#endif
#endif
