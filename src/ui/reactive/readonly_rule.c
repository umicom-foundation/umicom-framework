/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/readonly_rule.c
 *
 * PURPOSE:
 *   Bind editor read-only state to declarative state.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/readonly_rule.h"
#include <string.h>

/* Initialise the readonly rule contract to deterministic zero/default state. */
void umi_ui_reactive_readonly_rule_init(UmiUiReactiveReadonlyRule *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_readonly_rule_valid(const UmiUiReactiveReadonlyRule *item) {
    return item != NULL;
}
