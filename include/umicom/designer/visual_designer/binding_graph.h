/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/binding_graph.h
 *
 * PURPOSE:
 *   Maintain an acyclic visual binding graph with cycle detection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_GRAPH_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_GRAPH_H
#include "umicom/designer/visual_designer/binding_wire.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer binding graph data shared with callers of this public contract.
 */
typedef struct UmiRadBindingGraph { UmiRadBindingWire wires[UMI_RAD_MAX_BINDINGS]; size_t count; } UmiRadBindingGraph;
/**
 * Initialise visual designer binding graph from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_rad_binding_graph_init(UmiRadBindingGraph *graph);
/**
 * Add visual designer binding graph only after its inputs and available capacity have been checked.
 */
UmiStatus umi_rad_binding_graph_add(UmiRadBindingGraph *graph,const UmiRadBindingWire *wire);
/**
 * Provide the visual designer binding graph has cycle operation used by this module and its client
 * applications.
 */
int umi_rad_binding_graph_has_cycle(const UmiRadBindingGraph *graph);
#ifdef __cplusplus
}
#endif
#endif
