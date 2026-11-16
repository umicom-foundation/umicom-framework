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
/* Mutations require valid acyclic existing state and leave every graph byte
 * unchanged on failure. Step storage and terminated ID strings may borrow
 * existing live or unused slots; repeated dependency edges remain supported. */
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
/* NULL retains the historical false result. Malformed non-null storage returns
 * true conservatively; use the checked API to distinguish it from a cycle. */
bool umi_fabric_workflow_graph_has_cycle(const UmiFabricWorkflowGraph *graph);

/* Validate capacities, every live step/text field, unique step IDs and edge
 * endpoints. Empty graphs and repeated edges are valid. Cycles are queried
 * separately; unused slots are ignored. Invalid storage is INVALID_STATE.
 * Callers serialize access and provide a complete graph object. No I/O occurs. */
UmiStatus umi_fabric_workflow_graph_validate(const UmiFabricWorkflowGraph *graph);
/* On error out_cycle is unchanged. It must not overlap graph storage. */
UmiStatus umi_fabric_workflow_graph_has_cycle_checked(
    const UmiFabricWorkflowGraph *graph, bool *out_cycle);

#ifdef __cplusplus
}
#endif
#endif
