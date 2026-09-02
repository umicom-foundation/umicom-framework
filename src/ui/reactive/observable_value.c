/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/observable_value.c
 *
 * PURPOSE:
 *   Hold a revisioned value and dirty state for reactive propagation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/observable_value.h"
#include <string.h>

/* Initialise the observable value contract to deterministic zero/default state. */
void umi_ui_reactive_observable_value_init(UmiUiReactiveObservableValue *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_observable_value_valid(const UmiUiReactiveObservableValue *item) {
    return item != NULL;
}
