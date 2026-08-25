/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/missing_dependency.c
 *
 * PURPOSE:
 *   Count required graph dependencies whose service node is absent or disabled.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/missing_dependency.h"
#include "umicom/runtime/bootstrap/service_graph.h"


#include <string.h>

size_t umi_bootstrap_graph_missing_dependency_count(
    const UmiBootstrapServiceGraph *graph,
    char *out_first_missing,
    size_t capacity) {
    size_t missing = 0U;
    size_t edge_index;
    if (out_first_missing != NULL && capacity > 0U) out_first_missing[0] = '\0';
    if (graph == NULL) return 0U;
    for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
        const UmiBootstrapGraphNode *dependency;
        const UmiBootstrapGraphNode *dependent;
        if (!graph->edges[edge_index].required) continue;
        dependency = umi_bootstrap_service_graph_find_node(
            graph, graph->edges[edge_index].dependency_id);
        dependent = umi_bootstrap_service_graph_find_node(
            graph, graph->edges[edge_index].dependent_id);
        if (dependency == NULL || dependent == NULL || !dependency->enabled || !dependent->enabled) {
            if (missing == 0U && out_first_missing != NULL && capacity > 0U) {
                (void)umi_bootstrap_copy_text(out_first_missing, capacity,
                    dependency == NULL ? graph->edges[edge_index].dependency_id :
                    graph->edges[edge_index].dependent_id);
            }
            ++missing;
        }
    }
    return missing;
}
