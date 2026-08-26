/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/command_binding.c
 *
 * PURPOSE:
 *   Connect a semantic command to reactive enablement and parameter state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_binding.h"
#include <string.h>

/* Initialise the command binding contract to deterministic zero/default state. */
void umi_ui_reactive_command_binding_init(UmiUiReactiveCommandBinding *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_command_binding_valid(const UmiUiReactiveCommandBinding *item) {
    return item != NULL;
}
