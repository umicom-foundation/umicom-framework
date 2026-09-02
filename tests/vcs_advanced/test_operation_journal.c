/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_operation_journal.c
 *
 * PURPOSE:
 *   Validate record bounded vcs operation evidence for recovery, review and audit projections.
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
#include "umicom/vcs/advanced/operation_journal.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedOperationJournal j;
    umi_vcs_advanced_operation_journal_init(&j);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_operation_journal_append(&j,UMI_VCS_ADVANCED_OPERATION_MERGE,UMI_VCS_ADVANCED_STATE_COMPLETED,"feature",1U,UMI_STATUS_OK)!=UMI_STATUS_OK)return 1;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(umi_vcs_advanced_operation_journal_latest(&j)==NULL)return 2;
    return 0;
}
