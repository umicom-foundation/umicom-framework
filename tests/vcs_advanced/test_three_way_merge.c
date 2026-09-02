/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_three_way_merge.c
 *
 * PURPOSE:
 *   Validate perform conservative whole-text three-way merges with explicit conflict markers.
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
#include "umicom/vcs/advanced/three_way_merge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedThreeWayMergeResult r;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_three_way_merge_text("a","a","b",&r)!=UMI_STATUS_OK)return 1;
/* Use the stable identifier comparison to choose the matching record or policy. */
if(!r.clean||!umi_vcs_advanced_text_equal(r.text,"b"))return 2;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_three_way_merge_text("a","b","c",&r)!=UMI_STATUS_OK)return 3;
/* Apply this branch only when its contract condition is satisfied. */
if(r.clean||r.conflict_count!=1U)return 4;
    return 0;
}
