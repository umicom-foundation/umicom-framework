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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/dependency_graph.h"

#include <string.h>

void umi_repository_dependency_graph_init(UmiRepositoryDependencyGraph *graph)
{
    if (graph == NULL) return;
    (void)memset(graph, 0, sizeof(*graph));
    graph->revision = 1U;
}

static size_t find_node(
    const UmiRepositoryDependencyGraph *graph, const char *id)
{
    size_t index;
    if (graph == NULL || id == NULL) return SIZE_MAX;
    for (index = 0U; index < graph->node_count; ++index) {
        if (strcmp(graph->nodes[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_repository_dependency_graph_add_node(
    UmiRepositoryDependencyGraph *graph,
    const UmiRepositoryDependency *dependency)
{
    if (graph == NULL ||
        umi_repository_dependency_validate(dependency) != UMI_STATUS_OK) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (find_node(graph, dependency->id) != SIZE_MAX) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (graph->node_count >= UMI_REPOSITORY_CONTROL_ITEM_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->nodes[graph->node_count++] = *dependency;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_repository_dependency_graph_add_edge(
    UmiRepositoryDependencyGraph *graph,
    const char *from_id,
    const char *to_id)
{
    size_t from_index;
    size_t to_index;
    if (graph == NULL || from_id == NULL || to_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    from_index = find_node(graph, from_id);
    to_index = find_node(graph, to_id);
    if (from_index == SIZE_MAX || to_index == SIZE_MAX) {
        return UMI_STATUS_NOT_FOUND;
    }
    if (graph->edge_count >= UMI_REPOSITORY_CONTROL_EDGE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->edges[graph->edge_count].from_index = from_index;
    graph->edges[graph->edge_count].to_index = to_index;
    graph->edge_count += 1U;
    graph->revision += 1U;
    return UMI_STATUS_OK;
}

static int visit_node(
    const UmiRepositoryDependencyGraph *graph,
    size_t node,
    unsigned char *visiting,
    unsigned char *visited)
{
    size_t edge_index;
    if (visiting[node] != 0U) return 1;
    if (visited[node] != 0U) return 0;
    visiting[node] = 1U;
    for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
        const UmiRepositoryDependencyEdge *edge = &graph->edges[edge_index];
        if (edge->from_index == node &&
            visit_node(graph, edge->to_index, visiting, visited)) {
            return 1;
        }
    }
    visiting[node] = 0U;
    visited[node] = 1U;
    return 0;
}

int umi_repository_dependency_graph_has_cycle(
    const UmiRepositoryDependencyGraph *graph)
{
    unsigned char visiting[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY] = {0};
    unsigned char visited[UMI_REPOSITORY_CONTROL_ITEM_CAPACITY] = {0};
    size_t index;
    if (graph == NULL) return 0;
    for (index = 0U; index < graph->node_count; ++index) {
        if (visit_node(graph, index, visiting, visited)) return 1;
    }
    return 0;
}
