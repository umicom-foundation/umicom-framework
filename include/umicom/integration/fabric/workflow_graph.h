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


/**
 * Represent the fabric workflow edge data shared with callers of this public contract.
 */
typedef struct UmiFabricWorkflowEdge { char before_id[UMI_FABRIC_ID_CAPACITY]; char after_id[UMI_FABRIC_ID_CAPACITY]; } UmiFabricWorkflowEdge;
/**
 * Represent the fabric workflow graph data shared with callers of this public contract.
 */
typedef struct UmiFabricWorkflowGraph { UmiFabricWorkflowStep steps[UMI_FABRIC_MAX_STEPS]; size_t step_count; UmiFabricWorkflowEdge edges[UMI_FABRIC_MAX_EDGES]; size_t edge_count; } UmiFabricWorkflowGraph;
/**
 * Initialise fabric workflow graph from caller-provided values so later operations receive
 * a known state.
 */
void umi_fabric_workflow_graph_init(UmiFabricWorkflowGraph *graph);
/**
 * Provide the fabric workflow graph add step operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_workflow_graph_add_step(UmiFabricWorkflowGraph *graph,const UmiFabricWorkflowStep *step);
/**
 * Provide the fabric workflow graph add edge operation used by this module and its client
 * applications.
 */
UmiStatus umi_fabric_workflow_graph_add_edge(UmiFabricWorkflowGraph *graph,const char *before_id,const char *after_id);
/**
 * Provide the fabric workflow graph has cycle operation used by this module and its client
 * applications.
 */
bool umi_fabric_workflow_graph_has_cycle(const UmiFabricWorkflowGraph *graph);

#ifdef __cplusplus
}
#endif
#endif
