/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/computed_value.c
 *
 * PURPOSE:
 *   Implement derived state and its last computed revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/computed_value.h"
#include <string.h>

/* Initialise the computed value contract to deterministic zero/default state. */
void umi_ui_reactive_computed_value_init(UmiUiReactiveComputedValue *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_computed_value_valid(const UmiUiReactiveComputedValue *item) {
    return item != NULL;
}
