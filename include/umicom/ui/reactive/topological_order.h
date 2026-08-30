/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/topological_order.h
 *
 * PURPOSE:
 *   Produce deterministic dependency order for computed-state propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_TOPOLOGICAL_ORDER_H
#define UMICOM_UI_REACTIVE_TOPOLOGICAL_ORDER_H
#include "umicom/ui/reactive/types.h"
#include "umicom/ui/reactive/dependency_graph.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveTopologicalOrder {
    uint16_t order[UMI_UI_REACTIVE_MAX_ITEMS];
    size_t count;
    bool complete;
} UmiUiReactiveTopologicalOrder;
UmiStatus umi_ui_reactive_topological_order_build(const UmiUiReactiveDependencyGraph *graph,UmiUiReactiveTopologicalOrder *out);
#ifdef __cplusplus
}
#endif
#endif
