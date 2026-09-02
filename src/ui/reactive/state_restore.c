/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/state_restore.c
 *
 * PURPOSE:
 *   Implement governed state restoration intent and result.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_restore.h"
#include <string.h>

/* Initialise the state restore contract to deterministic zero/default state. */
void umi_ui_reactive_state_restore_init(UmiUiReactiveStateRestore *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_state_restore_valid(const UmiUiReactiveStateRestore *item) {
    return item != NULL;
}
