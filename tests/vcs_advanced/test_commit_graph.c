/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/vcs_advanced/test_commit_graph.c
 *
 * PURPOSE:
 *   Validate maintain a bounded commit dag with deterministic node and edge lookup.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable VCS capability. Applications, including Studio
 *   and Desk, consume the contract and must not duplicate Git/diff policy.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/commit_graph.h"
int main(void)
{
    UmiVcsAdvancedCommitGraph graph;
    UmiVcsAdvancedCommitNode a, b;
    UmiVcsAdvancedCommitEdge edge;
    umi_vcs_advanced_commit_graph_init(&graph);
    umi_vcs_advanced_commit_node_init(&a);
    umi_vcs_advanced_commit_node_init(&b);
    umi_vcs_advanced_commit_edge_init(&edge);
    if (umi_vcs_advanced_commit_node_set(&a, "a", "root", "u") != UMI_STATUS_OK) return 1;
    if (umi_vcs_advanced_commit_node_set(&b, "b", "child", "u") != UMI_STATUS_OK) return 2;
    if (umi_vcs_advanced_commit_graph_add_node(&graph, &a) != UMI_STATUS_OK) return 3;
    if (umi_vcs_advanced_commit_graph_add_node(&graph, &b) != UMI_STATUS_OK) return 4;
    if (umi_vcs_advanced_commit_edge_set(&edge, "b", "a", 0U) != UMI_STATUS_OK) return 5;
    if (umi_vcs_advanced_commit_graph_add_edge(&graph, &edge) != UMI_STATUS_OK) return 6;
    if (umi_vcs_advanced_commit_graph_root_count(&graph) != 1U) return 7;
    return 0;
}
