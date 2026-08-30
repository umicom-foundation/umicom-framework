/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/change_set.c
 *
 * PURPOSE:
 *   Collect changed property paths for one atomic update.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_set.h"
#include <string.h>

/* Initialise the change set contract to deterministic zero/default state. */
void umi_ui_reactive_change_set_init(UmiUiReactiveChangeSet *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_change_set_valid(const UmiUiReactiveChangeSet *item) {
    return item != NULL;
}
