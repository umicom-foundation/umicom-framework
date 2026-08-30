/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_autonomy_policy.c
 *
 * PURPOSE:
 *   Regression coverage for define the permitted autonomy ceiling and mandatory review requirements for engineering actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/autonomy_policy.h"
int main(void){ UmiHelixAutonomyPolicy r; umi_helix_autonomy_policy_init(&r,"autonomy_policy"); if(umi_helix_autonomy_policy_validate(&r)!=UMI_STATUS_OK){ return 1; } r.threshold=0.8; r.blocking=true; double s=umi_helix_autonomy_policy_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
