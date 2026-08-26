/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/context_binding.c
 *
 * PURPOSE:
 *   Synchronise typed context channels with declarative UI properties.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/context_binding.h"
#include <string.h>

/* Initialise the context binding contract to deterministic zero/default state. */
void umi_ui_reactive_context_binding_init(UmiUiReactiveContextBinding *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_context_binding_valid(const UmiUiReactiveContextBinding *item) {
    return item != NULL;
}
