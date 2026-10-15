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
#include "../value_archive/construction_cases.h"
#include "umicom/vcs/advanced/partial_stage_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Exercise rejected edits and borrowed text through the public setter. */
static int UmiVcsAdvancedPartialStagePlanTextEditCases(void)
{
    UmiVcsAdvancedPartialStagePlan *value = calloc(1U, sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_partial_stage_plan_set_root(value, "prefix-value") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    char oversized[sizeof(value->repository_root) + 1U];
    memset(oversized, 'x', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_partial_stage_plan_set_root(value, oversized) != UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_partial_stage_plan_set_root(value, value->repository_root + 7U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->repository_root, "value") == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_partial_stage_plan_set_root(NULL, "value") == UMI_STATUS_INVALID_ARGUMENT);
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiVcsAdvancedPartialStagePlanTextEditCases() != 0) return 1;

    UmiVcsAdvancedPartialStagePlan p; UmiVcsAdvancedStagingHunk h;
    umi_vcs_advanced_partial_stage_plan_init(&p); umi_vcs_advanced_staging_hunk_init(&h);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_partial_stage_plan_set_root(&p, ".") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_staging_hunk_set(&h, "a.c", 1U, 1U, 1U, 2U, "@@") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_partial_stage_plan_add(&p, &h) != UMI_STATUS_OK) return 3;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_vcs_advanced_partial_stage_plan_ready(&p)) return 4;
    return 0;
}
