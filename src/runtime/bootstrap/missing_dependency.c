/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/missing_dependency.c
 *
 * PURPOSE:
 *   Implement the missing dependency behavior for
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
 * File: src/runtime/bootstrap/missing_dependency.c
 *
 * PURPOSE:
 *   Count required graph dependencies whose service node is absent or disabled.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/missing_dependency.h"
#include "umicom/runtime/bootstrap/service_graph.h"


#include <string.h>

/*
 * Return the number of records represented by bootstrap graph missing dependency without
 * changing their state.
 */
size_t umi_bootstrap_graph_missing_dependency_count(
    const UmiBootstrapServiceGraph *graph,
    char *out_first_missing,
    size_t capacity) {
    size_t missing = 0U;
    size_t edge_index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_first_missing != NULL && capacity > 0U) out_first_missing[0] = '\0';
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (edge_index = 0U; edge_index < graph->edge_count; ++edge_index) {
        const UmiBootstrapGraphNode *dependency;
        const UmiBootstrapGraphNode *dependent;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!graph->edges[edge_index].required) continue;
        dependency = umi_bootstrap_service_graph_find_node(
            graph, graph->edges[edge_index].dependency_id);
        dependent = umi_bootstrap_service_graph_find_node(
            graph, graph->edges[edge_index].dependent_id);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (dependency == NULL || dependent == NULL || !dependency->enabled || !dependent->enabled) {
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
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
