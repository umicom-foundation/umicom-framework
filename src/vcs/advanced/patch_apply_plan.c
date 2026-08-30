/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/patch_apply_plan.c
 *
 * PURPOSE:
 *   Represent reviewable patch application options before any worktree mutation.
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
#include "umicom/vcs/advanced/patch_apply_plan.h"

#include <string.h>

void umi_vcs_advanced_patch_apply_plan_init(UmiVcsAdvancedPatchApplyPlan *value)
{
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->check_only = 1;
    value->safety = UMI_VCS_SAFETY_REVIEW;
}

UmiStatus umi_vcs_advanced_patch_apply_plan_validate(const UmiVcsAdvancedPatchApplyPlan *value)
{
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->repository_root) || !umi_vcs_advanced_text_present(value->patch_path))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_vcs_advanced_patch_apply_plan_set(UmiVcsAdvancedPatchApplyPlan *value,
                                                  const char *repository_root,
                                                  const char *patch_path,
                                                  size_t strip_components)
{
    UmiStatus status;
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->repository_root, sizeof(value->repository_root), repository_root);
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->patch_path, sizeof(value->patch_path), patch_path);
    if (status != UMI_STATUS_OK) return status;
    value->strip_components = strip_components;
    return umi_vcs_advanced_patch_apply_plan_validate(value);
}
