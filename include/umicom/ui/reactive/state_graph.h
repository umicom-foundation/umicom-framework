/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_graph.h
 *
 * PURPOSE:
 *   Represent aggregate dependency/state graph health.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_GRAPH_H
#define UMICOM_UI_REACTIVE_STATE_GRAPH_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveStateGraph {
    size_t node_count;
    size_t edge_count;
    size_t computed_count;
    bool acyclic;
    uint64_t revision;
} UmiUiReactiveStateGraph;
void umi_ui_reactive_state_graph_init(UmiUiReactiveStateGraph *item);
int umi_ui_reactive_state_graph_valid(const UmiUiReactiveStateGraph *item);
#ifdef __cplusplus
}
#endif
#endif
