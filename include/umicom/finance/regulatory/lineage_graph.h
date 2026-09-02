/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/lineage_graph.h
 *
 * PURPOSE:
 *   Maintain a bounded acyclic data-lineage graph with cycle prevention.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_LINEAGE_GRAPH_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_LINEAGE_GRAPH_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the lineage graph edge data shared with callers of this public contract.
 */
typedef struct UmiLineageGraphEdge { size_t from_index; size_t to_index; } UmiLineageGraphEdge;
/**
 * Represent the lineage graph data shared with callers of this public contract.
 */
typedef struct UmiLineageGraph { char node_ids[UMI_REG_MAX_NODES][UMI_REG_ID_CAPACITY]; size_t node_count; UmiLineageGraphEdge edges[UMI_REG_MAX_EDGES]; size_t edge_count; } UmiLineageGraph;
/* Initialise an empty lineage graph. */
void umi_reg_lineage_graph_init(UmiLineageGraph *graph);
/* Add a unique lineage node. */
UmiStatus umi_reg_lineage_graph_add_node(UmiLineageGraph *graph,const char *node_id);
/* Add a directed edge only when it cannot create a cycle. */
UmiStatus umi_reg_lineage_graph_add_edge(UmiLineageGraph *graph,const char *from_node,const char *to_node);

#ifdef __cplusplus
}
#endif

#endif
