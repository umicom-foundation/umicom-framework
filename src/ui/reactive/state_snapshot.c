/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/state_snapshot.c
 *
 * PURPOSE:
 *   Implement a named immutable state snapshot reference.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/state_snapshot.h"
#include <string.h>

/* Initialise the state snapshot contract to deterministic zero/default state. */
void umi_ui_reactive_state_snapshot_init(UmiUiReactiveStateSnapshot *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_state_snapshot_valid(const UmiUiReactiveStateSnapshot *item) {
    return item != NULL;
}
