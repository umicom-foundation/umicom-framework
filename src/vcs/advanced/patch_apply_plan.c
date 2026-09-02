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

/*
 * Initialise vcs advanced patch apply plan from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_patch_apply_plan_init(UmiVcsAdvancedPatchApplyPlan *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    (void)memset(value, 0, sizeof(*value));
    value->struct_size = (uint32_t)sizeof(*value);
    value->api_version = UMI_VCS_ADVANCED_API_VERSION;
    value->check_only = 1;
    value->safety = UMI_VCS_SAFETY_REVIEW;
}

/*
 * Check that vcs advanced patch apply plan satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_vcs_advanced_patch_apply_plan_validate(const UmiVcsAdvancedPatchApplyPlan *value)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL ||
        value->struct_size < sizeof(*value) ||
        value->api_version != UMI_VCS_ADVANCED_API_VERSION ||
        (!umi_vcs_advanced_text_present(value->repository_root) || !umi_vcs_advanced_text_present(value->patch_path))) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/*
 * Copy vcs advanced patch apply plan into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_vcs_advanced_patch_apply_plan_set(UmiVcsAdvancedPatchApplyPlan *value,
                                                  const char *repository_root,
                                                  const char *patch_path,
                                                  size_t strip_components)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_vcs_advanced_copy_text(value->repository_root, sizeof(value->repository_root), repository_root);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_vcs_advanced_copy_text(value->patch_path, sizeof(value->patch_path), patch_path);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->strip_components = strip_components;
    return umi_vcs_advanced_patch_apply_plan_validate(value);
}
