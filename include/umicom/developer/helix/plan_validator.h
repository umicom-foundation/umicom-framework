/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/plan_validator.h
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
#ifndef UMICOM_DEVELOPER_HELIX_PLAN_VALIDATOR_H
#define UMICOM_DEVELOPER_HELIX_PLAN_VALIDATOR_H
#include "umicom/developer/helix/plan_graph.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixPlanValidation { bool graph_valid; bool scope_valid; bool risk_valid; bool approval_valid; size_t blockers; UmiHelixDecision decision; } UmiHelixPlanValidation;
/* Evaluate independent pre-execution gates and reject cyclic plans. */
UmiStatus umi_helix_plan_validator_evaluate(const UmiHelixPlanGraph *graph,bool scope_valid,double risk,double max_risk,bool approval_valid,UmiHelixPlanValidation *out);
#ifdef __cplusplus
}
#endif
#endif
