/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_history_cursor.c
 *
 * PURPOSE:
 *   Validate track deterministic pagination through large repository histories.
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
#include "umicom/vcs/advanced/history_cursor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedHistoryCursor value;
    umi_vcs_advanced_history_cursor_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_history_cursor_validate(&value) != UMI_STATUS_OK) return 2;
    umi_vcs_advanced_history_cursor_advance(&value, 25U, 1);
    /* Apply this branch only when its contract condition is satisfied. */
    if (value.offset != 25U || value.has_more == 0) return 3;
    return 0;
}
