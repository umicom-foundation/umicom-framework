/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_reset_plan.c
 *
 * PURPOSE:
 *   Validate model soft/mixed/hard reset intent with destructive-operation classification.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/reset_plan.h"
int main(void){UmiVcsAdvancedResetPlan p;
    umi_vcs_advanced_reset_plan_init(&p);
if(umi_vcs_advanced_reset_plan_set(&p,UMI_VCS_RESET_HARD,"HEAD~1")!=UMI_STATUS_OK)return 1;
if(!umi_vcs_advanced_reset_plan_destructive(&p))return 2;
    return 0;
}
