/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_engineering_goal.c
 *
 * PURPOSE:
 *   Regression coverage for represent a measurable engineering outcome and progress evidence for a governed helix session.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/engineering_goal.h"
int main(void){ UmiHelixEngineeringGoal r; umi_helix_engineering_goal_init(&r,"engineering_goal"); if(umi_helix_engineering_goal_validate(&r)!=UMI_STATUS_OK){ return 1; } r.progress=0.75; double s=umi_helix_engineering_goal_score(&r); if(s<=0.0 || s>1.0){ return 2; } return 0; }
