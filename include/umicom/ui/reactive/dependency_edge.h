/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/dependency_edge.h
 *
 * PURPOSE:
 *   Represent a directed reactive dependency edge.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_DEPENDENCY_EDGE_H
#define UMICOM_UI_REACTIVE_DEPENDENCY_EDGE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive dependency edge data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveDependencyEdge {
    char from_id[UMI_UI_REACTIVE_ID_CAPACITY];
    char to_id[UMI_UI_REACTIVE_ID_CAPACITY];
} UmiUiReactiveDependencyEdge;
/**
 * Initialise ui reactive dependency edge from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_dependency_edge_init(UmiUiReactiveDependencyEdge *item);
/**
 * Check that ui reactive dependency edge satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_dependency_edge_valid(const UmiUiReactiveDependencyEdge *item);
#ifdef __cplusplus
}
#endif
#endif
