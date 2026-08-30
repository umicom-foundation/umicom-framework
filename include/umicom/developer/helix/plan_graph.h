/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/plan_graph.h
 *
 * PURPOSE:
 *   Store a bounded directed task graph and provide dependency/cycle/topological-order validation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_PLAN_GRAPH_H
#define UMICOM_DEVELOPER_HELIX_PLAN_GRAPH_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixPlanGraphEdge { uint16_t from; uint16_t to; } UmiHelixPlanGraphEdge;
typedef struct UmiHelixPlanGraph { char ids[UMI_HELIX_MAX_ITEMS][UMI_HELIX_ID_CAPACITY]; size_t count; UmiHelixPlanGraphEdge edges[UMI_HELIX_MAX_EDGES]; size_t edge_count; } UmiHelixPlanGraph;
/* Initialise an empty bounded graph. */
void umi_helix_plan_graph_init(UmiHelixPlanGraph *graph);
/* Add a stable task identifier and reject duplicates. */
UmiStatus umi_helix_plan_graph_add(UmiHelixPlanGraph *graph,const char *id);
/* Add a dependency edge by node index. */
UmiStatus umi_helix_plan_graph_depend(UmiHelixPlanGraph *graph,size_t prerequisite,size_t dependent);
/* Detect directed dependency cycles. */
bool umi_helix_plan_graph_has_cycle(const UmiHelixPlanGraph *graph);
/* Produce deterministic prerequisite-first topological order. */
UmiStatus umi_helix_plan_graph_order(const UmiHelixPlanGraph *graph,size_t *out_order,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
