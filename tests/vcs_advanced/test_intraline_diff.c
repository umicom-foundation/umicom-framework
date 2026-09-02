/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_intraline_diff.c
 *
 * PURPOSE:
 *   Validate locate changed spans within a pair of modified lines using common prefix/suffix boundaries.
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
#include "umicom/vcs/advanced/intraline_diff.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedIntralineDiff d;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_intraline_diff_compute("abcXdef","abcYdef",&d)!=UMI_STATUS_OK)return 1;
/* Apply this branch only when its contract condition is satisfied. */
if(d.common_prefix!=3U||d.common_suffix!=3U||d.left_change_count!=1U||d.right_change_count!=1U)return 2;
    return 0;
}
