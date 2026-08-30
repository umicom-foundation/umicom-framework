/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/propagation_plan.c
 *
 * PURPOSE:
 *   Implement ordered binding identifiers for one propagation pass.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_plan.h"
#include <string.h>

/* Initialise the propagation plan contract to deterministic zero/default state. */
void umi_ui_reactive_propagation_plan_init(UmiUiReactivePropagationPlan *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_propagation_plan_valid(const UmiUiReactivePropagationPlan *item) {
    return item != NULL;
}
