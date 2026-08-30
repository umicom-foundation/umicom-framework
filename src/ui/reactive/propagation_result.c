/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/propagation_result.c
 *
 * PURPOSE:
 *   Summarise propagated, skipped and failed binding operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/propagation_result.h"
#include <string.h>

/* Initialise the propagation result contract to deterministic zero/default state. */
void umi_ui_reactive_propagation_result_init(UmiUiReactivePropagationResult *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_propagation_result_valid(const UmiUiReactivePropagationResult *item) {
    return item != NULL;
}
