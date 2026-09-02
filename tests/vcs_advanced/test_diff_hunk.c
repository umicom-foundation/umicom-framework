/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_hunk.c
 *
 * PURPOSE:
 *   Validate describe normalized change blocks for navigation and partial operations.
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
#include "umicom/vcs/advanced/diff_hunk.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedDiffHunk value;
    umi_vcs_advanced_diff_hunk_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_diff_hunk_validate(&value) == UMI_STATUS_OK) return 1;
    value.old_start = 1U; value.old_count = 3U; value.new_start = 1U; value.new_count = 4U;
    umi_vcs_advanced_diff_hunk_set_counts(&value, 1U, 0U, 1U);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_diff_hunk_validate(&value) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_diff_hunk_change_count(&value) != 2U) return 3;
    return 0;
}
