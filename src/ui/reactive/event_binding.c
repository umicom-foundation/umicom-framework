/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/event_binding.c
 *
 * PURPOSE:
 *   Route a semantic UI event to a command or state action.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/event_binding.h"
#include <string.h>

/* Initialise the event binding contract to deterministic zero/default state. */
void umi_ui_reactive_event_binding_init(UmiUiReactiveEventBinding *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_event_binding_valid(const UmiUiReactiveEventBinding *item) {
    return item != NULL;
}
