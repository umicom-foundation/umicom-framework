/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/converter_registry.c
 *
 * PURPOSE:
 *   Implement converter descriptors by stable identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/converter_registry.h"
#include <string.h>

/* Initialise the converter registry contract to deterministic zero/default state. */
void umi_ui_reactive_converter_registry_init(UmiUiReactiveConverterRegistry *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_converter_registry_valid(const UmiUiReactiveConverterRegistry *item) {
    return item != NULL;
}
