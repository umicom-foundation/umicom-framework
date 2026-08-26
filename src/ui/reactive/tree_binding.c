/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/tree_binding.c
 *
 * PURPOSE:
 *   Implement tree provider/expansion/selection binding paths.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/tree_binding.h"
#include <string.h>

/* Initialise the tree binding contract to deterministic zero/default state. */
void umi_ui_reactive_tree_binding_init(UmiUiReactiveTreeBinding *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_tree_binding_valid(const UmiUiReactiveTreeBinding *item) {
    return item != NULL;
}
