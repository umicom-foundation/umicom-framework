/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/cycle_report.c
 *
 * PURPOSE:
 *   Implement the cycle report behavior for
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
 * File: src/runtime/bootstrap/cycle_report.c
 *
 * PURPOSE:
 *   Extract a representative cycle path from a service dependency graph.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/cycle_report.h"


#include <string.h>

/* Provide the node index operation used by this module and its client applications. */
static size_t node_index(const UmiBootstrapServiceGraph *graph, const char *id) {
    size_t i;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < graph->node_count; ++i)
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(graph->nodes[i].id, id) == 0) return i;
    return SIZE_MAX;
}

/* Provide the visit operation used by this module and its client applications. */
static bool visit(const UmiBootstrapServiceGraph *graph, size_t current,
                  uint8_t *marks, size_t *stack, size_t *depth,
                  UmiBootstrapIdList *out_cycle) {
    size_t e;
    marks[current] = 1U;
    stack[(*depth)++] = current;
    /* Visit each bounded item once so every record receives the same rule. */
    for (e = 0U; e < graph->edge_count; ++e) {
        size_t next;
        size_t s;
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(graph->edges[e].dependency_id, graph->nodes[current].id) != 0) continue;
        next = node_index(graph, graph->edges[e].dependent_id);
        /* Apply this operation only while the related capability or state is available. */
        if (next == SIZE_MAX || !graph->nodes[next].enabled) continue;
        /* Apply this branch only when its contract condition is satisfied. */
        if (marks[next] == 0U && visit(graph, next, marks, stack, depth, out_cycle)) return true;
        /* Apply this branch only when its contract condition is satisfied. */
        if (marks[next] == 1U) {
            /* Visit each bounded item once so every record receives the same rule. */
            for (s = 0U; s < *depth; ++s) {
                /* Apply this branch only when its contract condition is satisfied. */
                if (stack[s] == next) {
                    /* Visit each bounded item once so every record receives the same rule. */
                    for (; s < *depth && out_cycle->count < UMI_BOOTSTRAP_MAX_ITEMS; ++s) {
                        (void)umi_bootstrap_copy_text(out_cycle->ids[out_cycle->count++],
                            UMI_BOOTSTRAP_ID_CAPACITY, graph->nodes[stack[s]].id);
                    }
                    (void)umi_bootstrap_copy_text(out_cycle->ids[out_cycle->count++],
                        UMI_BOOTSTRAP_ID_CAPACITY, graph->nodes[next].id);
                    return true;
                }
            }
        }
    }
    --(*depth);
    marks[current] = 2U;
    return false;
}

/*
 * Provide the bootstrap graph cycle report operation used by this module and its client
 * applications.
 */
UmiStatus umi_bootstrap_graph_cycle_report(
    const UmiBootstrapServiceGraph *graph,
    UmiBootstrapIdList *out_cycle) {
    uint8_t marks[UMI_BOOTSTRAP_MAX_ITEMS] = {0U};
    size_t stack[UMI_BOOTSTRAP_MAX_ITEMS] = {0U};
    size_t depth = 0U;
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || out_cycle == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_cycle, 0, sizeof(*out_cycle));
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < graph->node_count; ++i) {
        /* Apply this operation only while the related capability or state is available. */
        if (graph->nodes[i].enabled && marks[i] == 0U &&
            visit(graph, i, marks, stack, &depth, out_cycle)) return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
