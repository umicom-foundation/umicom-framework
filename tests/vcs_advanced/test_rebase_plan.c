/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_rebase_plan.c
 *
 * PURPOSE:
 *   Validate maintain an interactive rebase todo list with stable sequencing and validation.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/rebase_plan.h"
int main(void){UmiVcsAdvancedRebasePlan p;UmiVcsAdvancedRebaseStep s;
    umi_vcs_advanced_rebase_plan_init(&p);
    umi_vcs_advanced_rebase_step_init(&s);
if(umi_vcs_advanced_rebase_plan_set_base(&p,"main","main")!=UMI_STATUS_OK)return 1;
if(umi_vcs_advanced_rebase_step_set(&s,"a","one",0U,UMI_VCS_REBASE_PICK)!=UMI_STATUS_OK)return 2;
if(umi_vcs_advanced_rebase_plan_add(&p,&s)!=UMI_STATUS_OK)return 3;
    if (!umi_vcs_advanced_rebase_plan_ready(&p))return 4;
    return 0;
}
