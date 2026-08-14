/* Umicom Framework | Polyglot interoperability graph | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_COMPILER_INTEROP_GRAPH_H
#define UMICOM_COMPILER_INTEROP_GRAPH_H
#include "umicom/compiler/abi.h"
#define UMI_INTEROP_MAX_NODES 64U
#define UMI_INTEROP_MAX_EDGES 128U
typedef struct UmiInteropNode { char unit_id[UMI_COMPILER_ID_CAPACITY]; UmiCompilerLanguage language; UmiCompilerAbiProfile abi; } UmiInteropNode;
typedef struct UmiInteropEdge { char producer_id[UMI_COMPILER_ID_CAPACITY]; char consumer_id[UMI_COMPILER_ID_CAPACITY]; char symbol_prefix[64U]; bool required; } UmiInteropEdge;
typedef struct UmiInteropGraph { UmiInteropNode nodes[UMI_INTEROP_MAX_NODES]; UmiInteropEdge edges[UMI_INTEROP_MAX_EDGES]; size_t node_count; size_t edge_count; uint64_t revision; } UmiInteropGraph;
UmiStatus umi_interop_graph_add_node(UmiInteropGraph *graph,const UmiInteropNode *node);
UmiStatus umi_interop_graph_add_edge(UmiInteropGraph *graph,const UmiInteropEdge *edge);
UmiStatus umi_interop_graph_validate(const UmiInteropGraph *graph,char *out_reason,size_t capacity);
UmiStatus umi_interop_graph_order(const UmiInteropGraph *graph,size_t *out_indices,size_t capacity,size_t *out_count);
#endif
