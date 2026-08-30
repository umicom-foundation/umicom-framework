/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/command_enablement.c
 *
 * PURPOSE:
 *   Implement command enablement evidence from a state expression.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_enablement.h"
#include <string.h>

/* Initialise the command enablement contract to deterministic zero/default state. */
void umi_ui_reactive_command_enablement_init(UmiUiReactiveCommandEnablement *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_command_enablement_valid(const UmiUiReactiveCommandEnablement *item) {
    return item != NULL;
}
