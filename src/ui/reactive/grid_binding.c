/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/grid_binding.c
 *
 * PURPOSE:
 *   Implement enterprise grid provider/query/selection binding paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/grid_binding.h"
#include <string.h>

/* Initialise the grid binding contract to deterministic zero/default state. */
void umi_ui_reactive_grid_binding_init(UmiUiReactiveGridBinding *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_grid_binding_valid(const UmiUiReactiveGridBinding *item) {
    return item != NULL;
}
