/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_validation_plan.c
 *
 * PURPOSE:
 *   Regression coverage for collect validation gates and determine whether required evidence is complete.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/validation_plan.h"
int main(void){ UmiHelixValidationPlan p={0}; UmiHelixValidationGate g; umi_helix_validation_gate_init(&g,"tests"); g.threshold=1.0; g.blocking=true; if(umi_helix_validation_plan_add(&p,&g)!=UMI_STATUS_OK||umi_helix_validation_plan_ready(&p)){ return 1; } if(umi_helix_validation_plan_record(&p,0,true)!=UMI_STATUS_OK||!umi_helix_validation_plan_ready(&p)){ return 2; } return 0; }
