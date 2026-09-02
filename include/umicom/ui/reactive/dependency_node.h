/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/dependency_node.h
 *
 * PURPOSE:
 *   Represent one property/computed-state node in the dependency graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_DEPENDENCY_NODE_H
#define UMICOM_UI_REACTIVE_DEPENDENCY_NODE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive dependency node data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveDependencyNode {
    char node_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool computed;
    uint64_t revision;
} UmiUiReactiveDependencyNode;
/**
 * Initialise ui reactive dependency node from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_dependency_node_init(UmiUiReactiveDependencyNode *item);
/**
 * Check that ui reactive dependency node satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_dependency_node_valid(const UmiUiReactiveDependencyNode *item);
#ifdef __cplusplus
}
#endif
#endif
