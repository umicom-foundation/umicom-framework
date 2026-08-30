/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/inspector_binding.c
 *
 * PURPOSE:
 *   Implement inspector subject and editing binding paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/inspector_binding.h"
#include <string.h>

/* Initialise the inspector binding contract to deterministic zero/default state. */
void umi_ui_reactive_inspector_binding_init(UmiUiReactiveInspectorBinding *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_inspector_binding_valid(const UmiUiReactiveInspectorBinding *item) {
    return item != NULL;
}
