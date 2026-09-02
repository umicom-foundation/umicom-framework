/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_direction.c
 *
 * PURPOSE:
 *   Implement binding direction and update trigger policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_direction.h"
#include <string.h>

/* Initialise the binding direction contract to deterministic zero/default state. */
void umi_ui_reactive_binding_direction_init(UmiUiReactiveBindingDirectionPolicy *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_direction_valid(const UmiUiReactiveBindingDirectionPolicy *item) {
    return item != NULL;
}
