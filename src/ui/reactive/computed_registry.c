/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/computed_registry.c
 *
 * PURPOSE:
 *   Implement bounded derived-state identifiers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/computed_registry.h"
#include <string.h>

/* Initialise the computed registry contract to deterministic zero/default state. */
void umi_ui_reactive_computed_registry_init(UmiUiReactiveComputedRegistry *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_computed_registry_valid(const UmiUiReactiveComputedRegistry *item) {
    return item != NULL;
}
