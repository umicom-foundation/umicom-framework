/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_commit_selection.c
 *
 * PURPOSE:
 *   Validate maintain ordered, de-duplicated commit selections for history operations.
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
#include "umicom/vcs/advanced/commit_selection.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedCommitSelection s;
    umi_vcs_advanced_commit_selection_init(&s);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_commit_selection_add(&s,"a")!=UMI_STATUS_OK)return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_vcs_advanced_commit_selection_contains(&s,"a"))return 2;
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(umi_vcs_advanced_commit_selection_remove(&s,"a")!=UMI_STATUS_OK||s.count!=0U)return 3;
    return 0;
}
