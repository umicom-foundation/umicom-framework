/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_computed_value.c
 *
 * PURPOSE:
 *   Exercise the computed value reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/computed_value.h"
int main(void) { UmiUiReactiveComputedValue item; umi_ui_reactive_computed_value_init(&item); return umi_ui_reactive_computed_value_valid(&item) ? 0 : 1; }
