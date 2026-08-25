/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_patch_apply_plan.c
 *
 * PURPOSE:
 *   Validate represent reviewable patch application options before any worktree mutation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/patch_apply_plan.h"

int main(void)
{
    UmiVcsAdvancedPatchApplyPlan value;
    umi_vcs_advanced_patch_apply_plan_init(&value);
    if (umi_vcs_advanced_patch_apply_plan_validate(&value) == UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_patch_apply_plan_set(&value, ".", "change.patch", 1U) != UMI_STATUS_OK) return 2;
    if (!value.check_only) return 3;
    return 0;
}
