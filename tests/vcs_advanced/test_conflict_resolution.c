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
#include "umicom/vcs/advanced/conflict_resolution.h"
int main(void){UmiVcsAdvancedConflictResolution r;UmiVcsAdvancedConflictHunk h;
    umi_vcs_advanced_conflict_resolution_init(&r);
    umi_vcs_advanced_conflict_hunk_init(&h);
if(umi_vcs_advanced_conflict_resolution_set_path(&r,"a.c")!=UMI_STATUS_OK)return 1;
if(umi_vcs_advanced_conflict_hunk_set(&h,1U,1U,1U,1U,1U,1U,"x")!=UMI_STATUS_OK)return 2;
if(umi_vcs_advanced_conflict_resolution_add(&r,&h)!=UMI_STATUS_OK)return 3;
if(umi_vcs_advanced_conflict_resolution_choose(&r,0U,UMI_VCS_CONFLICT_USE_OURS)!=UMI_STATUS_OK)return 4;
if(!umi_vcs_advanced_conflict_resolution_complete(&r))return 5;
    return 0;
}
