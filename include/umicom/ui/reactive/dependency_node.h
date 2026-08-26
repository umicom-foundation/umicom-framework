/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/dependency_node.h
 *
 * PURPOSE:
 *   Represent one property/computed-state node in the dependency graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_DEPENDENCY_NODE_H
#define UMICOM_UI_REACTIVE_DEPENDENCY_NODE_H
#include "umicom/ui/reactive/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiReactiveDependencyNode {
    char node_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool computed;
    uint64_t revision;
} UmiUiReactiveDependencyNode;
void umi_ui_reactive_dependency_node_init(UmiUiReactiveDependencyNode *item);
int umi_ui_reactive_dependency_node_valid(const UmiUiReactiveDependencyNode *item);
#ifdef __cplusplus
}
#endif
#endif
