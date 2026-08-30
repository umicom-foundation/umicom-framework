/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/regeneration_graph.h
 *
 * PURPOSE:
 *   Track dependencies among regeneration units and reject cyclic regeneration plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_REGENERATION_GRAPH_H
#define UMICOM_DEVELOPER_HELIX_REGENERATION_GRAPH_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixRegenerationGraphEdge { uint16_t from; uint16_t to; } UmiHelixRegenerationGraphEdge;
typedef struct UmiHelixRegenerationGraph { char ids[UMI_HELIX_MAX_ITEMS][UMI_HELIX_ID_CAPACITY]; size_t count; UmiHelixRegenerationGraphEdge edges[UMI_HELIX_MAX_EDGES]; size_t edge_count; } UmiHelixRegenerationGraph;
/* Initialise an empty bounded graph. */
void umi_helix_regeneration_graph_init(UmiHelixRegenerationGraph *graph);
/* Add a stable unit identifier and reject duplicates. */
UmiStatus umi_helix_regeneration_graph_add(UmiHelixRegenerationGraph *graph,const char *id);
/* Add a dependency edge by node index. */
UmiStatus umi_helix_regeneration_graph_depend(UmiHelixRegenerationGraph *graph,size_t prerequisite,size_t dependent);
/* Detect directed dependency cycles. */
bool umi_helix_regeneration_graph_has_cycle(const UmiHelixRegenerationGraph *graph);
/* Produce deterministic prerequisite-first topological order. */
UmiStatus umi_helix_regeneration_graph_order(const UmiHelixRegenerationGraph *graph,size_t *out_order,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
