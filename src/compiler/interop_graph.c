/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/interop_graph.c
 *
 * PURPOSE:
 *   Implement the interop graph behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Polyglot interoperability graph | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/interop_graph.h"
#include <stdio.h>
#include <string.h>
/* Provide the node index operation used by this module and its client applications. */
static size_t node_index(const UmiInteropGraph *graph,const char *id) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || id == NULL) return SIZE_MAX; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < graph->node_count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(graph->nodes[index].unit_id,id) == 0) return index; return SIZE_MAX; }
/*
 * Provide the interop graph add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_interop_graph_add_node(UmiInteropGraph *graph,const UmiInteropNode *node) { /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || node == NULL || node->unit_id[0] == '\0' || node->language == UMI_COMPILER_LANGUAGE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (node_index(graph,node->unit_id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph->node_count >= UMI_INTEROP_MAX_NODES) return UMI_STATUS_CAPACITY_EXCEEDED; graph->nodes[graph->node_count++] = *node; graph->revision += 1U; return UMI_STATUS_OK; }
/*
 * Provide the interop graph add edge operation used by this module and its client
 * applications.
 */
UmiStatus umi_interop_graph_add_edge(UmiInteropGraph *graph,const UmiInteropEdge *edge) { size_t index; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || edge == NULL || edge->producer_id[0] == '\0' || edge->consumer_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (node_index(graph,edge->producer_id) == SIZE_MAX || node_index(graph,edge->consumer_id) == SIZE_MAX) return UMI_STATUS_NOT_FOUND; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < graph->edge_count; ++index) /* Protect caller-owned memory by checking that required state is available before it is used. */ if (strcmp(graph->edges[index].producer_id,edge->producer_id) == 0 && strcmp(graph->edges[index].consumer_id,edge->consumer_id) == 0) return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph->edge_count >= UMI_INTEROP_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED; graph->edges[graph->edge_count++] = *edge; graph->revision += 1U; return UMI_STATUS_OK; }
/* Check that interop graph satisfies its contract before another service relies on it. */
UmiStatus umi_interop_graph_validate(const UmiInteropGraph *graph,char *out_reason,size_t capacity) { size_t index; int length; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (graph == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < graph->edge_count; ++index) { size_t producer = node_index(graph,graph->edges[index].producer_id); size_t consumer = node_index(graph,graph->edges[index].consumer_id); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (producer == SIZE_MAX || consumer == SIZE_MAX || !umi_compiler_abi_compatible(&graph->nodes[producer].abi,&graph->nodes[consumer].abi)) { length = snprintf(out_reason,capacity,"Incompatible edge %s -> %s",graph->edges[index].producer_id,graph->edges[index].consumer_id); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; } } length = snprintf(out_reason,capacity,"Interoperability graph is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK; }
/*
 * Provide the interop graph order operation used by this module and its client
 * applications.
 */
UmiStatus umi_interop_graph_order(const UmiInteropGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count)
{
    size_t indegree[UMI_INTEROP_MAX_NODES] = {0}; bool emitted[UMI_INTEROP_MAX_NODES] = {false}; size_t edge; size_t produced = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || out_indices == NULL || out_count == NULL || capacity < graph->node_count) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (edge = 0U; edge < graph->edge_count; ++edge) { size_t consumer = node_index(graph,graph->edges[edge].consumer_id); /* Use the stable identifier comparison to choose the matching record or policy. */ if (consumer != SIZE_MAX) indegree[consumer] += 1U; }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (produced < graph->node_count) { size_t index; bool progress = false; /* Visit each bounded item once so every record receives the same rule. */ for (index = 0U; index < graph->node_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!emitted[index] && indegree[index] == 0U) { out_indices[produced++] = index; emitted[index] = true; progress = true; /* Visit each bounded item once so every record receives the same rule. */ for (edge = 0U; edge < graph->edge_count; ++edge) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(graph->edges[edge].producer_id,graph->nodes[index].unit_id) == 0) { size_t consumer = node_index(graph,graph->edges[edge].consumer_id); /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (consumer != SIZE_MAX && indegree[consumer] > 0U) indegree[consumer] -= 1U; } } /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (!progress) return UMI_STATUS_INVALID_STATE; }
    *out_count = produced; return UMI_STATUS_OK;
}
