/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/command_parameter.c
 *
 * PURPOSE:
 *   Implement a revisioned command parameter value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/command_parameter.h"
#include <string.h>

/* Initialise the command parameter contract to deterministic zero/default state. */
void umi_ui_reactive_command_parameter_init(UmiUiReactiveCommandParameter *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_command_parameter_valid(const UmiUiReactiveCommandParameter *item) {
    return item != NULL;
}
