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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_VCS_ADVANCED_COMMIT_GRAPH_H
#define UMICOM_VCS_ADVANCED_COMMIT_GRAPH_H

#include "umicom/vcs/advanced/commit_node.h"
#include "umicom/vcs/advanced/commit_edge.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the vcs advanced commit graph data shared with callers of this public
 * contract.
 */
typedef struct UmiVcsAdvancedCommitGraph {
    uint32_t struct_size;
    uint32_t api_version;
    UmiVcsAdvancedCommitNode nodes[UMI_VCS_ADVANCED_LIST_CAPACITY];
    UmiVcsAdvancedCommitEdge edges[UMI_VCS_ADVANCED_GRAPH_EDGE_CAPACITY];
    size_t node_count;
    size_t edge_count;
    uint64_t revision;
} UmiVcsAdvancedCommitGraph;

/**
 * Initialise vcs advanced commit graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_graph_init(UmiVcsAdvancedCommitGraph *graph);
/**
 * Provide the vcs advanced commit graph add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_commit_graph_add_node(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitNode *node);
/**
 * Provide the vcs advanced commit graph add edge operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_commit_graph_add_edge(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitEdge *edge);
/**
 * Find vcs advanced commit graph while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiVcsAdvancedCommitNode *umi_vcs_advanced_commit_graph_find(
    const UmiVcsAdvancedCommitGraph *graph, const char *oid);
/**
 * Return the number of records represented by vcs advanced commit graph root without
 * changing their state.
 */
size_t umi_vcs_advanced_commit_graph_root_count(const UmiVcsAdvancedCommitGraph *graph);
/**
 * Provide the vcs advanced commit graph has edge operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_commit_graph_has_edge(const UmiVcsAdvancedCommitGraph *graph,
                                            const char *child_oid,
                                            const char *parent_oid);

#ifdef __cplusplus
}
#endif
#endif
