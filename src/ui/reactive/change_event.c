/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/change_event.c
 *
 * PURPOSE:
 *   Implement one observable property change with monotonic sequence metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_event.h"
#include <string.h>

/* Initialise the change event contract to deterministic zero/default state. */
void umi_ui_reactive_change_event_init(UmiUiReactiveChangeEvent *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_change_event_valid(const UmiUiReactiveChangeEvent *item) {
    return item != NULL;
}
