/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/required_rule.c
 *
 * PURPOSE:
 *   Bind form required-state to declarative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/required_rule.h"
#include <string.h>

/* Initialise the required rule contract to deterministic zero/default state. */
void umi_ui_reactive_required_rule_init(UmiUiReactiveRequiredRule *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_required_rule_valid(const UmiUiReactiveRequiredRule *item) {
    return item != NULL;
}
