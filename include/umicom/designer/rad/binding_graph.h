/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/rad/binding_graph.h
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
#ifndef UMICOM_DESIGNER_RAD_BINDING_GRAPH_H
#define UMICOM_DESIGNER_RAD_BINDING_GRAPH_H
#include "umicom/designer/rad/binding_wire.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiRadBindingGraph { UmiRadBindingWire wires[UMI_RAD_MAX_BINDINGS]; size_t count; } UmiRadBindingGraph;
UmiStatus umi_rad_binding_graph_init(UmiRadBindingGraph *graph);
UmiStatus umi_rad_binding_graph_add(UmiRadBindingGraph *graph,const UmiRadBindingWire *wire);
int umi_rad_binding_graph_has_cycle(const UmiRadBindingGraph *graph);
#ifdef __cplusplus
}
#endif
#endif
