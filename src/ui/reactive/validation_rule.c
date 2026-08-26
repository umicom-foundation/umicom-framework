/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validation_rule.c
 *
 * PURPOSE:
 *   Implement common required/range/length validation constraints.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validation_rule.h"
#include <string.h>

/* Initialise the validation rule contract to deterministic zero/default state. */
void umi_ui_reactive_validation_rule_init(UmiUiReactiveValidationRule *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validation_rule_valid(const UmiUiReactiveValidationRule *item) {
    return item != NULL;
}
