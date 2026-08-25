/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_helix/test_plan_validator.c
 *
 * PURPOSE:
 *   Regression coverage for evaluate a plan graph against scope, risk, task and approval constraints before execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/plan_validator.h"
int main(void){ UmiHelixPlanGraph g; umi_helix_plan_graph_init(&g); (void)umi_helix_plan_graph_add(&g,"compile"); UmiHelixPlanValidation v; if(umi_helix_plan_validator_evaluate(&g,true,0.2,0.5,true,&v)!=UMI_STATUS_OK||v.decision!=UMI_HELIX_DECISION_ALLOW){ return 1; } if(umi_helix_plan_validator_evaluate(&g,false,0.8,0.5,false,&v)!=UMI_STATUS_OK||v.blockers<2U){ return 2; } return 0; }
