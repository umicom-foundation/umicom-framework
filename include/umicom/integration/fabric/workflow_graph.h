/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/workflow_graph.h
 *
 * PURPOSE:
 *   Maintain workflow dependency edges and reject cycles before orchestration.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_WORKFLOW_GRAPH_H
#define UMICOM_INTEGRATION_FABRIC_WORKFLOW_GRAPH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"
#include "umicom/integration/fabric/workflow_step.h"
#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiFabricWorkflowEdge { char before_id[UMI_FABRIC_ID_CAPACITY]; char after_id[UMI_FABRIC_ID_CAPACITY]; } UmiFabricWorkflowEdge;
typedef struct UmiFabricWorkflowGraph { UmiFabricWorkflowStep steps[UMI_FABRIC_MAX_STEPS]; size_t step_count; UmiFabricWorkflowEdge edges[UMI_FABRIC_MAX_EDGES]; size_t edge_count; } UmiFabricWorkflowGraph;
void umi_fabric_workflow_graph_init(UmiFabricWorkflowGraph *graph);
UmiStatus umi_fabric_workflow_graph_add_step(UmiFabricWorkflowGraph *graph,const UmiFabricWorkflowStep *step);
UmiStatus umi_fabric_workflow_graph_add_edge(UmiFabricWorkflowGraph *graph,const char *before_id,const char *after_id);
bool umi_fabric_workflow_graph_has_cycle(const UmiFabricWorkflowGraph *graph);

#ifdef __cplusplus
}
#endif
#endif
