/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/vcs/advanced/commit_graph.h
 *
 * PURPOSE:
 *   Maintain a bounded commit DAG with deterministic node and edge lookup.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_COMMIT_GRAPH_H
#define UMICOM_VCS_ADVANCED_COMMIT_GRAPH_H

#include "umicom/vcs/advanced/commit_node.h"
#include "umicom/vcs/advanced/commit_edge.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiVcsAdvancedCommitGraph {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedCommitNode nodes[UMI_VCS_ADVANCED_LIST_CAPACITY];
    UmiVcsAdvancedCommitEdge edges[UMI_VCS_ADVANCED_GRAPH_EDGE_CAPACITY];
    size_t node_count;
    size_t edge_count;
    uint64_t revision;
} UmiVcsAdvancedCommitGraph;

void umi_vcs_advanced_commit_graph_init(UmiVcsAdvancedCommitGraph *graph);
UmiStatus umi_vcs_advanced_commit_graph_add_node(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitNode *node);
UmiStatus umi_vcs_advanced_commit_graph_add_edge(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitEdge *edge);
const UmiVcsAdvancedCommitNode *umi_vcs_advanced_commit_graph_find(
    const UmiVcsAdvancedCommitGraph *graph, const char *oid);
size_t umi_vcs_advanced_commit_graph_root_count(const UmiVcsAdvancedCommitGraph *graph);
int umi_vcs_advanced_commit_graph_has_edge(const UmiVcsAdvancedCommitGraph *graph,
                                            const char *child_oid,
                                            const char *parent_oid);

#ifdef __cplusplus
}
#endif
#endif
