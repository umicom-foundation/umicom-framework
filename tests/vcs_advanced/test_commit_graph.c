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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/vcs/advanced/commit_graph.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiVcsAdvancedCommitGraph graph;
    UmiVcsAdvancedCommitNode a, b;
    UmiVcsAdvancedCommitEdge edge;
    umi_vcs_advanced_commit_graph_init(&graph);
    umi_vcs_advanced_commit_node_init(&a);
    umi_vcs_advanced_commit_node_init(&b);
    umi_vcs_advanced_commit_edge_init(&edge);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_node_set(&a, "a", "root", "u") != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_node_set(&b, "b", "child", "u") != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_graph_add_node(&graph, &a) != UMI_STATUS_OK) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_graph_add_node(&graph, &b) != UMI_STATUS_OK) return 4;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_edge_set(&edge, "b", "a", 0U) != UMI_STATUS_OK) return 5;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_vcs_advanced_commit_graph_add_edge(&graph, &edge) != UMI_STATUS_OK) return 6;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_commit_graph_root_count(&graph) != 1U) return 7;
    return 0;
}
