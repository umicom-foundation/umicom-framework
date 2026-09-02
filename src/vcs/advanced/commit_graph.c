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

/*
 * Initialise vcs advanced commit graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_vcs_advanced_commit_graph_init(UmiVcsAdvancedCommitGraph *graph)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return;
    (void)memset(graph, 0, sizeof(*graph));
    graph->struct_size = (uint32_t)sizeof(*graph);
    graph->api_version = UMI_VCS_ADVANCED_API_VERSION;
    graph->revision = 1U;
}

/*
 * Find vcs advanced commit graph while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiVcsAdvancedCommitNode *umi_vcs_advanced_commit_graph_find(
    const UmiVcsAdvancedCommitGraph *graph, const char *oid)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || !umi_vcs_advanced_text_present(oid)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < graph->node_count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_vcs_advanced_text_equal(graph->nodes[i].oid, oid)) return &graph->nodes[i];
    }
    return NULL;
}

/*
 * Provide the vcs advanced commit graph add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_commit_graph_add_node(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitNode *node)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || umi_vcs_advanced_commit_node_validate(node) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_vcs_advanced_commit_graph_find(graph, node->oid) != NULL)
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->node_count >= UMI_VCS_ADVANCED_LIST_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->nodes[graph->node_count++] = *node;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the vcs advanced commit graph has edge operation used by this module and its
 * client applications.
 */
int umi_vcs_advanced_commit_graph_has_edge(const UmiVcsAdvancedCommitGraph *graph,
                                            const char *child_oid,
                                            const char *parent_oid)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < graph->edge_count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (umi_vcs_advanced_text_equal(graph->edges[i].child_oid, child_oid) &&
            umi_vcs_advanced_text_equal(graph->edges[i].parent_oid, parent_oid))
            return 1;
    }
    return 0;
}

/*
 * Provide the vcs advanced commit graph add edge operation used by this module and its
 * client applications.
 */
UmiStatus umi_vcs_advanced_commit_graph_add_edge(UmiVcsAdvancedCommitGraph *graph,
                                                  const UmiVcsAdvancedCommitEdge *edge)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || umi_vcs_advanced_commit_edge_validate(edge) != UMI_STATUS_OK)
        return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_vcs_advanced_commit_graph_find(graph, edge->child_oid) == NULL ||
        umi_vcs_advanced_commit_graph_find(graph, edge->parent_oid) == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_vcs_advanced_commit_graph_has_edge(graph, edge->child_oid, edge->parent_oid))
        return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->edge_count >= UMI_VCS_ADVANCED_GRAPH_EDGE_CAPACITY)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    graph->edges[graph->edge_count++] = *edge;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by vcs advanced commit graph root without
 * changing their state.
 */
size_t umi_vcs_advanced_commit_graph_root_count(const UmiVcsAdvancedCommitGraph *graph)
{
    size_t roots = 0U, i, j;
    int has_parent;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < graph->node_count; ++i) {
        has_parent = 0;
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = 0U; j < graph->edge_count; ++j) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (umi_vcs_advanced_text_equal(graph->edges[j].child_oid, graph->nodes[i].oid)) {
                has_parent = 1;
                break;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (!has_parent) roots += 1U;
    }
    return roots;
}
