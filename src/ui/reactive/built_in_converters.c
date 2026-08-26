/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/built_in_converters.c
 *
 * PURPOSE:
 *   Provide deterministic scalar conversion helpers used by declarative bindings.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/built_in_converters.h"
#include <string.h>

/* Initialise the built in converters contract to deterministic zero/default state. */
void umi_ui_reactive_built_in_converters_init(UmiUiReactiveBuiltInConverters *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_built_in_converters_valid(const UmiUiReactiveBuiltInConverters *item) {
    return item != NULL;
}
