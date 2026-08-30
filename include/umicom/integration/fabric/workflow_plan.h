/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/workflow_plan.h
 *
 * PURPOSE:
 *   Topologically order workflow steps so dependencies are always executed first.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_WORKFLOW_PLAN_H
#define UMICOM_INTEGRATION_FABRIC_WORKFLOW_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/workflow_graph.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricWorkflowPlan { char step_ids[UMI_FABRIC_MAX_STEPS][UMI_FABRIC_ID_CAPACITY]; size_t count; uint64_t fingerprint; } UmiFabricWorkflowPlan;
UmiStatus umi_fabric_workflow_plan_build(const UmiFabricWorkflowGraph *graph,UmiFabricWorkflowPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
