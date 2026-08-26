/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_converter_registry.c
 *
 * PURPOSE:
 *   Exercise the converter registry reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/converter_registry.h"
int main(void) { UmiUiReactiveConverterRegistry item; umi_ui_reactive_converter_registry_init(&item); return umi_ui_reactive_converter_registry_valid(&item) ? 0 : 1; }
