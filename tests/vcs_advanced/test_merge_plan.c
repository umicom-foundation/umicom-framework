/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_merge_plan.c
 *
 * PURPOSE:
 *   Validate plan merge strategy, fast-forward policy and safety gates before repository mutation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/merge_plan.h"
int main(void){UmiVcsAdvancedMergePlan p;
    umi_vcs_advanced_merge_plan_init(&p);
if(umi_vcs_advanced_merge_plan_set(&p,"feature","main")!=UMI_STATUS_OK)return 1;
if(!umi_vcs_advanced_merge_plan_ready(&p,1,0))return 2;
    return 0;
}
