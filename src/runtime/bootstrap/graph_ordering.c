/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/graph_ordering.c
 *
 * PURPOSE:
 *   Produce deterministic topological service order from dependency graphs.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/graph_ordering.h"


#include <string.h>

static size_t find_node_index(const UmiBootstrapServiceGraph *graph, const char *id) {
    size_t index;
    for (index = 0U; index < graph->node_count; ++index) {
        if (strcmp(graph->nodes[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

UmiStatus umi_bootstrap_graph_order(const UmiBootstrapServiceGraph *graph,
                                    UmiBootstrapIdList *out_order) {
    size_t indegree[UMI_BOOTSTRAP_MAX_ITEMS] = {0U};
    bool used[UMI_BOOTSTRAP_MAX_ITEMS] = {false};
    size_t emitted = 0U;
    size_t edge_index;
    if (graph == NULL || out_order == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_order, 0, sizeof(*out_order));
    for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
        size_t dependent = find_node_index(graph, graph->edges[edge_index].dependent_id);
        size_t dependency = find_node_index(graph, graph->edges[edge_index].dependency_id);
        if (dependent == SIZE_MAX || dependency == SIZE_MAX) {
            if (graph->edges[edge_index].required) return UMI_STATUS_NOT_FOUND;
            continue;
        }
        if (graph->nodes[dependent].enabled && graph->nodes[dependency].enabled) {
            ++indegree[dependent];
        }
    }
    while (emitted < graph->node_count) {
        size_t candidate = SIZE_MAX;
        size_t index;
        for (index = 0U; index < graph->node_count; ++index) {
            if (!graph->nodes[index].enabled || used[index] || indegree[index] != 0U) continue;
            if (candidate == SIZE_MAX ||
                graph->nodes[index].priority > graph->nodes[candidate].priority ||
                (graph->nodes[index].priority == graph->nodes[candidate].priority &&
                 strcmp(graph->nodes[index].id, graph->nodes[candidate].id) < 0)) {
                candidate = index;
            }
        }
        if (candidate == SIZE_MAX) {
            bool pending_enabled = false;
            for (index = 0U; index < graph->node_count; ++index) {
                if (graph->nodes[index].enabled && !used[index]) pending_enabled = true;
                if (!graph->nodes[index].enabled && !used[index]) {
                    used[index] = true;
                    ++emitted;
                }
            }
            if (pending_enabled) return UMI_STATUS_INVALID_STATE;
            continue;
        }
        if (out_order->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)umi_bootstrap_copy_text(out_order->ids[out_order->count],
                                      sizeof(out_order->ids[out_order->count]),
                                      graph->nodes[candidate].id);
        ++out_order->count;
        used[candidate] = true;
        ++emitted;
        for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
            if (strcmp(graph->edges[edge_index].dependency_id,
                       graph->nodes[candidate].id) == 0) {
                size_t dependent = find_node_index(graph, graph->edges[edge_index].dependent_id);
                if (dependent != SIZE_MAX && indegree[dependent] > 0U) --indegree[dependent];
            }
        }
    }
    return UMI_STATUS_OK;
}
