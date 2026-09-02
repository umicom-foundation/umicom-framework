/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_compare_session.c
 *
 * PURPOSE:
 *   Validate manage professional two-way/three-way comparison session identity and synchronized view state.
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
#include "umicom/vcs/advanced/compare_session.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiVcsAdvancedCompareSession s;UmiVcsAdvancedCompareSide l,r;
    umi_vcs_advanced_compare_session_init(&s);
    umi_vcs_advanced_compare_side_init(&l);
    umi_vcs_advanced_compare_side_init(&r);
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_compare_side_set(&l,"a","HEAD","L",1)!=UMI_STATUS_OK)return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_compare_side_set(&r,"a","","R",0)!=UMI_STATUS_OK)return 2;
/* Preserve the original failure result so the caller can respond to the correct cause. */
if(umi_vcs_advanced_compare_session_open(&s,"cmp1",&l,&r,NULL)!=UMI_STATUS_OK)return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (s.mode!=UMI_VCS_COMPARE_TWO_WAY)return 4;
    return 0;
}
