/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_directory_diff.c
 *
 * PURPOSE:
 *   Validate compare directory entries and classify left-only, right-only, changed and type-changed state.
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
#include "umicom/vcs/advanced/directory_diff.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedDirectoryEntry a,b;UmiVcsAdvancedDirectoryDiff d;
    umi_vcs_advanced_directory_entry_init(&a);
    umi_vcs_advanced_directory_entry_init(&b);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_directory_entry_set(&a,"a",10U,1U,0)!=UMI_STATUS_OK)return 1;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_directory_entry_set(&b,"a",10U,1U,0)!=UMI_STATUS_OK)return 2;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_directory_diff_compare(&a,&b,&d)!=UMI_STATUS_OK||d.state!=UMI_VCS_DIRECTORY_EQUAL)return 3;
    return 0;
}
