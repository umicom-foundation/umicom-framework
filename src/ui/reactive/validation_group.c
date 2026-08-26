/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validation_group.c
 *
 * PURPOSE:
 *   Aggregate validation results for one form, object or editing transaction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_group.h"
#include <string.h>

/* Initialise the validation group contract to deterministic zero/default state. */
void umi_ui_reactive_validation_group_init(UmiUiReactiveValidationGroup *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validation_group_valid(const UmiUiReactiveValidationGroup *item) {
    return item != NULL;
}
