/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/change_event.c
 *
 * PURPOSE:
 *   Implement one observable property change with monotonic sequence metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/change_event.h"
#include <string.h>

/* Initialise the change event contract to deterministic zero/default state. */
void umi_ui_reactive_change_event_init(UmiUiReactiveChangeEvent *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_change_event_valid(const UmiUiReactiveChangeEvent *item) {
    return item != NULL;
}
