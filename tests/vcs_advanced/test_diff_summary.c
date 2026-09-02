/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_summary.c
 *
 * PURPOSE:
 *   Validate aggregate normalized diff-line statistics for source control and compare views.
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
#include "umicom/vcs/advanced/diff_summary.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedDiffSummary s;UmiVcsAdvancedDiffLine l;
    umi_vcs_advanced_diff_summary_init(&s);
    umi_vcs_advanced_diff_line_init(&l);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_diff_line_set(&l,UMI_VCS_DIFF_ADDED,0U,1U,"x")!=UMI_STATUS_OK)return 1;
    umi_vcs_advanced_diff_summary_add(&s,&l);
/* Apply this branch only when its contract condition is satisfied. */
if(umi_vcs_advanced_diff_summary_change_count(&s)!=1U||umi_vcs_advanced_diff_summary_change_percent(&s)!=100U)return 2;
    return 0;
}
