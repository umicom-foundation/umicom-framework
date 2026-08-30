/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/enablement_rule.c
 *
 * PURPOSE:
 *   Bind component enablement to declarative state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/enablement_rule.h"
#include <string.h>

/* Initialise the enablement rule contract to deterministic zero/default state. */
void umi_ui_reactive_enablement_rule_init(UmiUiReactiveEnablementRule *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_enablement_rule_valid(const UmiUiReactiveEnablementRule *item) {
    return item != NULL;
}
