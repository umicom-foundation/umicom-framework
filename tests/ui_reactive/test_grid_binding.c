/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_grid_binding.c
 *
 * PURPOSE:
 *   Exercise the grid binding reactive UI contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/grid_binding.h"
int main(void) { UmiUiReactiveGridBinding item; umi_ui_reactive_grid_binding_init(&item); return umi_ui_reactive_grid_binding_valid(&item) ? 0 : 1; }
