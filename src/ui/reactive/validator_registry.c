/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/validator_registry.c
 *
 * PURPOSE:
 *   Implement validator identifiers for form and property binding pipelines.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/validator_registry.h"
#include <string.h>

/* Initialise the validator registry contract to deterministic zero/default state. */
void umi_ui_reactive_validator_registry_init(UmiUiReactiveValidatorRegistry *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_validator_registry_valid(const UmiUiReactiveValidatorRegistry *item) {
    return item != NULL;
}
