/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/event_binding.c
 *
 * PURPOSE:
 *   Route a semantic UI event to a command or state action.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/event_binding.h"
#include <string.h>

/* Initialise the event binding contract to deterministic zero/default state. */
void umi_ui_reactive_event_binding_init(UmiUiReactiveEventBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_event_binding_valid(const UmiUiReactiveEventBinding *item) {
    return item != NULL;
}
