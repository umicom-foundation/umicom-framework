/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_endpoint.c
 *
 * PURPOSE:
 *   Implement a view/property endpoint without owning the target object.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_endpoint.h"
#include <string.h>

/* Initialise the binding endpoint contract to deterministic zero/default state. */
void umi_ui_reactive_binding_endpoint_init(UmiUiReactiveBindingEndpoint *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_endpoint_valid(const UmiUiReactiveBindingEndpoint *item) {
    return item != NULL;
}
