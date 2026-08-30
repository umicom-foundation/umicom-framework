/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validation_result.c
 *
 * PURPOSE:
 *   Implement deterministic validation outcome and user-facing message.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_result.h"
#include <string.h>

/* Initialise the validation result contract to deterministic zero/default state. */
void umi_ui_reactive_validation_result_init(UmiUiReactiveValidationResult *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validation_result_valid(const UmiUiReactiveValidationResult *item) {
    return item != NULL;
}
