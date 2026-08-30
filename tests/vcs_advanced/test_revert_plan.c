/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_revert_plan.c
 *
 * PURPOSE:
 *   Validate maintain ordered, reviewable revert plans without rewriting existing history.
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
#include "umicom/vcs/advanced/revert_plan.h"
int main(void){UmiVcsAdvancedRevertPlan p;
    umi_vcs_advanced_revert_plan_init(&p);
if(umi_vcs_advanced_revert_plan_add(&p,"deadbeef")!=UMI_STATUS_OK)return 1;
if(!umi_vcs_advanced_revert_plan_ready(&p))return 2;
    return 0;
}
