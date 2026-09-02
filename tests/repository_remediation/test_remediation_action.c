/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/repository_remediation/test_remediation_action.c
 *
 * PURPOSE:
 *   Provide a focused contract smoke test for the remediation action capability.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/remediation_action.h"

/* Verify this module remains callable through its public Framework contract. */
int main(void)
{
    UmiRepositoryRemediationAction a; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_repository_remediation_action_set(&a,UMI_REMEDIATION_REVIEW_WORKTREE,UMI_REPOSITORY_REMEDIATION_DESTRUCTIVE,"x","y","z",1)!=UMI_STATUS_OK)return 1; return a.executable_automatically==0?0:1;
}
