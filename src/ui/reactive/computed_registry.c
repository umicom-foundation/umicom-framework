/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/computed_registry.c
 *
 * PURPOSE:
 *   Implement bounded derived-state identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/computed_registry.h"
#include <string.h>

/* Initialise the computed registry contract to deterministic zero/default state. */
void umi_ui_reactive_computed_registry_init(UmiUiReactiveComputedRegistry *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_computed_registry_valid(const UmiUiReactiveComputedRegistry *item) {
    return item != NULL;
}
