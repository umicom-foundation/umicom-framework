/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_session.c
 *
 * PURPOSE:
 *   Track binding activation, revision and propagation counts.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_session.h"
#include <string.h>

/* Initialise the binding session contract to deterministic zero/default state. */
void umi_ui_reactive_binding_session_init(UmiUiReactiveBindingSession *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_session_valid(const UmiUiReactiveBindingSession *item) {
    return item != NULL;
}
