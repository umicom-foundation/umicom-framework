/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/patch_apply_plan.h
 *
 * PURPOSE:
 *   Represent reviewable patch application options before any worktree mutation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_PATCH_APPLY_PLAN_H
#define UMICOM_VCS_ADVANCED_PATCH_APPLY_PLAN_H

#include "umicom/vcs/advanced/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedPatchApplyPlan {
    uint32_t struct_size;
    uint32_t api_version;
    char repository_root[UMI_VCS_ADVANCED_PATH_CAPACITY];
    char patch_path[UMI_VCS_ADVANCED_PATH_CAPACITY];
    size_t strip_components;
    int reverse;
    int check_only;
    int three_way;
    int index_only;
    UmiVcsSafetyLevel safety;
} UmiVcsAdvancedPatchApplyPlan;

void umi_vcs_advanced_patch_apply_plan_init(UmiVcsAdvancedPatchApplyPlan *value);
UmiStatus umi_vcs_advanced_patch_apply_plan_validate(const UmiVcsAdvancedPatchApplyPlan *value);
UmiStatus umi_vcs_advanced_patch_apply_plan_set(UmiVcsAdvancedPatchApplyPlan *value,
                                                  const char *repository_root,
                                                  const char *patch_path,
                                                  size_t strip_components);

#ifdef __cplusplus
}
#endif

#endif
