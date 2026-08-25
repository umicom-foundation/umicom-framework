/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_cherry_pick_plan.c
 *
 * PURPOSE:
 *   Validate maintain ordered cherry-pick plans with mainline and no-commit controls.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/cherry_pick_plan.h"
int main(void){UmiVcsAdvancedCherryPickPlan p;
    umi_vcs_advanced_cherry_pick_plan_init(&p);
if(umi_vcs_advanced_cherry_pick_plan_add(&p,"a")!=UMI_STATUS_OK)return 1;
if(umi_vcs_advanced_cherry_pick_plan_add(&p,"b")!=UMI_STATUS_OK)return 2;
if(!umi_vcs_advanced_cherry_pick_plan_ready(&p))return 3;
    return 0;
}
