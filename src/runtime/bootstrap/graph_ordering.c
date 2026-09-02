/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/graph_ordering.c
 *
 * PURPOSE:
 *   Implement the graph ordering behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/graph_ordering.c
 *
 * PURPOSE:
 *   Produce deterministic topological service order from dependency graphs.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/graph_ordering.h"


#include <string.h>

/* Provide the find node index operation used by this module and its client applications. */
static size_t find_node_index(const UmiBootstrapServiceGraph *graph, const char *id) {
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < graph->node_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(graph->nodes[index].id, id) == 0) return index;
    }
    return SIZE_MAX;
}

/*
 * Provide the bootstrap graph order operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_graph_order(const UmiBootstrapServiceGraph *graph,
                                    UmiBootstrapIdList *out_order) {
    size_t indegree[UMI_BOOTSTRAP_MAX_ITEMS] = {0U};
    bool used[UMI_BOOTSTRAP_MAX_ITEMS] = {false};
    size_t emitted = 0U;
    size_t edge_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || out_order == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_order, 0, sizeof(*out_order));
    /* Visit each bounded item once so every record receives the same rule. */
    for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
        size_t dependent = find_node_index(graph, graph->edges[edge_index].dependent_id);
        size_t dependency = find_node_index(graph, graph->edges[edge_index].dependency_id);
        /* Apply this branch only when its contract condition is satisfied. */
        if (dependent == SIZE_MAX || dependency == SIZE_MAX) {
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (graph->edges[edge_index].required) return UMI_STATUS_NOT_FOUND;
            continue;
        }
        /* Apply this operation only while the related capability or state is available. */
        if (graph->nodes[dependent].enabled && graph->nodes[dependency].enabled) {
            ++indegree[dependent];
        }
    }
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (emitted < graph->node_count) {
        size_t candidate = SIZE_MAX;
        size_t index;
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < graph->node_count; ++index) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (!graph->nodes[index].enabled || used[index] || indegree[index] != 0U) continue;
            /* Apply this branch only when its contract condition is satisfied. */
            if (candidate == SIZE_MAX ||
                graph->nodes[index].priority > graph->nodes[candidate].priority ||
                (graph->nodes[index].priority == graph->nodes[candidate].priority &&
                 strcmp(graph->nodes[index].id, graph->nodes[candidate].id) < 0)) {
                candidate = index;
            }
        }
        /* Apply this branch only when its contract condition is satisfied. */
        if (candidate == SIZE_MAX) {
            bool pending_enabled = false;
            /* Visit each bounded item once so every record receives the same rule. */
            for (index = 0U; index < graph->node_count; ++index) {
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (graph->nodes[index].enabled && !used[index]) pending_enabled = true;
                /* Keep the operation inside its valid bounds before reading, writing or adding data. */
                if (!graph->nodes[index].enabled && !used[index]) {
                    used[index] = true;
                    ++emitted;
                }
            }
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (pending_enabled) return UMI_STATUS_INVALID_STATE;
            continue;
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (out_order->count >= UMI_BOOTSTRAP_MAX_ITEMS) return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)umi_bootstrap_copy_text(out_order->ids[out_order->count],
                                      sizeof(out_order->ids[out_order->count]),
                                      graph->nodes[candidate].id);
        ++out_order->count;
        used[candidate] = true;
        ++emitted;
        /* Visit each bounded item once so every record receives the same rule. */
        for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(graph->edges[edge_index].dependency_id,
                       graph->nodes[candidate].id) == 0) {
                size_t dependent = find_node_index(graph, graph->edges[edge_index].dependent_id);
                /* Apply this branch only when its contract condition is satisfied. */
                if (dependent != SIZE_MAX && indegree[dependent] > 0U) --indegree[dependent];
            }
        }
    }
    return UMI_STATUS_OK;
}
