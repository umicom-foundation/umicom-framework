/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validator.c
 *
 * PURPOSE:
 *   Implement a named validator and the value kind it accepts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validator.h"
#include <string.h>

/* Initialise the validator contract to deterministic zero/default state. */
void umi_ui_reactive_validator_init(UmiUiReactiveValidator *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validator_valid(const UmiUiReactiveValidator *item) {
    return item != NULL;
}
