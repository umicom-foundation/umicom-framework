/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_descriptor.c
 *
 * PURPOSE:
 *   Implement a declarative binding between source and target endpoints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_descriptor.h"
#include <string.h>

/* Initialise the binding descriptor contract to deterministic zero/default state. */
void umi_ui_reactive_binding_descriptor_init(UmiUiReactiveBindingDescriptor *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_descriptor_valid(const UmiUiReactiveBindingDescriptor *item) {
    return item != NULL;
}
