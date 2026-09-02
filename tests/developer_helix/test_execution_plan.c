/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_execution_plan.c
 *
 * PURPOSE:
 *   Regression coverage for order executable steps using dependency edges and reject cyclic or unapproved plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/execution_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixExecutionPlan p={0}; UmiHelixExecutionStep a,b; umi_helix_execution_step_init(&a,"a"); a.progress=0.0; umi_helix_execution_step_init(&b,"b"); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_execution_plan_add(&p,&a)!=UMI_STATUS_OK||umi_helix_execution_plan_add(&p,&b)!=UMI_STATUS_OK||umi_helix_execution_plan_depend(&p,0,1)!=UMI_STATUS_OK){ return 1; } size_t o[2],n=0; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_execution_plan_order(&p,o,2,&n)!=UMI_STATUS_PERMISSION_DENIED){ return 2; } p.approved=true; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_execution_plan_order(&p,o,2,&n)!=UMI_STATUS_OK||o[0]!=0){ return 3; } return 0; }
