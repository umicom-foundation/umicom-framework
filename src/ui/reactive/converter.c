/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/reactive/converter.c
 *
 * PURPOSE:
 *   Implement a named value converter with forward and reverse availability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/converter.h"
#include <string.h>

/* Initialise the converter contract to deterministic zero/default state. */
void umi_ui_reactive_converter_init(UmiUiReactiveConverter *item) {
    if (item != NULL) memset(item, 0, sizeof *item);
}

/* Validate that the contract pointer is available to a binding/state pipeline. */
int umi_ui_reactive_converter_valid(const UmiUiReactiveConverter *item) {
    return item != NULL;
}
