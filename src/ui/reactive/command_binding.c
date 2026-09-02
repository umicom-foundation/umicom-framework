/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/command_binding.c
 *
 * PURPOSE:
 *   Connect a semantic command to reactive enablement and parameter state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_binding.h"
#include <string.h>

/* Initialise the command binding contract to deterministic zero/default state. */
void umi_ui_reactive_command_binding_init(UmiUiReactiveCommandBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_command_binding_valid(const UmiUiReactiveCommandBinding *item) {
    return item != NULL;
}
