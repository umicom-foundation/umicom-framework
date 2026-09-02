/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_diff_document.c
 *
 * PURPOSE:
 *   Validate describe one compared document and aggregate change statistics.
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
#include "umicom/vcs/advanced/diff_document.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedDiffDocument value;
    umi_vcs_advanced_diff_document_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_diff_document_validate(&value) == UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_diff_document_set_paths(&value, "left.c", "right.c", UMI_VCS_COMPARE_TWO_WAY) != UMI_STATUS_OK) return 2;
    value.added_lines = 2U; value.deleted_lines = 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_diff_document_change_count(&value) != 3U) return 3;
    return 0;
}
