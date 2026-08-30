/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/cycle_detection.h
 *
 * PURPOSE:
 *   Detect cyclic state dependencies before propagation is enabled.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_CYCLE_DETECTION_H
#define UMICOM_UI_REACTIVE_CYCLE_DETECTION_H
#include "umicom/ui/reactive/types.h"
#include "umicom/ui/reactive/dependency_graph.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveCycleDetection {
    bool has_cycle;
    size_t visited_nodes;
} UmiUiReactiveCycleDetection;
UmiStatus umi_ui_reactive_cycle_detection_run(const UmiUiReactiveDependencyGraph *graph,UmiUiReactiveCycleDetection *out);
#ifdef __cplusplus
}
#endif
#endif
