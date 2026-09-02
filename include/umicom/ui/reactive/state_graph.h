/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_graph.h
 *
 * PURPOSE:
 *   Represent aggregate dependency/state graph health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_GRAPH_H
#define UMICOM_UI_REACTIVE_STATE_GRAPH_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive state graph data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveStateGraph {
    size_t node_count;
    size_t edge_count;
    size_t computed_count;
    bool acyclic;
    uint64_t revision;
} UmiUiReactiveStateGraph;
/**
 * Initialise ui reactive state graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_state_graph_init(UmiUiReactiveStateGraph *item);
/**
 * Check that ui reactive state graph satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_state_graph_valid(const UmiUiReactiveStateGraph *item);
#ifdef __cplusplus
}
#endif
#endif
