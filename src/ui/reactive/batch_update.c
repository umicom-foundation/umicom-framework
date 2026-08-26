/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/batch_update.c
 *
 * PURPOSE:
 *   Aggregate state mutations into one revision boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/batch_update.h"
#include <string.h>

/* Initialise the batch update contract to deterministic zero/default state. */
void umi_ui_reactive_batch_update_init(UmiUiReactiveBatchUpdate *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_batch_update_valid(const UmiUiReactiveBatchUpdate *item) {
    return item != NULL;
}
