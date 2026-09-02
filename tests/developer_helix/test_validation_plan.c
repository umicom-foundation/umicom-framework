/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_validation_plan.c
 *
 * PURPOSE:
 *   Regression coverage for collect validation gates and determine whether required evidence is complete.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/validation_plan.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiHelixValidationPlan p={0}; UmiHelixValidationGate g; umi_helix_validation_gate_init(&g,"tests"); g.threshold=1.0; g.blocking=true; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_validation_plan_add(&p,&g)!=UMI_STATUS_OK||umi_helix_validation_plan_ready(&p)){ return 1; } /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_helix_validation_plan_record(&p,0,true)!=UMI_STATUS_OK||!umi_helix_validation_plan_ready(&p)){ return 2; } return 0; }
