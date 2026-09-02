/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/list_binding.c
 *
 * PURPOSE:
 *   Implement list data-path and selection-path binding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/list_binding.h"
#include <string.h>

/* Initialise the list binding contract to deterministic zero/default state. */
void umi_ui_reactive_list_binding_init(UmiUiReactiveListBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_list_binding_valid(const UmiUiReactiveListBinding *item) {
    return item != NULL;
}
