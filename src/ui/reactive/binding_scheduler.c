/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/binding_scheduler.c
 *
 * PURPOSE:
 *   Track pending propagation work and deterministic dispatch generation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/binding_scheduler.h"
#include <string.h>

/* Initialise the binding scheduler contract to deterministic zero/default state. */
void umi_ui_reactive_binding_scheduler_init(UmiUiReactiveBindingScheduler *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_binding_scheduler_valid(const UmiUiReactiveBindingScheduler *item) {
    return item != NULL;
}
