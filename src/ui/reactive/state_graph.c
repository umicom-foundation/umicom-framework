/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/state_graph.c
 *
 * PURPOSE:
 *   Implement aggregate dependency/state graph health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_graph.h"
#include <string.h>

/* Initialise the state graph contract to deterministic zero/default state. */
void umi_ui_reactive_state_graph_init(UmiUiReactiveStateGraph *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_state_graph_valid(const UmiUiReactiveStateGraph *item) {
    return item != NULL;
}
