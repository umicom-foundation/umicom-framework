/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/visibility_rule.c
 *
 * PURPOSE:
 *   Bind component visibility to declarative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/visibility_rule.h"
#include <string.h>

/* Initialise the visibility rule contract to deterministic zero/default state. */
void umi_ui_reactive_visibility_rule_init(UmiUiReactiveVisibilityRule *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_visibility_rule_valid(const UmiUiReactiveVisibilityRule *item) {
    return item != NULL;
}
