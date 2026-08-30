/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/advanced/commit_graph.c
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
#include "umicom/vcs/advanced/commit_graph.h"

#include <string.h>

void umi_vcs_advanced_commit_graph_init(UmiVcsAdvancedCommitGraph *graph)
{
    if (graph == NULL) return;
    (void)memset(graph, 0, sizeof(*graph));
    graph->struct_size = (uint32_t)sizeof(*graph);
    graph->api_version = UMI_VCS_ADVANCED_API_VERSION;
    graph->revision = 1U;
}

const UmiVcsAdvancedCommitNode *umi_vcs_advanced_commit_graph_find(
    const UmiVcsAdvancedCommitGraph *graph, const char *oid)
{
    size_t i;
    if (graph == NULL || !umi_vcs_advanced_text_present(oid)) return NULL;
    for (i = 0U; i < graph->node_count; ++i) {
        if (umi_vcs_advanced_text_equal(graph->nodes[i].oid, oid)) return &graph->nodes[i];
    }
    return NULL;
}

UmiStatus umi_vcs_advanced_commit_graph_add_node(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitNode *node)
{
    if (graph == NULL || umi_vcs_advanced_commit_node_validate(node) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_vcs_advanced_commit_graph_find(graph, node->oid) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    if (graph->node_count >= UMI_VCS_ADVANCED_LIST_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->nodes[graph->node_count++] = *node;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

int umi_vcs_advanced_commit_graph_has_edge(const UmiVcsAdvancedCommitGraph *graph,
                                            const char *child_oid,
                                            const char *parent_oid)
{
    size_t i;
    if (graph == NULL) return 0;
    for (i = 0U; i < graph->edge_count; ++i) {
        if (umi_vcs_advanced_text_equal(graph->edges[i].child_oid, child_oid) &&
            umi_vcs_advanced_text_equal(graph->edges[i].parent_oid, parent_oid))
            return 1;
    }
    return 0;
}

UmiStatus umi_vcs_advanced_commit_graph_add_edge(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitEdge *edge)
{
    if (graph == NULL || umi_vcs_advanced_commit_edge_validate(edge) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_vcs_advanced_commit_graph_find(graph, edge->child_oid) == NULL ||
        umi_vcs_advanced_commit_graph_find(graph, edge->parent_oid) == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (umi_vcs_advanced_commit_graph_has_edge(graph, edge->child_oid, edge->parent_oid))
        return UMI_STATUS_ALREADY_EXISTS;
    if (graph->edge_count >= UMI_VCS_ADVANCED_GRAPH_EDGE_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->edges[graph->edge_count++] = *edge;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

size_t umi_vcs_advanced_commit_graph_root_count(const UmiVcsAdvancedCommitGraph *graph)
{
    size_t roots = 0U, i, j;
    int has_parent;
    if (graph == NULL) return 0U;
    for (i = 0U; i < graph->node_count; ++i) {
        has_parent = 0;
        for (j = 0U; j < graph->edge_count; ++j) {
            if (umi_vcs_advanced_text_equal(graph->edges[j].child_oid, graph->nodes[i].oid)) {
                has_parent = 1;
                break;
            }
        }
        if (!has_parent) roots += 1U;
    }
    return roots;
}
