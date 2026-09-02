/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_compare_navigation.c
 *
 * PURPOSE:
 *   Validate track deterministic next/previous change navigation in comparison sessions.
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
#include "umicom/vcs/advanced/compare_navigation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedCompareNavigation value;
    umi_vcs_advanced_compare_navigation_init(&value);
    value.change_count = 3U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_compare_navigation_validate(&value) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_compare_navigation_next(&value) || value.current_index != 1U) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_compare_navigation_previous(&value) || value.current_index != 0U) return 4;
    return 0;
}
