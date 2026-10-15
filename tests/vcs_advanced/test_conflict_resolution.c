/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_conflict_resolution.c
 *
 * PURPOSE:
 *   Validate track per-hunk conflict choices and completion state for three-way merge workflows.
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
#include "umicom/vcs/advanced/conflict_resolution.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* Exercise rejected edits and borrowed text through the public setter. */
static int UmiVcsAdvancedConflictResolutionTextEditCases(void)
{
    UmiVcsAdvancedConflictResolution *value = calloc(1U, sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_conflict_resolution_set_path(value, "prefix-value") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    char oversized[sizeof(value->path) + 1U];
    memset(oversized, 'x', sizeof(oversized) - 1U);
    oversized[sizeof(oversized) - 1U] = '\0';
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_conflict_resolution_set_path(value, oversized) != UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_conflict_resolution_set_path(value, value->path + 7U) == UMI_STATUS_OK);
    UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "value") == 0);
    UMI_CONSTRUCTION_REQUIRE(umi_vcs_advanced_conflict_resolution_set_path(NULL, "value") == UMI_STATUS_INVALID_ARGUMENT);
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiVcsAdvancedConflictResolutionTextEditCases() != 0) return 1;
UmiVcsAdvancedConflictResolution r;UmiVcsAdvancedConflictHunk h;
    umi_vcs_advanced_conflict_resolution_init(&r);
    umi_vcs_advanced_conflict_hunk_init(&h);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_conflict_resolution_set_path(&r,"a.c")!=UMI_STATUS_OK)return 1;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_conflict_hunk_set(&h,1U,1U,1U,1U,1U,1U,"x")!=UMI_STATUS_OK)return 2;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_conflict_resolution_add(&r,&h)!=UMI_STATUS_OK)return 3;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_conflict_resolution_choose(&r,0U,UMI_VCS_CONFLICT_USE_OURS)!=UMI_STATUS_OK)return 4;
/* Apply this branch only when its contract condition is satisfied. */
if(!umi_vcs_advanced_conflict_resolution_complete(&r))return 5;
    return 0;
}
