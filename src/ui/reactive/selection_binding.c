/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/selection_binding.c
 *
 * PURPOSE:
 *   Synchronise canonical selection context with a semantic surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/selection_binding.h"
#include <string.h>

/* Initialise the selection binding contract to deterministic zero/default state. */
void umi_ui_reactive_selection_binding_init(UmiUiReactiveSelectionBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_selection_binding_valid(const UmiUiReactiveSelectionBinding *item) {
    return item != NULL;
}
