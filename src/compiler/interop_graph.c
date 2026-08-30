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
static size_t node_index(const UmiInteropGraph *graph,const char *id) { size_t index; if (graph == NULL || id == NULL) return SIZE_MAX; for (index = 0U; index < graph->node_count; ++index) if (strcmp(graph->nodes[index].unit_id,id) == 0) return index; return SIZE_MAX; }
UmiStatus umi_interop_graph_add_node(UmiInteropGraph *graph,const UmiInteropNode *node) { if (graph == NULL || node == NULL || node->unit_id[0] == '\0' || node->language == UMI_COMPILER_LANGUAGE_UNKNOWN) return UMI_STATUS_INVALID_ARGUMENT; if (node_index(graph,node->unit_id) != SIZE_MAX) return UMI_STATUS_ALREADY_EXISTS; if (graph->node_count >= UMI_INTEROP_MAX_NODES) return UMI_STATUS_CAPACITY_EXCEEDED; graph->nodes[graph->node_count++] = *node; graph->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_interop_graph_add_edge(UmiInteropGraph *graph,const UmiInteropEdge *edge) { size_t index; if (graph == NULL || edge == NULL || edge->producer_id[0] == '\0' || edge->consumer_id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT; if (node_index(graph,edge->producer_id) == SIZE_MAX || node_index(graph,edge->consumer_id) == SIZE_MAX) return UMI_STATUS_NOT_FOUND; for (index = 0U; index < graph->edge_count; ++index) if (strcmp(graph->edges[index].producer_id,edge->producer_id) == 0 && strcmp(graph->edges[index].consumer_id,edge->consumer_id) == 0) return UMI_STATUS_ALREADY_EXISTS; if (graph->edge_count >= UMI_INTEROP_MAX_EDGES) return UMI_STATUS_CAPACITY_EXCEEDED; graph->edges[graph->edge_count++] = *edge; graph->revision += 1U; return UMI_STATUS_OK; }
UmiStatus umi_interop_graph_validate(const UmiInteropGraph *graph,char *out_reason,size_t capacity) { size_t index; int length; if (graph == NULL || out_reason == NULL || capacity == 0U) return UMI_STATUS_INVALID_ARGUMENT; for (index = 0U; index < graph->edge_count; ++index) { size_t producer = node_index(graph,graph->edges[index].producer_id); size_t consumer = node_index(graph,graph->edges[index].consumer_id); if (producer == SIZE_MAX || consumer == SIZE_MAX || !umi_compiler_abi_compatible(&graph->nodes[producer].abi,&graph->nodes[consumer].abi)) { length = snprintf(out_reason,capacity,"Incompatible edge %s -> %s",graph->edges[index].producer_id,graph->edges[index].consumer_id); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_INVALID_STATE; } } length = snprintf(out_reason,capacity,"Interoperability graph is valid"); return length < 0 || (size_t)length >= capacity ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK; }
UmiStatus umi_interop_graph_order(const UmiInteropGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count)
{
    size_t indegree[UMI_INTEROP_MAX_NODES] = {0}; bool emitted[UMI_INTEROP_MAX_NODES] = {false}; size_t edge; size_t produced = 0U;
    if (graph == NULL || out_indices == NULL || out_count == NULL || capacity < graph->node_count) return UMI_STATUS_INVALID_ARGUMENT;
    for (edge = 0U; edge < graph->edge_count; ++edge) { size_t consumer = node_index(graph,graph->edges[edge].consumer_id); if (consumer != SIZE_MAX) indegree[consumer] += 1U; }
    while (produced < graph->node_count) { size_t index; bool progress = false; for (index = 0U; index < graph->node_count; ++index) if (!emitted[index] && indegree[index] == 0U) { out_indices[produced++] = index; emitted[index] = true; progress = true; for (edge = 0U; edge < graph->edge_count; ++edge) if (strcmp(graph->edges[edge].producer_id,graph->nodes[index].unit_id) == 0) { size_t consumer = node_index(graph,graph->edges[edge].consumer_id); if (consumer != SIZE_MAX && indegree[consumer] > 0U) indegree[consumer] -= 1U; } } if (!progress) return UMI_STATUS_INVALID_STATE; }
    *out_count = produced; return UMI_STATUS_OK;
}
