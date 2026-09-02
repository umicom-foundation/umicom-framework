/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_reactive/test_grid_binding.c
 *
 * PURPOSE:
 *   Exercise the grid binding reactive UI contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/reactive/grid_binding.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiUiReactiveGridBinding item; umi_ui_reactive_grid_binding_init(&item); return umi_ui_reactive_grid_binding_valid(&item) ? 0 : 1; }
