/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/dependency_graph.c
 *
 * PURPOSE:
 *   Build and validate dependency topology including cycle detection.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/dependency_graph.h"

#include <string.h>

/*
 * Initialise repository dependency graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_dependency_graph_init(UmiRepositoryDependencyGraph *graph)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return;
    (void)memset(graph, 0, sizeof(*graph));
    graph->revision = 1U;
}

/* Provide the find node operation used by this module and its client applications. */
static size_t find_node(
    const UmiRepositoryDependencyGraph *graph, const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || id == NULL) return SIZE_MAX;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(graph->nodes[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Provide the repository dependency graph add node operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_dependency_graph_add_node(
    UmiRepositoryDependencyGraph *graph,
    const UmiRepositoryDependency *dependency)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL ||
        umi_repository_dependency_validate(dependency) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (find_node(graph, dependency->id) != SIZE_MAX) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->node_count >= UMI_REPOSITORY_CONTROL_ITEM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->nodes[graph->node_count++] = *dependency;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository dependency graph add edge operation used by this module and its
 * client applications.
 */
UmiStatus umi_repository_dependency_graph_add_edge(
    UmiRepositoryDependencyGraph *graph,
    const char *from_id,
    const char *to_id)
{
    size_t from_index;
    size_t to_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || from_id == NULL || to_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    from_index = find_node(graph, from_id);
    to_index = find_node(graph, to_id);
    /* Apply this branch only when its contract condition is satisfied. */
    if (from_index == SIZE_MAX || to_index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->edge_count >= UMI_REPOSITORY_CONTROL_EDGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->edges[graph->edge_count].from_index = from_index;
    graph->edges[graph->edge_count].to_index = to_index;
    graph->edge_count += 1U;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

/* Provide the visit node operation used by this module and its client applications. */
static int visit_node(
    const UmiRepositoryDependencyGraph *graph,
    size_t node,
    unsigned char *visiting,
    unsigned char *visited)
{
    size_t edge_index;
    /* Apply this branch only when its contract condition is satisfied. */
    if (visiting[node] != 0U) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (visited[node] != 0U) return 0;
    visiting[node] = 1U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
        const UmiRepositoryDependencyEdge *edge = &graph->edges[edge_index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (edge->from_index == node &&
            visit_node(graph, edge->to_index, visiting, visited)) {
            return 1;
        }
    }
    visiting[node] = 0U;
    visited[node] = 1U;
    return 0;
}

/*
 * Provide the repository dependency graph has cycle operation used by this module and its
 * client applications.
 */
int umi_repository_dependency_graph_has_cycle(
    const UmiRepositoryDependencyGraph *graph)
{
    unsigned char visiting[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY] = {0};
    unsigned char visited[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY] = {0};
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (visit_node(graph, index, visiting, visited)) return 1;
    }
    return 0;
}
