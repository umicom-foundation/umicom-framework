/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/dependency_edge.c
 *
 * PURPOSE:
 *   Implement a directed reactive dependency edge.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/dependency_edge.h"
#include <string.h>

/* Initialise the dependency edge contract to deterministic zero/default state. */
void umi_ui_reactive_dependency_edge_init(UmiUiReactiveDependencyEdge *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_dependency_edge_valid(const UmiUiReactiveDependencyEdge *item) {
    return item != NULL;
}
