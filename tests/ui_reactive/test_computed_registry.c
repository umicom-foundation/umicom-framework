/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_computed_registry.c
 *
 * PURPOSE:
 *   Exercise the computed registry reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/computed_registry.h"
int main(void) { UmiUiReactiveComputedRegistry item; umi_ui_reactive_computed_registry_init(&item); return umi_ui_reactive_computed_registry_valid(&item) ? 0 : 1; }
