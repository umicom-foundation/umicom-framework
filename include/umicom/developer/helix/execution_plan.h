/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/execution_plan.h
 *
 * PURPOSE:
 *   Order executable steps using dependency edges and reject cyclic or unapproved plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_EXECUTION_PLAN_H
#define UMICOM_DEVELOPER_HELIX_EXECUTION_PLAN_H
#include "umicom/developer/helix/execution_step.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixExecutionEdge { uint16_t from; uint16_t to; } UmiHelixExecutionEdge;
typedef struct UmiHelixExecutionPlan { UmiHelixExecutionStep steps[UMI_HELIX_MAX_ITEMS]; size_t count; UmiHelixExecutionEdge edges[UMI_HELIX_MAX_EDGES]; size_t edge_count; bool approved; } UmiHelixExecutionPlan;
/* Add a validated execution step. */
UmiStatus umi_helix_execution_plan_add(UmiHelixExecutionPlan *plan,const UmiHelixExecutionStep *step);
/* Add a prerequisite edge between execution steps. */
UmiStatus umi_helix_execution_plan_depend(UmiHelixExecutionPlan *plan,size_t prerequisite,size_t dependent);
/* Produce prerequisite-first order only when the plan is approved and acyclic. */
UmiStatus umi_helix_execution_plan_order(const UmiHelixExecutionPlan *plan,size_t *out,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
