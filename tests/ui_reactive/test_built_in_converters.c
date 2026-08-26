/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_built_in_converters.c
 *
 * PURPOSE:
 *   Exercise the built in converters reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/built_in_converters.h"
int main(void) { UmiUiReactiveBuiltInConverters item; umi_ui_reactive_built_in_converters_init(&item); return umi_ui_reactive_built_in_converters_valid(&item) ? 0 : 1; }
