/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/form_binding.c
 *
 * PURPOSE:
 *   Implement form-level model binding and commit policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/form_binding.h"
#include <string.h>

/* Initialise the form binding contract to deterministic zero/default state. */
void umi_ui_reactive_form_binding_init(UmiUiReactiveFormBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_form_binding_valid(const UmiUiReactiveFormBinding *item) {
    return item != NULL;
}
