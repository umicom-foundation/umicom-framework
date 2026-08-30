/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/helix/plan_validator.c
 *
 * PURPOSE:
 *   Evaluate a plan graph against scope, risk, task and approval constraints before execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/helix/plan_validator.h"
#include <string.h>
/* Combine graph, scope, risk and approval evidence into one conservative decision. */
UmiStatus umi_helix_plan_validator_evaluate(const UmiHelixPlanGraph *g,bool scope,double risk,double max_risk,bool approval,UmiHelixPlanValidation *out){ if(g==NULL||out==NULL||max_risk<0.0||max_risk>1.0){ return UMI_STATUS_INVALID_ARGUMENT; } memset(out,0,sizeof(*out)); out->graph_valid=!umi_helix_plan_graph_has_cycle(g); out->scope_valid=scope; out->risk_valid=umi_helix_clamp_score(risk)<=max_risk; out->approval_valid=approval; out->blockers=(out->graph_valid?0U:1U)+(scope?0U:1U)+(out->risk_valid?0U:1U)+(approval?0U:1U); out->decision=out->blockers==0U?UMI_HELIX_DECISION_ALLOW:UMI_HELIX_DECISION_REVIEW; return UMI_STATUS_OK; }
