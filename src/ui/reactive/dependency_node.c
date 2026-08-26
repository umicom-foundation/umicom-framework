/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/dependency_node.c
 *
 * PURPOSE:
 *   Implement one property/computed-state node in the dependency graph.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_node.h"
#include <string.h>

/* Initialise the dependency node contract to deterministic zero/default state. */
void umi_ui_reactive_dependency_node_init(UmiUiReactiveDependencyNode *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_dependency_node_valid(const UmiUiReactiveDependencyNode *item) {
    return item != NULL;
}
