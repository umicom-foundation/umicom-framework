/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/compiler/interop_graph.h
 *
 * PURPOSE:
 *   Publish the public interop graph contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_COMPILER_INTEROP_GRAPH_H
#define UMICOM_COMPILER_INTEROP_GRAPH_H
#include "umicom/compiler/abi.h"
#define UMI_INTEROP_MAX_NODES 64U
#define UMI_INTEROP_MAX_EDGES 128U
/**
 * Represent the interop node data shared with callers of this public contract.
 */
typedef struct UmiInteropNode { char unit_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerLanguage language; UmiCompilerAbiProfile abi; } UmiInteropNode;
/**
 * Represent the interop edge data shared with callers of this public contract.
 */
typedef struct UmiInteropEdge { char producer_id[UMI_COMPILER_ID_CAPACITY]; char consumer_id[UMI_COMPILER_ID_CAPACITY]; char symbol_prefix[64U]; bool required; } UmiInteropEdge;
/**
 * Represent the interop graph data shared with callers of this public contract.
 */
typedef struct UmiInteropGraph { UmiInteropNode nodes[UMI_INTEROP_MAX_NODES]; UmiInteropEdge edges[UMI_INTEROP_MAX_EDGES]; size_t node_count; size_t edge_count; uint64_t revision; } UmiInteropGraph;
/**
 * Provide the interop graph add node operation used by this module and its client
 * applications.
 */
UmiStatus umi_interop_graph_add_node(UmiInteropGraph *graph,const UmiInteropNode *node);
/**
 * Provide the interop graph add edge operation used by this module and its client
 * applications.
 */
UmiStatus umi_interop_graph_add_edge(UmiInteropGraph *graph,const UmiInteropEdge *edge);
/**
 * Check that interop graph satisfies its contract before another service relies on it.
 */
UmiStatus umi_interop_graph_validate(const UmiInteropGraph *graph,char *out_reason,size_t capacity);
/**
 * Provide the interop graph order operation used by this module and its client
 * applications.
 */
UmiStatus umi_interop_graph_order(const UmiInteropGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count);
#endif
