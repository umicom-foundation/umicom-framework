/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_repository_state.c
 *
 * PURPOSE:
 *   Validate aggregate branch/upstream and in-progress git operation state.
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
#include "umicom/vcs/advanced/repository_state.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedRepositoryState value;
    umi_vcs_advanced_repository_state_init(&value);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_repository_state_validate(&value) == UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_copy_text(value.head_oid, sizeof(value.head_oid), "abc") != UMI_STATUS_OK) return 2;
    value.ahead = 1U; value.behind = 2U;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_repository_state_validate(&value) != UMI_STATUS_OK) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_repository_state_diverged(&value)) return 4;
    return 0;
}
