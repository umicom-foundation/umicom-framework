/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_partial_stage_plan.c
 *
 * PURPOSE:
 *   Validate build reviewable partial-stage plans from selected diff hunks.
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
#include "umicom/vcs/advanced/partial_stage_plan.h"
int main(void)
{
    UmiVcsAdvancedPartialStagePlan p; UmiVcsAdvancedStagingHunk h;
    umi_vcs_advanced_partial_stage_plan_init(&p); umi_vcs_advanced_staging_hunk_init(&h);
    if (umi_vcs_advanced_partial_stage_plan_set_root(&p, ".") != UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_staging_hunk_set(&h, "a.c", 1U, 1U, 1U, 2U, "@@") != UMI_STATUS_OK) return 2;
    if (umi_vcs_advanced_partial_stage_plan_add(&p, &h) != UMI_STATUS_OK) return 3;
    if (!umi_vcs_advanced_partial_stage_plan_ready(&p)) return 4;
    return 0;
}
