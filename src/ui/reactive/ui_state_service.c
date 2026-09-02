/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/ui_state_service.c
 *
 * PURPOSE:
 *   Implement aggregate readiness of binding, validation and state-graph services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/ui_state_service.h"
#include <string.h>

/* Initialise the ui state service contract to deterministic zero/default state. */
void umi_ui_reactive_ui_state_service_init(UmiUiReactiveUiStateService *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_ui_state_service_valid(const UmiUiReactiveUiStateService *item) {
    return item != NULL;
}
