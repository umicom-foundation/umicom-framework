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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/three_way_merge.h"
int main(void){UmiVcsAdvancedThreeWayMergeResult r;
if(umi_vcs_advanced_three_way_merge_text("a","a","b",&r)!=UMI_STATUS_OK)return 1;
if(!r.clean||!umi_vcs_advanced_text_equal(r.text,"b"))return 2;
if(umi_vcs_advanced_three_way_merge_text("a","b","c",&r)!=UMI_STATUS_OK)return 3;
if(r.clean||r.conflict_count!=1U)return 4;
    return 0;
}
